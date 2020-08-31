#ifndef ast_binary
#define ast_binary

// Defines classes for various binary operators:
//    - Binary operator superclass: line 22
//    - Assignment operator class: line 204
//    - Array assignment operator class: line 348
//    - += operator class: line 399
//    - -= operator class: line 492
//    - *= operator class: line 585
//    - /= operator class: line 681
//    - %= operator class: line 771
//    - <<= operator class: line 833
//    - >>= operator class: line 900
//    - &= operator class: line 967
//    - |= operator class: line 1032
//    - ^= operator class: line 1099


//SUPERCLASS

class BinaryOperator : public Program
{
  public:
    BinaryOperator(ProgramPtr leftop, ProgramPtr rightop): left(leftop), right(rightop) {}

    virtual ~BinaryOperator()
    {
      delete left;
      delete right;
    }

    std::string Whatami() const
    {
      return "BinaryOperator";
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      return left->findstrings(slabel) + right->findstrings(slabel);
    }

    virtual int maxparams(Context& c) const
    {
      int l = left->maxparams(c), r = right->maxparams(c);
      if(l >= r)
      { return l; }
      else
      { return r; }
    }

    // Used to store left and right operands of binary operator into registers.
    // optype == 0 indicates that the datatype is non floating point.
    void setupLR(Context& c, std::ostream& os, int sp_off, int optype) const
    {
      std::string Rl, Rr, Streg;
      if(optype == 0)
      { Rl = "$8"; Rr = "$9"; Streg = "$16";  }
      else
      { Rl = "$f4"; Rr = "$f6"; Streg = "$f20"; }
      std::string iam_l = left->Whatami();
      std::string iam_r = right->Whatami();
      // In case LHS involves function call
      if(left->maxparams(c) >= 0)
      {
        left->MIPSPrint(c, os, sp_off, Rl);
        if((right->maxparams(c) >= 0)&&(optype == 0))
        { os << "move       " << Streg << ", " << Rl << std::endl;}
        else if((right->maxparams(c) >= 0)&&(optype == 1))
        { os << "mov.s      " << Streg << ", " << Rl << std::endl;}
        else if(!IsOperand(iam_r))
        {
          stpushreg(Rl, os);
          if(optype == 2)
          {
            stpushreg("$f5", os);
          }
        }
        /////////////////////////////
        right->MIPSPrint(c, os, sp_off, Rr);
        ////////////////////////////
        // If right hand side also involves a function call
        if((right->maxparams(c) >= 0)&&(optype == 0))
        { os << "move       " << Rl <<", " << Streg << std::endl;}
        else if((right->maxparams(c) >= 0)&&(optype == 1))
        { os << "mov.s      " << Rl << ", " << Streg << std::endl;}
        else if(!IsOperand(iam_r))
        {
          if(optype == 2)
          {
            stpopreg("$f5", os);
          }
          stpopreg(Rl, os);
        }
      }
      else
      {
        // If right hand side involves a function call.
        if(right->maxparams(c) >= 0)
        {
          right->MIPSPrint(c, os, sp_off, Rr);
          if(!IsOperand(iam_l))
          {
            stpushreg(Rr, os);
            if(optype == 2)
            {
              stpushreg("$f5", os);
            }
          }
          left->MIPSPrint(c, os, sp_off, Rl);
          if(!IsOperand(iam_l))
          {
            if(optype == 2)
            {
              stpopreg("$f5", os);
            }
            stpopreg(Rr, os);
          }
        }
        else if((IsOperand(iam_l))&&(!IsOperand(iam_r)))
        {
          right->MIPSPrint(c, os, sp_off, Rr);
          left->MIPSPrint(c, os, sp_off, Rl);
        }
        // If we need to evaluate both LHS and RHS expressions
        else if((!IsOperand(iam_r))&&(!IsOperand(iam_l)))
        {
          left->MIPSPrint(c, os, sp_off, Rl);
          stpushreg(Rl, os);
          if(optype == 2)
          {
            stpushreg("$f5", os);
          }
          right->MIPSPrint(c, os, sp_off, Rr);
          if(optype == 2)
          {
            stpopreg("$f5", os);
          }
          stpopreg(Rl, os);
        }
        else
        {
          left->MIPSPrint(c, os, sp_off, Rl);
          right->MIPSPrint(c, os, sp_off, Rr);
        }
      }
    }

