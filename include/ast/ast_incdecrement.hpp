#ifndef ast_incdec
#define ast_incdec

// Defines classes for unary increment and decrement operators
//    - IncDec parent class: line 11
//    - Unary increment class: line 68
//    - Unary decrement class: line 131

//PARENT CLASS

class IncDec : public UnaryOperator
{
  public:
    IncDec(ProgramPtr operand): UnaryOperator(operand) {}

    // Used in case we are performing increment on a pointer (the added value depends on memory size of datatype)
    int IncDecShift(Context& c, int sp_off, std::string typ) const
    {
        int shift = 4;
        if(typ == "double *")
        { shift = 8; }
        else if((c.assign_str).find(operand->Name()) != (c.assign_str).end()) { shift = 1; }
        else if(operand->Whatami() == "Variable")
        {
          if((sp_off - c.variablelist[operand->Name()]) >= c.framesize)
          {
            if((typ == "short *")||(typ == "unsigned short *")||(typ == "signed short *"))
            { shift = 2; }
            else if((typ == "char *")||(typ == "unsigned char *")||(typ == "signed char *"))
            { shift = 1; }
          }
        }
        return shift;
    }

    // Used to assign result of an increment to another variable member.
    void PerformAssignment(Context& c, std::ostream& os, int sp_off) const
    {
      if(operand->Whatami() == "ArraySubscript")
      {
        ArraySubscript* ar = (ArraySubscript*)operand;
        ar->ArSubassign(c, os, sp_off);
      }
      else if(operand->Whatami() == "PointerDeref")
      {
        PointerDeref* pd = (PointerDeref*)operand;
        pd->pdassign(c, os, sp_off);
      }
      else if((operand->Whatami() == "IndMemberRef")||(operand->Whatami() == "DirMemberRef"))
      {
        MemberRef* mr = (MemberRef*)operand;
        mr->MRassign(c, os, sp_off);
      }
      else
      {
        int addr = c.variablelist[operand->Name()];
        std::string type = c.vartype[operand->Name()];
        if(addr <= -1000)
        { gvassign(operand->Name(), type, os); }
        else
        { assignment(addr, sp_off, type, os); }
      }
    }
};



//UNARY INCREMENT

class Increment : public IncDec
{
  public:
    Increment(ProgramPtr operand, bool post): IncDec(operand), isPostIncrement(post){}

    std::string Whatami() const
    {
      return "Increment";
    }

    std::string MyType(Context& c)
    {
      return reducetype(c.userdefs, operand->MyType(c));
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string typ = reducetype(c.userdefs, operand->MyType(c));

      //POST-INCREMENT
      if(isPostIncrement)
      {
        operand->MIPSPrint(c, os, sp_off, "$8");
        if(destreg != "")
        { os << "move       $12, $8" << std::endl; }
        if(typ[typ.size() - 1] == '*')
        {
          int shift = IncDecShift(c, sp_off, typ);
          if(shift > 0)
          { os << "addiu        $8, $8, " << shift << std::endl; }
        }
        else
        { os << "addiu      $8, $8, 1" << std::endl; }
        PerformAssignment(c, os, sp_off);
        if(destreg != "")
        { os << "move       " << destreg << ", $12" << std::endl; }
      }

      //PRE-INCREMENT
      else
      {
        operand->MIPSPrint(c, os, sp_off, "$8");
        if(typ[typ.size() - 1] == '*')
        {
          int shift = IncDecShift(c, sp_off, typ);
          if(shift > 0)
          { os << "addiu        $8, $8, " << shift << std::endl; }
        }
        else
        { os << "addiu      $8, $8, 1" << std::endl; }
        PerformAssignment(c, os, sp_off);
        if((destreg != "$8")&&(destreg != ""))
        { os << "move       " << destreg << ", $8" << std::endl; }
      }
    }
  private:
    bool isPostIncrement;
};



//UNARY DECREMENT

class Decrement : public IncDec
{
  public:
    Decrement(ProgramPtr operand, bool post): IncDec(operand), isPostDecrement(post){}

    std::string Whatami() const
    {
      return "Decrement";
    }

    std::string MyType(Context& c)
    {
      return reducetype(c.userdefs, operand->MyType(c));
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string typ = operand->MyType(c);

      //POST DECREMENT
      if(isPostDecrement)
      {
        operand->MIPSPrint(c, os, sp_off, "$8");
        if(destreg != "")
        { os << "move       $12, $8" << std::endl; }
        if(typ[typ.size() - 1] == '*')
        {
          int shift = -IncDecShift(c, sp_off, typ);
          if(shift < 0)
          { os << "addiu        $8, $8, " << shift << std::endl; }
        }
        else
        { os << "addiu      $8, $8, -1" << std::endl; }
        PerformAssignment(c, os, sp_off);
        if(destreg != "")
        { os << "move       " << destreg << ", $12" << std::endl; }
      }

      //PRE-DECREMENT
      else
      {
        operand->MIPSPrint(c, os, sp_off, "$8");
        if(typ[typ.size() - 1] == '*')
        {
          int shift = -IncDecShift(c, sp_off, typ);
          if(shift < 0)
          { os << "addiu        $8, $8, " << shift << std::endl; }
        }
        else
        { os << "addiu      $8, $8, -1" << std::endl; }
        PerformAssignment(c, os, sp_off);
        if((destreg != "$8")&&(destreg != ""))
        { os << "move       " << destreg << ", $8" << std::endl;}
      }
    }
  private:
    bool isPostDecrement;
};





#endif
