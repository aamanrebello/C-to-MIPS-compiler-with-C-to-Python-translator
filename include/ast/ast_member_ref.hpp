#ifndef ast_member_ref
#define ast_member_ref

// Classes for direct and indirect member reference operators (mostly used for structs).
//    - Member reference superclass: line 9
//    - Indirect member reference: line 87
//    - Direct member reference: line 208

class MemberRef : public Program
{
  public:
    MemberRef(ProgramPtr var, ProgramPtr member): variable(var), reference(member) {}
    ~MemberRef()
    {
      delete variable;
      delete reference;
    }

    // Used to calculate memory offset of particular member within struct given index of member
    // within corresponding Sdecl struct entity (see ast.hpp).
    int offset(Context& c, std::string strname, int& index) const
    {
      int offset = 0;
      std::string type, nexttype;
      for(unsigned int i = 0; i < c.structs[strname].strvars.size(); i++)
      {
        type = reducetype(c.userdefs, c.structs[strname].types[i]);
        if(i != c.structs[strname].strvars.size() - 1)
        {
          nexttype = reducetype(c.userdefs, c.structs[strname].types[i + 1]);
        }
        else
        {
          nexttype = "";
        }
        if(c.structs[strname].strvars[i] == reference->Name())
        {
          index = i;
          break;
        }
        if((type == "char")||(type == "unsigned char")||(type == "signed char"))
        {
          if((nexttype == "char")||(nexttype == "unsigned char")||(nexttype == "signed char")||(nexttype == ""))
          { offset += 1; }
          else if((nexttype == "short")||(nexttype == "unsigned short")||(nexttype == "signed short"))
          { offset += 2 - (offset%2); }
          else if(nexttype == "double")
          { offset += 8 - (offset%8); }
          else
          { offset += 4 - (offset%4); }
        }
        else if((type == "short")||(type == "unsigned short")||(type == "signed short"))
        {
          if((nexttype == "short")||(nexttype == "unsigned short")||(nexttype == "signed short")||(nexttype == ""))
          { offset += 2; }
          else if((nexttype == "char")||(nexttype == "unsigned char")||(nexttype == "signed char"))
          { offset += 2; }
          else if(nexttype == "double")
          { offset += 8 - (offset%8); }
          else
          { offset += 4 - (offset%4); }
        }
        else if(type == "double")
        {
          offset += 8;
        }
        else
        {
          offset += 4;
        }
      }
      return offset;
    }

    //virtual function used for assignment of value to a member.
    virtual void MRassign(Context& c, std::ostream& os, int sp_off) const
    {
      os << "ERROR" << std::endl;
    }
  protected:
    ProgramPtr variable;
    ProgramPtr reference;
};

//INDIRECT MEMBER REFERENCE: ->

class IndMemberRef : public MemberRef
{
  public:
    IndMemberRef(ProgramPtr var, ProgramPtr member): MemberRef(var, member) {}

    std::string Whatami() const
    {
      return "IndMemberRef";
    }

    // Used to determine type of member being referred to.
    std::string MyType(Context& c) const
    {
      // remove typedef aliases
      std::string strtype = reducetype(c.userdefs, c.vartype[variable->Name()]);
      // The type of a struct is defined as 'struct a', so by taking substring we will obtain 'a'.
      std::string strid = strtype.substr(7);
      strid.pop_back();
      strid.pop_back();
      for(unsigned int i = 0; i < c.structs[strid].strvars.size(); i++)
      {
        if(c.structs[strid].strvars[i] == reference->Name())
        {
          return reducetype(c.userdefs, c.structs[strid].types[i]);
        }
      }
      return "";
    }

    void MRassign(Context& c, std::ostream& os, int sp_off) const
    {
      std::string strtype = reducetype(c.userdefs, c.vartype[variable->Name()]);
      std::string strid = strtype.substr(7);
      strid.pop_back();
      strid.pop_back();
      int addr = c.variablelist[variable->Name()];
      os << "lw         $9, " << sp_off - addr << "($fp)" << std::endl;
      os << "nop" << std::endl;
      int index = 0;
      int off = offset(c, strid, index);
      std::string vtype = reducetype(c.userdefs, c.structs[strid].types[index]);
      if((vtype == "char")||(vtype == "unsigned char")||(vtype == "signed char"))
      {
        os << "sb         $8, " << off << "($9)" << std::endl;
      }
      else if((vtype == "short")||(vtype == "unsigned short")||(vtype == "signed short"))
      {
        os << "sh         $8, " << off << "($9)" << std::endl;
      }
      else if(vtype == "float")
      {
        os << "swc1       $f4, " << off << "($9)" << std::endl;
      }
      else if(vtype == "double")
      {
        os << "swc1       $f4, " << (off + 4) << "($9)" << std::endl;
        os << "nop" << std::endl;
        os << "swc1       $f5, " << off << "($9)" << std::endl;
      }
      else
      {
        os << "sw         $8, " << off << "($9)" << std::endl;
      }
      os << "nop" << std::endl;
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string strtype = reducetype(c.userdefs, c.vartype[variable->Name()]);
      std::string strid = strtype.substr(7);
      strid.pop_back();
      strid.pop_back();
      int addr = c.variablelist[variable->Name()];
      int index = 0;
      int off = offset(c, strid, index);
      std::string vtype = reducetype(c.userdefs, c.structs[strid].types[index]);
      if((vtype != "double")&&(vtype != "float"))
      {
        os << "lw         " << destreg << ", " << sp_off - addr << "($fp)" << std::endl;
      }
      else
      {
        os << "lw         $8, " << sp_off - addr << "($fp)" << std::endl;
      }
      os << "nop" << std::endl;
      /////
      if((vtype == "char")||(vtype == "signed char"))
      {
        os << "lb         " << destreg << ", " << off << "(" << destreg << ")" << std::endl;
      }
      else if(vtype == "unsigned char")
      {
        os << "lbu        " << destreg << ", " << off << "(" << destreg << ")" << std::endl;
      }
      else if((vtype == "short")||(vtype == "signed short"))
      {
        os << "lh         " << destreg << ", " << off << "(" << destreg << ")" << std::endl;
      }
      else if(vtype == "unsigned short")
      {
        os << "lhu        " << destreg << ", " << off << "(" << destreg << ")" << std::endl;
      }
      else if(vtype == "float")
      {
        os << "lwc1       " << destreg << ", " << off << "($8)" << std::endl;
      }
      else if(vtype == "double")
      {
        int n = std::stoi(destreg.substr(2)) + 1;
        std::string reg2 = destreg.substr(0, 2) + std::to_string(n);
        os << "lwc1       " << destreg << ", " << off + 4 << "($8)" << std::endl;
        os << "nop" << std::endl;
        os << "lwc1       " << reg2 << ", " << off << "($8)" << std::endl;
      }
      else
      {
        os << "lw         " << destreg << ", " << off << "(" << destreg << ")" << std::endl;
      }
      os << "nop" << std::endl;
    }
};

