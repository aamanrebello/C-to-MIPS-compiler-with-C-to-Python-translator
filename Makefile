CC=g++
CPPFLAGS+=-std=c++11 -W -Wall -g -Wno-unused-parameter
CPPFLAGS+=-I include

OBJECTS += src/helperfunctions.o src/main.o  src/compiler_flexer.yy.o src/compiler_parser.tab.o
OBJECTS += src/impl/FuncDeclCall.o src/impl/Function.o 
OBJECTS += src/impl/Program.o
OBJECTS += src/impl/Sequence.o


all: bin/tester bin/c_compiler

src/test.yy.cpp: src/test.flex
	flex -o src/test.yy.cpp  src/test.flex

bin/tester: src/test.yy.cpp
	mkdir -p bin
	$(CC) src/test.yy.cpp -o bin/tester

run_tester: 
	bin/tester

src/compiler_flexer.yy.cpp: src/compiler_flexer.flex src/compiler_parser.tab.hpp
	flex -o src/compiler_flexer.yy.cpp  src/compiler_flexer.flex

src/compiler_parser.tab.cpp src/compiler_parser.tab.hpp: src/compiler_parser.y
	bison -v -d src/compiler_parser.y -o src/compiler_parser.tab.cpp

src/impl/%.o: src/impl/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

bin/c_compiler: $(OBJECTS)
	mkdir -p bin
	$(CC) $(CPPFLAGS) -o bin/c_compiler $^

run_compiler: 
	bin/c_compiler

clean:
	rm src/*.o src/impl/*.o bin/* src/*.tab.cpp src/*.yy.cpp src/*.tab.hpp src/*.output