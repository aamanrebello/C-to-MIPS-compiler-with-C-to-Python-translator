# C-to-MIPS-compiler-with-C-to-Python-translator
----

**1. The Project**
-
This software, written in C++, serves two functions:
 1. It can compile an input C90 (ANSI) program to MIPS1 assembly
 2. It can also translate this program to the Python3 equivalent.

It conforms to [This spec](https://github.com/LangProc/langproc-2019-cw). All
specified features are covered.

Also included are bash scripts to run the compiler against sets of test programs for
both functionalities (these test cases are also included).


**2. Required Software**
-
  1. [Flex](https://github.com/westes/flex), the fast lexical analyzer generator.
  2. [GNU Bison](https://www.gnu.org/software/bison/), is used as parser.

  The compiler is intended to run on a *Linux* OS e.g. Ubuntu.


**3. How to run**
-
1. First build:
`make bin/c_compiler`
Build will take a little bit of time.

2. Then you may compile:
`bin/c_compiler -S filename.c -o output_file.s`

3. Or translate:
`bin/c_compiler --translate filename.c -o output_file.py`

4. To output translation to terminal window instead of to a file, use '-k'
   flag instead of '-o' and do not put a file name after the flag e.g.
   `bin/c_compiler -S filename.c -k`

5. Finally, to test the compiler, run the bash script:
`./compiler_test.sh`

6. And to test translator:
`./c_translator_formative.sh`


**4. Additional Information**
-
This project was done as part of a college assessment in a group of 2. It conforms to [this spec](https://github.com/LangProc/langproc-2019-cw), but the implementation is original work of the authors.
