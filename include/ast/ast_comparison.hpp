#ifndef ast_comparison
#define ast_comparison

// Defines classes for comparison operator classes:
//    - >: line 14
//    - >=: line 84
//    - <: line 155
//    - <=: line 225
//    - ==: line 296
//    - !=: line 364

//COMPARISON OPERATORS

//>

class Greater : public BinaryOperator
{
  public:
    Greater(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "Greater";
    }

    int intvalue(Context& c) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float"))
      {
        return (left->dblvalue(c)) > (right->dblvalue(c));
      }
      return (left->intvalue(c)) > (right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " > ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      bool ptr_arr_sub_l = ((left->Whatami() == "ArraySubscript")&&((tl == "double *")||(tl == "float *")));
      bool ptr_arr_sub_r = ((right->Whatami() == "ArraySubscript")&&((tr == "double *")||(tr == "float *")));
      // In case we are comparing floating point
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float")||(ptr_arr_sub_l)||(ptr_arr_sub_r))
      {
        std::string label = makeName("L");
        if((tl == "double")||(tr == "double"))
        {   setupLR(c, os, sp_off, 2);  }
        else
        {   setupLR(c, os, sp_off, 1);  }
        os << "li         " << destreg << ", 1" << std::endl;
        if((tl == "double")||(tr == "double"))
        {   os << "c.lt.d     $f6, $f4" << std::endl; }
        else
        {   os << "c.lt.s     $f6, $f4" << std::endl;}
        os << "nop" << std::endl;
        os << "bc1t       " << label << std::endl;
        os << "nop" << std::endl;
        os << "move       " << destreg << ", $0" << std::endl;
        os << label << ":" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
      // In case we are comparing non-floating point
      else
      {
        setupLR(c, os, sp_off, 0);
        if((tl == "unsigned int")||(tr == "unsigned int")||(tl == "unsigned long")||(tr == "unsigned long"))
        {   os << "sltu       " << destreg << ", $9, $8" << std::endl;  }
        else
        {   os << "slt        " << destreg << ", $9, $8" << std::endl;  }
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
    }
};


// >=

class GreaterEqual : public BinaryOperator
{
  public:
    GreaterEqual(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "GreaterEqual";
    }

    int intvalue(Context& c) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float"))
      {
        return (left->dblvalue(c)) >= (right->dblvalue(c));
      }
      return (left->intvalue(c)) >= (right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " >= ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      bool ptr_arr_sub_l = ((left->Whatami() == "ArraySubscript")&&((tl == "double *")||(tl == "float *")));
      bool ptr_arr_sub_r = ((right->Whatami() == "ArraySubscript")&&((tr == "double *")||(tr == "float *")));
      // In case we are comparing floating point
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float")||(ptr_arr_sub_l)||(ptr_arr_sub_r))
      {
        std::string label = makeName("L");
        if((tl == "double")||(tr == "double"))
        {   setupLR(c, os, sp_off, 2);  }
        else
        {   setupLR(c, os, sp_off, 1);  }
        os << "li         " << destreg << ", 1" << std::endl;
        if((tl == "double")||(tr == "double"))
        {   os << "c.le.d     $f6, $f4" << std::endl; }
        else
        {   os << "c.le.s     $f6, $f4" << std::endl;}
        os << "nop" << std::endl;
        os << "bc1t       " << label << std::endl;
        os << "nop" << std::endl;
        os << "move       " << destreg << ", $0" << std::endl;
        os << label << ":" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
      // In case we are comparing non floating point
      else
      {
        setupLR(c, os, sp_off, 0);
        if((tl == "unsigned int")||(tr == "unsigned int")||(tl == "unsigned long")||(tr == "unsigned long"))
        {   os << "sltu       " << destreg << ", $8, $9" << std::endl;  }
        else
        {   os << "slt        " << destreg << ", $8, $9" << std::endl;  }
        os << "xori       " << destreg << ", " << destreg << ", 0x1" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
    }
};


//<

class Less : public BinaryOperator
{
  public:
    Less(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "Less";
    }

    int intvalue(Context& c) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float"))
      {
        return (left->dblvalue(c)) < (right->dblvalue(c));
      }
      return (left->intvalue(c)) < (right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " < ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      bool ptr_arr_sub_l = ((left->Whatami() == "ArraySubscript")&&((tl == "double *")||(tl == "float *")));
      bool ptr_arr_sub_r = ((right->Whatami() == "ArraySubscript")&&((tr == "double *")||(tr == "float *")));
      // In case we are comparing floating point
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float")||(ptr_arr_sub_l)||(ptr_arr_sub_r))
      {
        std::string label = makeName("L");
        if((tl == "double")||(tr == "double"))
        {   setupLR(c, os, sp_off, 2);  }
        else
        {   setupLR(c, os, sp_off, 1);  }
        os << "li         " << destreg << ", 1" << std::endl;
        if((tl == "double")||(tr == "double"))
        {   os << "c.lt.d     $f4, $f6" << std::endl; }
        else
        {   os << "c.lt.s     $f4, $f6" << std::endl;}
        os << "nop" << std::endl;
        os << "bc1t       " << label << std::endl;
        os << "nop" << std::endl;
        os << "move       " << destreg << ", $0" << std::endl;
        os << label << ":" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
      // In case we are not comparing floating point
      else
      {
        setupLR(c, os, sp_off, 0);
        if((tl == "unsigned int")||(tr == "unsigned int")||(tl == "unsigned long")||(tr == "unsigned long"))
        {   os << "sltu       " << destreg << ", $8, $9" << std::endl;  }
        else
        {   os << "slt        " << destreg << ", $8, $9" << std::endl;  }
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
    }
};


// <=

class LessEqual : public BinaryOperator
{
  public:
    LessEqual(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "LessEqual";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " <= ";
      right->PythonPrint(tabs, globals, os);
    }

    // We only need this function for int values because the floating point coprocessor has a built in function
    // for <=.
    int intvalue(Context& c) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float"))
      {
        return (left->dblvalue(c)) <= (right->dblvalue(c));
      }
      return (left->intvalue(c)) <= (right->intvalue(c));
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      bool ptr_arr_sub_l = ((left->Whatami() == "ArraySubscript")&&((tl == "double *")||(tl == "float *")));
      bool ptr_arr_sub_r = ((right->Whatami() == "ArraySubscript")&&((tr == "double *")||(tr == "float *")));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float")||(ptr_arr_sub_l)||(ptr_arr_sub_r))
      {
        std::string label = makeName("L");
        if((tl == "double")||(tr == "double"))
        {   setupLR(c, os, sp_off, 2);  }
        else
        {   setupLR(c, os, sp_off, 1);  }
        os << "li         " << destreg << ", 1" << std::endl;
        if((tl == "double")||(tr == "double"))
        {   os << "c.le.d     $f4, $f6" << std::endl; }
        else
        {   os << "c.le.s     $f4, $f6" << std::endl;}
        os << "nop" << std::endl;
        os << "bc1t       " << label << std::endl;
        os << "nop" << std::endl;
        os << "move       " << destreg << ", $0" << std::endl;
        os << label << ":" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        if((tl == "unsigned int")||(tr == "unsigned int")||(tl == "unsigned long")||(tr == "unsigned long"))
        {   os << "sltu       " << destreg << ", $9, $8" << std::endl;  }
        else
        {   os << "slt        " << destreg << ", $9, $8" << std::endl;  }
        os << "xori       " << destreg << ", " << destreg << ", 0x1" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
    }
};


//==

class Equal : public BinaryOperator
{
  public:
    Equal(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "Equal";
    }

    // We only need this function for int values because the floating point coprocessor has a built in function
    // for <=.
    int intvalue(Context& c) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float"))
      {
        return (left->dblvalue(c)) == (right->dblvalue(c));
      }
      return (left->intvalue(c)) == (right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " == ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      bool ptr_arr_sub_l = ((left->Whatami() == "ArraySubscript")&&((tl == "double *")||(tl == "float *")));
      bool ptr_arr_sub_r = ((right->Whatami() == "ArraySubscript")&&((tr == "double *")||(tr == "float *")));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float")||(ptr_arr_sub_l)||(ptr_arr_sub_r))
      {
        std::string label = makeName("L");
        if((tl == "double")||(tr == "double"))
        {   setupLR(c, os, sp_off, 2);  }
        else
        {   setupLR(c, os, sp_off, 1);  }
        os << "li         " << destreg << ", 1" << std::endl;
        if((tl == "double")||(tr == "double"))
        {   os << "c.eq.d     $f4, $f6" << std::endl; }
        else
        {   os << "c.eq.s     $f4, $f6" << std::endl;}
        os << "nop" << std::endl;
        os << "bc1t       " << label << std::endl;
        os << "nop" << std::endl;
        os << "move       " << destreg << ", $0" << std::endl;
        os << label << ":" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        os << "xor        " << destreg << ", $9, $8" << std::endl;
        os << "sltiu      " << destreg << ", " << destreg << ", 0x1" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
    }
};


// !=

class NotEqual : public BinaryOperator
{
  public:
    NotEqual(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "NotEqual";
    }

    // We only need this function for int values because the floating point coprocessor has a built in function
    // for <=.
    int intvalue(Context& c) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float"))
      {
        return (left->dblvalue(c)) != (right->dblvalue(c));
      }
      return (left->intvalue(c)) != (right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " != ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      bool ptr_arr_sub_l = ((left->Whatami() == "ArraySubscript")&&((tl == "double *")||(tl == "float *")));
      bool ptr_arr_sub_r = ((right->Whatami() == "ArraySubscript")&&((tr == "double *")||(tr == "float *")));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float")||(ptr_arr_sub_l)||(ptr_arr_sub_r))
      {
        std::string label = makeName("L");
        if((tl == "double")||(tr == "double"))
        {   setupLR(c, os, sp_off, 2);  }
        else
        {   setupLR(c, os, sp_off, 1);  }
        os << "li         " << destreg << ", 1" << std::endl;
        if((tl == "double")||(tr == "double"))
        {   os << "c.eq.d     $f4, $f6" << std::endl; }
        else
        {   os << "c.eq.s     $f4, $f6" << std::endl;}
        os << "nop" << std::endl;
        os << "bc1f       " << label << std::endl;
        os << "nop" << std::endl;
        os << "move       " << destreg << ", $0" << std::endl;
        os << label << ":" << std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        os << "xor        " << destreg << ", $9, $8" << std::endl;
        os << "sltu       " << destreg << ", $0, " << destreg <<  std::endl;
        os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
      }
    }
};

#endif