//DIRECT MEMBER REFERENCE: .

class DirMemberRef : public MemberRef
{
  public:
    DirMemberRef(ProgramPtr var, ProgramPtr member): MemberRef(var, member) {}

    std::string Whatami() const
    {
      return "DirMemberRef";
    }

    // Used to determine type of member being referred to.
    std::string MyType(Context& c) const
    {
      // remove typedef aliases
      std::string strtype = reducetype(c.userdefs, c.vartype[variable->Name()]);
      // The type of a struct is defined as 'struct a', so by taking substring we will obtain 'a'.
      std::string strid = strtype.substr(7);
      for(unsigned int i = 0; i < c.structs[strid].strvars.size(); i++)
      {
        if(c.structs[strid].strvars[i] == reference->Name())
        {
          return reducetype(c.userdefs, c.structs[strid].types[i]);
        }
      }
      return "";
    }

    void MRassign(Context& c, std::ostream& os, int sp_off) const
    {
      std::string strtype = reducetype(c.userdefs, c.vartype[variable->Name()]);
      std::string strid = strtype.substr(7);
      int index = 0;
      int addr = c.variablelist[variable->Name()] - offset(c, strid, index);
      std::string vtype = reducetype(c.userdefs, c.structs[strid].types[index]);
      if((vtype == "char")||(vtype == "unsigned char")||(vtype == "signed char"))
      {
        os << "sb         $8, " << sp_off - addr << "($fp)" << std::endl;
      }
      else if((vtype == "short")||(vtype == "unsigned short")||(vtype == "signed short"))
      {
        os << "sh         $8, " << sp_off - addr << "($fp)" << std::endl;
      }
      else if(vtype == "float")
      {
        os << "swc1       $f4, " << sp_off - addr << "($fp)" << std::endl;
      }
      else if(vtype == "double")
      {
        os << "swc1       $f4, " << sp_off - addr + 4 << "($fp)" << std::endl;
        os << "nop" << std::endl;
        os << "swc1       $f5, " << sp_off - addr << "($fp)" << std::endl;
      }
      else
      {
        os << "sw         $8, " << sp_off - addr << "($fp)" << std::endl;
      }
      os << "nop" << std::endl;
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string strtype = reducetype(c.userdefs, c.vartype[variable->Name()]);
      std::string strid = strtype.substr(7);
      int index = 0;
      int addr = c.variablelist[variable->Name()] - offset(c, strid, index);
      std::string vtype = reducetype(c.userdefs, c.structs[strid].types[index]);
      if((vtype == "char")||(vtype == "signed char"))
      {
        os << "lb         " << destreg << ", " << sp_off - addr << "($fp)" << std::endl;
      }
      else if(vtype == "unsigned char")
      {
        os << "lbu        " << destreg << ", " << sp_off - addr << "($fp)" << std::endl;
      }
      else if((vtype == "short")||(vtype == "signed short"))
      {
        os << "lh         " << destreg << ", " << sp_off - addr << "($fp)" << std::endl;
      }
      else if(vtype == "unsigned short")
      {
        os << "lhu        " << destreg << ", " << sp_off - addr << "($fp)" << std::endl;
      }
      else if(vtype == "float")
      {
        os << "lwc1       " << destreg << ", " << sp_off - addr << "($fp)" << std::endl;
      }
      else if(vtype == "double")
      {
        int n = std::stoi(destreg.substr(2)) + 1;
        std::string reg2 = destreg.substr(0, 2) + std::to_string(n);
        os << "lwc1       " << destreg << ", " << sp_off - addr + 4 << "($fp)" << std::endl;
        os << "nop" << std::endl;
        os << "lwc1       " << reg2 << ", " << sp_off - addr << "($fp)" << std::endl;
      }
      else
      {
        os << "lw         " << destreg << ", " << sp_off - addr << "($fp)" << std::endl;
      }
      os << "nop" << std::endl;
    }
};

#endif
