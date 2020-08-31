#ifndef ast_array
#define ast_array

// Deals with Array related classes - declaration (line 6) and subscript reference (line 32 onwards)

//ARRAY DECL

class ArrayDefinition : public Program
{
  public:
    ArrayDefinition(std::string id, int size): identifier(id), size(size) {}

    std::string Whatami() const
    {
      return "ArrayDefinition";
    }
    std::string Name() const
    {
      return identifier;
    }

    int compsize() const
    {
      return size;
    }
  private:
    std::string identifier;
    int size;
};


//ARRAY SUBSCRIPT


class ArraySubscript : public Program
{
  public:
    ArraySubscript(std::string id, ProgramPtr index): identifier(id), index(index) {}

    std::string Whatami() const
    {
      return "ArraySubscript";
    }
    std::string Name() const
    {
      return identifier;
    }
    std::string MyType(Context& c) const
    {
      return reducetype(c.userdefs, c.vartype[identifier]);
    }

    int maxparams(Context& c) const
    {
      return index->maxparams(c);
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      return index->findstrings(slabel);
    }

    //FUNCTION TO ASSIGN ARRAY SUBSCRIPT
    void ArSubassign(Context& c, std::ostream& os, int sp_off)
    {
      bool is_parameter = false;
      // If we cannot find the array name in our list of defined arrays, this means we are dealing with a pointer.
      bool is_array_pointer = (c.arrays.find(identifier) == c.arrays.end());
      int addr = c.variablelist[identifier];
      // In case the array index is an expression we need to evaluate it.
      if(!IsOperand(index->Whatami()))
      { stpushreg("$8", os);}
      index->MIPSPrint(c, os, sp_off, "$9");
      if(!IsOperand(index->Whatami()))
      {stpopreg("$8", os);}
      std::string type = reducetype(c.userdefs, c.vartype[identifier]);
      int wshift = 2;
      if(type == "double")
      { wshift = 3;  }
      else if((sp_off - addr) >= c.framesize)
      {
        is_parameter = true;
        if((type == "short *")||(type == "unsigned short *")||(type == "signed short *"))
        {
          wshift = 1;
        }
        else if((type == "char *")||(type == "unsigned char *")||(type == "signed char *"))
        {
          wshift = 0;
        }
      }
      // We use large negative numbers for the address to indicate that the array is global
      if(addr <= -1000)
      {
            os << "sll        $9, $9, " << wshift << std::endl;
            os << "lui        $10, %hi(" << identifier << ")" << std::endl;
            os << "addiu      $10, $10, %lo(" << identifier << ")" << std::endl;
            os << "addu       $9, $9, $10" << std::endl;
            if((type == "char")||(type == "unsigned char")||(type == "signed char"))
            {
              os << "sb         $8, 3($9)" << std::endl;
            }
            else if((type == "short")||(type == "unsigned short")||(type == "signed short"))
            {
              os << "sh         $8, 2($9)" << std::endl;
            }
            else if(type == "float")
            {
              os << "swc1       $f4, 0($9)" << std::endl;
            }
            else if(type == "double")
            {
              os << "swc1       $f4, 4($9)" << std::endl;
              os << "nop" << std::endl;
              os << "swc1       $f5, 0($9)" << std::endl;
            }
            else
            {
              os << "sw         $8, 0($9)" << std::endl;
            }
            os << "nop" << std::endl;
      }
      // In case arry is not globally declared outside functions.
      else
      {
          os << "sll        $9, $9, " << wshift << std::endl;
          if((type[type.size() - 1] == '*')&&(is_array_pointer))
          { os << "lw         $11, " << (sp_off - addr) << "($fp)" << std::endl; os << "nop" << std::endl; }
          else
          { os << "addiu      $11, $fp, " << (sp_off - addr) << std::endl; }
          os << "addu       $9, $9, $11" << std::endl;
          if((type == "char")||(type == "unsigned char")||(type == "signed char"))
          {
            os << "sb         $8, 3($9)" << std::endl;
          }
          else if((type == "short")||(type == "unsigned short")||(type == "signed short"))
          {
            os << "sh         $8, 2($9)" << std::endl;
          }
          else if((type == "float")||((type == "float *")&&(is_array_pointer)))
          {
            os << "swc1       $f4, 0($9)" << std::endl;
          }
          else if((type == "double")||((type == "double *")&&(is_array_pointer)))
          {
            os << "swc1       $f4, 4($9)" << std::endl;
            os << "nop" << std::endl;
            os << "swc1       $f5, 0($9)" << std::endl;
          }
          else if(is_parameter)
          {
            if((type == "char *")||(type == "unsigned char *")||(type == "signed char *"))
            {
              os << "sb         $8, 0($9)" << std::endl;
            }
            else if((type == "short *")||(type == "unsigned short *")||(type == "signed short *"))
            {
              os << "sh         $8, 0($9)" << std::endl;
            }
            else
            {
              os << "sw         $8, 0($9)" << std::endl;
            }
          }
          else
          {
            os << "sw         $8, 0($9)" << std::endl;
          }
          os << "nop" << std::endl;
      }
    }

