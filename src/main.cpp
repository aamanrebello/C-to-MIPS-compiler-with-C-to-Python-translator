#include "ast.hpp"

extern FILE *yyin;

// TOP LEVEL FUNCTION THAT RUNS THE COMPILER/TRANSLATOR BASED ON COMMAND LINE ARGUMENTS
// Prints translation into file or on terminal window based on flags.

int main(int argc, char *argv[])
{
    std::ostream* fp;
    std::ofstream outfile;

    if(argc < 4)
    {
        std::cout << "Invalid Parameters Provided" << std::endl;
        std::exit(-1);
    }

    yyin = fopen(argv[2], "r");
    if(yyin==NULL)
    {
        std::cout <<"Input file not found.\n";
        std::exit(1);
    }

    // if the -o <<filename>> flag is provided, we output into the file
    if(std::string(argv[3]) == "-o")
    {
        outfile.open(argv[4]);
        if (!outfile.is_open())
        {
          std::cout << "Output file not found." << std::endl;
          std::exit(2);
        }
        fp = &outfile;
    }
    // else output to terminal window.
    else
    {
        fp = &(std::cout);
    }

    // Build parse tree by calling function
    const Program *ast=parseAST();
    if(ast==NULL)
    {
        std::cout << "Tree not built\n";
        std::exit(3);
    }

    // -- translate flag implies we need to translate to Python
    if(std::string(argv[1]) == "--translate")
    {
        std::map<std::string, std::string> gvar;
        ast->PythonPrint(0, gvar, *fp);
    }

    // -S flag implies we need to compile to MIPS
    else if(std::string(argv[1]) == "-S")
    {
      Context state;
      ast->MIPSPrint(state, *fp, 0, "");
    }

   if (outfile.is_open())
   {
      outfile.close();
   }
   return 0;
}