    // In case we perform += (for example) to a pointer, we need to multiply by a particular value to obtain the address.
    // e.g. 4 for an int. This is found using findShift.
    int findShift(Context& c, int sp_off, bool l) const
    {
      int shift = 2;
      if(l)
      {
        std::string tl = left->MyType(c);
        if(tl == "double *")
        { shift = 3; }
        else if((c.assign_str).find(left->Name()) != (c.assign_str).end()) { shift = 0; }
        else if(left->Whatami() == "Variable")
        {
          if((sp_off - c.variablelist[left->Name()]) >= c.framesize)
          {
            if((tl == "short *")||(tl == "unsigned short *")||(tl == "signed short *"))
            { shift = 1; }
            else if((tl == "char *")||(tl == "unsigned char *")||(tl == "signed char *"))
            { shift = 0; }
          }
        }
        return shift;
      }
      else
      {
        std::string tr = right->MyType(c);
        if(tr == "double *")
        { shift = 3; }
        else if((c.assign_str).find(right->Name()) != (c.assign_str).end()) { shift = 0; }
        else if(right->Whatami() == "Variable")
        {
          if((sp_off - c.variablelist[right->Name()]) >= c.framesize)
          {
            if((tr == "short *")||(tr == "unsigned short *")||(tr == "signed short *"))
            { shift = 1; }
            else if((tr == "char *")||(tr == "unsigned char *")||(tr == "signed char *"))
            { shift = 0; }
          }
        }
        return shift;
      }
    }


    virtual std::string MyType(Context& c) const
    {
      return "int";
    }
  protected:
    ProgramPtr left, right;
};

//---------------------------------------------------------------------------------------------
//ASSIGNMENT OPERATORS

class Assignment : public BinaryOperator
{
  public:
    Assignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      if(right->Whatami() == "StringLiteral")
      { return "StrAssignment"; }
      return "Assignment";
    }

    std::string Name() const
    {
      return left->Name();
    }

    int Value(Context& c) const
    {
        if(right->Whatami() == "IntLiteral")
        {
          return right->intvalue(c);
        }
        return 0;
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " = ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if(destreg != "global")
      {
        std::string type;
        if((left->Whatami() == "Variable")||(left->Whatami() == "ArraySubscript"))
        {
          type = reducetype(c.userdefs, c.vartype[left->Name()]);
        }
        else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
        {
          type = left->MyType(c);
        }
        int addr = c.variablelist[left->Name()];
        if((type == "double")||(type == "float"))
        {
          right->MIPSPrint(c, os, sp_off, "$f4");
          if(destreg[0] == '$')
          {
            if(type == "double")
            { os << "mov.d      " << destreg << ", $f4" << std::endl; }
            else
            { os << "mov.s      " << destreg << ", $f4" << std::endl; }
          }
        }
        else if(((type == "double *")||(type == "float *"))&&(left->Whatami() == "ArraySubscript"))
        {
          right->MIPSPrint(c, os, sp_off, "$f4");
          if(destreg[0] == '$')
          {
            if(type == "double *")
            { os << "mov.d      " << destreg << ", $f4" << std::endl; }
            else
            { os << "mov.s      " << destreg << ", $f4" << std::endl; }
          }
        }
        else
        {
          right->MIPSPrint(c, os, sp_off, "$8");
          if(destreg[0] == '$')
          { os << "move       " << destreg << ", $8" << std::endl;}
        }
        if((type == "char *")&&(right->Whatami() == "StringLiteral"))
        {
          c.assign_str[left->Name()] = right->compsize();
          assignment(addr, sp_off, type, os);
        }
        else if(left->Whatami() == "ArraySubscript")
        {
          ArraySubscript* ar = (ArraySubscript*)left;
          ar->ArSubassign(c, os, sp_off);
        }
        else if(left->Whatami() == "PointerDeref")
        {
          PointerDeref* pd = (PointerDeref*)left;
          pd->pdassign(c, os, sp_off);
        }
        else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
        {
          MemberRef* mr = (MemberRef*)left;
          mr->MRassign(c, os, sp_off);
        }
        else
        {
          if(addr <= -1000)
          { gvassign(left->Name(), type, os); }
          else
          { assignment(addr, sp_off, type, os); }
        }
      }
      else
      {
          std::string type = reducetype(c.userdefs, right->MyType(c));
          int size = 8;
          if(type != "double")
          {  size = 4; }
          os << "           .globl " << left->Name() << std::endl;
          if(type != "double")
          { os << "           .align 2" << std::endl; }
          else
          { os << "           .align 3" << std::endl; }
          os << "           .type " << left->Name() << ", @object" << std::endl;
          os << "           .size " << left->Name() << ", " << size << std::endl;
          os << left->Name() << ":" << std::endl;
          if(type == "double")
          {
            dbConv myValue;
            myValue.dValue = right->dblvalue(c);
            unsigned int low = myValue.INTValue&(0xFFFFFFFF);
            myValue.INTValue >>= 32;
            unsigned int upp = myValue.INTValue&(0xFFFFFFFF);
            os << "           .word " << upp << std::endl;
            os << "           .word " << low << std::endl;
          }
          else if(type == "float")
          {
            fpconv myValue;
            myValue.fValue = (float)right->dblvalue(c);
            os << "           .word " << myValue.iValue << std::endl;
          }
          else
          {
            os << "           .word " << right->intvalue(c);
          }

      }
    }
};