    //FUNCTION TO PRINT VALUE AT ARRAY SUBSCRIPT
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      bool is_array_pointer = (c.arrays.find(identifier) == c.arrays.end());
      int addr = c.variablelist[identifier];
      std::string type = reducetype(c.userdefs, c.vartype[identifier]);
      // Used to define how much the index needs to be shifted by (based on datatype) to obtain the catual memory offset
      int wshift = 2;
      // variable used to indicate that array is a function parameter.
      bool is_parameter = false;
      if(type == "double")
      {
        wshift = 3;
      }
      else if((c.assign_str).find(identifier) != (c.assign_str).end())
      { wshift = 0; }
      else if((sp_off - addr) >= c.framesize)
      {
        is_parameter = true;
        if((type == "short *")||(type == "unsigned short *")||(type == "signed short *"))
        {
          wshift = 1;
        }
        else if((type == "char *")||(type == "unsigned char *")||(type == "signed char *"))
        {
          wshift = 0;
        }
      }
      if(destreg == "$8")
      {stpushreg("$9", os);}
      else if(destreg == "$9")
      {stpushreg("$8", os);}
      else
      {stpushreg("$8", os); stpushreg("$9", os);}
      index->MIPSPrint(c, os, sp_off, "$8");
      // array is global - indicated by large negative address.
      if(addr <= -1000)
      {
          os << "sll        $8, $8, " << wshift << std::endl;
          os << "lui        $9, %hi(" << identifier << ")" << std::endl;
          os << "addiu      $9, $9, %lo(" << identifier << ")" << std::endl;
          os << "addu       $8, $8, $9" << std::endl;
          if((type == "char")||(type == "signed char"))
          { os << "lb         " << destreg << ", 3($8)" << std::endl; }
          else if(type == "unsigned char")
          { os << "lbu        " << destreg << ", 3($8)" << std::endl; }
          else if((type == "short")||(type == "signed short"))
          { os << "lh         " << destreg << ", 2($8)" << std::endl; }
          else if(type == "unsigned short")
          { os << "lh         " << destreg << ", 2($8)" << std::endl; }
          else if((type == "float")||((type == "float *")&&(is_array_pointer)))
          { os << "lwc1       " << destreg << ", 0($8)" << std::endl; }
          else if((type == "double")||((type == "double *")&&(is_array_pointer)))
          {
            int n = std::stoi(destreg.substr(2)) + 1;
            std::string reg2 = destreg.substr(0, 2) + std::to_string(n);
            os << "lwc1       " << destreg << ", 4($8)" << std::endl;
            os << "nop" << std::endl;
            os << "lwc1       " << reg2 << ", 0($8)" << std::endl;
          }
          else
          { os << "lw         " << destreg << ", 0($8)" << std::endl; }
          os << "nop" << std::endl;
      }
      // array is not glabal - declared inside function.
      else
      {
          os << "sll        $8, $8, " << wshift << std::endl;
          if((type[type.size() - 1] == '*')&&(is_array_pointer))
          { os << "lw         $9, " << (sp_off - addr) << "($fp)" << std::endl; os << "nop" << std::endl; }
          else
          { os << "addiu      $9, $fp, " << (sp_off - addr) << std::endl; }
          os << "addu       $8, $8, $9" << std::endl;
          if((c.assign_str).find(identifier) != (c.assign_str).end())
          { os << "lbu        " << destreg << ", 0($8)" << std::endl;}
          else if((type == "char")||(type == "signed char"))
          { os << "lb         " << destreg << ", 3($8)" << std::endl; }
          else if(type == "unsigned char")
          { os << "lbu        " << destreg << ", 3($8)" << std::endl; }
          else if((type == "short")||(type == "signed short"))
          { os << "lh         " << destreg << ", 2($8)" << std::endl; }
          else if(type == "unsigned short")
          { os << "lh         " << destreg << ", 2($8)" << std::endl; }
          else if((type == "float")||((type == "float *")&&(is_array_pointer)))
          { os << "lwc1       " << destreg << ", 0($8)" << std::endl; }
          else if((type == "double")||((type == "double *")&&(is_array_pointer)))
          {
            int n = std::stoi(destreg.substr(2)) + 1;
            std::string reg2 = destreg.substr(0, 2) + std::to_string(n);
            os << "lwc1       " << destreg << ", 4($8)" << std::endl;
            os << "nop" << std::endl;
            os << "lwc1       " << reg2 << ", 0($8)" << std::endl;
          }
          else if(is_parameter)
          {
            if((type == "char *")||(type == "signed char *"))
            { os << "lb         " << destreg << ", 0($8)" << std::endl; }
            else if(type == "unsigned char *")
            { os << "lbu        " << destreg << ", 0($8)" << std::endl; }
            else if((type == "short *")||(type == "signed short *"))
            { os << "lh         " << destreg << ", 0($8)" << std::endl; }
            else if(type == "unsigned short *")
            { os << "lhu        " << destreg << ", 0($8)" << std::endl; }
            else
            { os << "lw         " << destreg << ", 0($8)" << std::endl; }
          }
          else
          { os << "lw         " << destreg << ", 0($8)" << std::endl; }
          os << "nop" << std::endl;
      }
      if(destreg == "$8")
      {stpopreg("$9", os);}
      else if(destreg == "$9")
      {stpopreg("$8", os);}
      else
      {stpopreg("$8", os); stpopreg("$9", os);}
    }
  private:
    std::string identifier;
    ProgramPtr index;
};





#endif