//---------------------------------------------------------------------------------------------------------

class ArrayAssignment : public BinaryOperator
{
  public:
    ArrayAssignment(ProgramPtr leftarray, ProgramPtr rightblock): BinaryOperator(leftarray, rightblock) {}

    std::string Whatami() const
    {
      return "ArrayAssignment";
    }

    std::string Name() const
    {
      return left->Name();
    }

    int compsize() const
    {
      return left->compsize();
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      // in case array assigned globally
      if(destreg != "global")
      {
        int crvf = c.vpframe;
        c.vpframe = sp_off - c.variablelist[left->Name()];
        right->MIPSPrint(c, os, sp_off, "ArrayAssignment");
        c.vpframe = crvf;
      }
      else
      {
        std::string type = reducetype(c.userdefs, c.vartype[left->Name()]);
        int size = 8;
        if(type != "double")
        {  size = 4; }
        os << "           .globl " << left->Name() << std::endl;
        if(type != "double")
        { os << "           .align 2" << std::endl; }
        else
        { os << "           .align 3" << std::endl; }
        os << "           .type " << left->Name() << ", @object" << std::endl;
        os << "           .size " << left->Name() << ", " << size*right->compsize() << std::endl;
        os << left->Name() << ":" << std::endl;
        right->MIPSPrint(c, os, sp_off, destreg);
      }
    }
};

//-----------------------------------------------------------------------------------------------------

class AddAssignment : public BinaryOperator
{
  public:
    AddAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "AddAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " += ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string type = reducetype(c.userdefs, left->MyType(c));
      std::string rl, rr;
      std::string command, mve;
      if(type == "float")
      {
        rl = "$f4"; rr = "$f6";
        command = "add.s      $f4, $f4, $f6";
        mve = "mov.s      " + destreg + ", $f4";
      }
      else if(type == "double")
      {
        rl = "$f4"; rr = "$f6";
        command = "add.d      $f4, $f4, $f6";
        mve = "mov.d      " + destreg + ", $f4";
      }
      else
      {
        rl = "$8"; rr = "$9";
        command = "addu       $8, $8, $9";
        mve = "move       " + destreg + ", $8";
      }
      right->MIPSPrint(c, os, sp_off, rr);
      if(left->Whatami() == "ArraySubscript")
      {
        stpushreg(rr, os);
        left->MIPSPrint(c, os, sp_off, rl);
        stpopreg(rr, os);
        os << command << std::endl;
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        if(type[type.size() - 1] == '*')
        {
          int shift = findShift(c, sp_off, true);
          if(shift > 0)
          { os << "sll        $9, $9, " << shift << std::endl; }
        }
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        int addr = c.variablelist[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

//-------------------------------------------------------------------------------------------------

class SubAssignment : public BinaryOperator
{
  public:
    SubAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "SubAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " -= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string type = reducetype(c.userdefs, left->MyType(c));
      std::string rl, rr;
      std::string command, mve;
      if(type == "float")
      {
        rl = "$f4"; rr = "$f6";
        command = "sub.s      $f4, $f4, $f6";
        mve = "mov.s      " + destreg + ", $f4";
      }
      else if(type == "double")
      {
        rl = "$f4"; rr = "$f6";
        command = "sub.d      $f4, $f4, $f6";
        mve = "mov.d      " + destreg + ", $f4";
      }
      else
      {
        rl = "$8"; rr = "$9";
        command = "subu       $8, $8, $9";
        mve = "move       " + destreg + ", $8";
      }
      right->MIPSPrint(c, os, sp_off, rr);
      if(left->Whatami() == "ArraySubscript")
      {
        stpushreg(rr, os);
        left->MIPSPrint(c, os, sp_off, rl);
        stpopreg(rr, os);
        os << command << std::endl;
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        if(type[type.size() - 1] == '*')
        {
          int shift = findShift(c, sp_off, true);
          if(shift > 0)
          { os << "sll        $9, $9, " << shift << std::endl; }
        }
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        int addr = c.variablelist[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

//----------------------------------------------------------------------------------------------------

class MultAssignment : public BinaryOperator
{
  public:
    MultAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "MultAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " *= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string type = reducetype(c.userdefs, left->MyType(c));
      std::string rl, rr;
      std::string command, mve;
      if(type == "float")
      {
        rl = "$f4"; rr = "$f6";
        command = "mul.s      $f4, $f4, $f6";
        mve = "mov.s      " + destreg + ", $f4";
      }
      else if(type == "double")
      {
        rl = "$f4"; rr = "$f6";
        command = "mul.d      $f4, $f4, $f6";
        mve = "mov.d      " + destreg + ", $f4";
      }
      else
      {
        rl = "$8"; rr = "$9";
        command = "mult       $8, $9";
        mve = "move       " + destreg + ", $8";
      }
      right->MIPSPrint(c, os, sp_off, rr);
      if(left->Whatami() == "ArraySubscript")
      {
        stpushreg(rr, os);
        left->MIPSPrint(c, os, sp_off, rl);
        stpopreg(rr, os);
        os << command << std::endl;
        if((type != "float")&&(type != "double"))
        { os << "mflo       $8" << std::endl; }
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if((type != "float")&&(type != "double"))
        { os << "mflo       $8" << std::endl; }
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if((type != "float")&&(type != "double"))
        { os << "mflo       $8" << std::endl; }
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        left->MIPSPrint(c, os, sp_off, rl);
        os << command << std::endl;
        if((type != "float")&&(type != "double"))
        { os << "mflo       $8" << std::endl; }
        if(destreg[0] =='$')
        { os  << mve << std::endl; }
        int addr = c.variablelist[left->Name()];
        std::string type = c.vartype[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

//--------------------------------------------------------------------------------------------

class DivAssignment : public BinaryOperator
{
  public:
    DivAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "DivAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " /= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string type = reducetype(c.userdefs, left->MyType(c));
      std::string rl, rr;
      std::string command, mve;
      if(type == "float")
      {
        rl = "$f4"; rr = "$f6";
        command = "div.s      $f4, $f4, $f6";
        mve = "mov.s       " + destreg + ", $f4";;
      }
      else if(type == "double")
      {
        rl = "$f4"; rr = "$f6";
        command = "div.d      $f4, $f4, $f6";
        mve = "mov.d      " + destreg + ", $f4";
      }
      else
      {
        rl = "$8"; rr = "$9";
        mve = "move       " + destreg + ", $8";
      }
      right->MIPSPrint(c, os, sp_off, rr);
      if(left->Whatami() == "ArraySubscript")
      { stpushreg(rr, os); }
      left->MIPSPrint(c, os, sp_off, rl);
      if(left->Whatami() == "ArraySubscript")
      { stpopreg(rr, os); }
      std::string labelend = makeName("L");
      if((type != "float")&&(type != "double"))
      {
        os << "bne        $9, $0, " << labelend << std::endl;
        os << "div        $0, $8, $9" << std::endl;
        os << "break        7" << std::endl;
        os << labelend << ":" << std::endl;
        os << "mfhi       $8" << std::endl;
        os << "mflo       $8" << std::endl;
      }
      else
      {
        os << command << std::endl;
      }
      if(destreg[0] =='$')
      { os  << mve << std::endl; }
      if(left->Whatami() == "ArraySubscript")
      {
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        int addr = c.variablelist[left->Name()];
        std::string type = c.vartype[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

//--------------------------------------------------------------------------------------------

class ModAssignment : public BinaryOperator
{
  public:
    ModAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "ModAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " %= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      right->MIPSPrint(c, os, sp_off, "$9");
      if(left->Whatami() == "ArraySubscript")
      { stpushreg("$9", os); }
      left->MIPSPrint(c, os, sp_off, "$8");
      if(left->Whatami() == "ArraySubscript")
      { stpopreg("$9", os); }
      std::string labelend = makeName("L");
      os << "bne        $9, $0, " << labelend << std::endl;
      os << "div        $0, $8, $9" << std::endl;
      os << "break        7" << std::endl;
      os << labelend << ":" << std::endl;
      os << "mfhi       $8" << std::endl;
      if(destreg[0] =='$')
      { os  << "move        " << destreg << ", $8" << std::endl; }
      if(left->Whatami() == "ArraySubscript")
      {
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        int addr = c.variablelist[left->Name()];
        std::string type = c.vartype[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

//-----------------------------------------------------------------------------------------------

class LShiftAssignment : public BinaryOperator
{
  public:
    LShiftAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "LShiftAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " <<= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      right->MIPSPrint(c, os, sp_off, "$9");
      if(left->Whatami() == "ArraySubscript")
      {
        stpushreg("$9", os);
        left->MIPSPrint(c, os, sp_off, "$8");
        stpopreg("$9", os);
        os << "sllv       $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "sllv       $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "sllv       $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "sllv       $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        int addr = c.variablelist[left->Name()];
        std::string type = c.vartype[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

//-------------------------------------------------------------------------------------

class RShiftAssignment : public BinaryOperator
{
  public:
    RShiftAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "RShiftAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " >>= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      right->MIPSPrint(c, os, sp_off, "$9");
      if(left->Whatami() == "ArraySubscript")
      {
        stpushreg("$9", os);
        left->MIPSPrint(c, os, sp_off, "$8");
        stpopreg("$9", os);
        os << "srlv       $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "srlv       $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "srlv       $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "srlv       $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        int addr = c.variablelist[left->Name()];
        std::string type = c.vartype[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

//-------------------------------------------------------------------------------------------------

class BitAndAssignment : public BinaryOperator
{
  public:
    BitAndAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "BitAndAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " &= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      right->MIPSPrint(c, os, sp_off, "$9");
      if(left->Whatami() == "ArraySubscript")
      {
        stpushreg("$9", os);
        left->MIPSPrint(c, os, sp_off, "$8");
        stpopreg("$9", os);
        os << "and        $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "and        $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "and        $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "and        $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        int addr = c.variablelist[left->Name()];
        std::string type = c.vartype[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

class BitOrAssignment : public BinaryOperator
{
  public:
    BitOrAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "BitOrAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " |= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      right->MIPSPrint(c, os, sp_off, "$9");
      if(left->Whatami() == "ArraySubscript")
      {
        stpushreg("$9", os);
        left->MIPSPrint(c, os, sp_off, "$8");
        stpopreg("$9", os);
        os << "or         $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "or         $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "or         $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "or         $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        int addr = c.variablelist[left->Name()];
        std::string type = c.vartype[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};

//-----------------------------------------------------------------------------------------------------

class BitXorAssignment : public BinaryOperator
{
  public:
    BitXorAssignment(ProgramPtr lefths, ProgramPtr righths): BinaryOperator(lefths, righths) {}

    std::string Whatami() const
    {
      return "BitXorAssignment";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " ^= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      right->MIPSPrint(c, os, sp_off, "$9");
      if(left->Whatami() == "ArraySubscript")
      {
        stpushreg("$9", os);
        left->MIPSPrint(c, os, sp_off, "$8");
        stpopreg("$9", os);
        os << "xor        $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        ArraySubscript* ar = (ArraySubscript*)left;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(left->Whatami() == "PointerDeref")
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "xor        $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        PointerDeref* pd = (PointerDeref*)left;
        pd->pdassign(c, os, sp_off);
      }
      else if((left->Whatami() == "IndMemberRef")||(left->Whatami() == "DirMemberRef"))
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "xor        $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        MemberRef* mr = (MemberRef*)left;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        os << "xor        $8, $8, $9" << std::endl;
        if(destreg[0] =='$')
        { os  << "move        " << destreg << ", $8" << std::endl; }
        int addr = c.variablelist[left->Name()];
        std::string type = c.vartype[left->Name()];
        if(addr <= -1000)
        { gvassign(left->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};




#endif
