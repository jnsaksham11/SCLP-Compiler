CXX = g++
CXXFLAGS = -Wall -std=c++17

LEX = lex
YACC = bison -dv

OBJS = lex.yy.o parser.tab.o main.o ast.o symbol_table.o

sclp: $(OBJS)
	$(CXX) -o sclp $(OBJS) -lfl

parser.tab.c parser.tab.h: parser.y
	$(YACC) parser.y

parser.tab.o: parser.tab.c
	$(CXX) $(CXXFLAGS) -c parser.tab.c

lex.yy.c: scanner.l parser.tab.h
	$(LEX) scanner.l

lex.yy.o: lex.yy.c
	$(CXX) $(CXXFLAGS) -c lex.yy.c

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

ast.o: ast.cc ast.hh
	$(CXX) $(CXXFLAGS) -c ast.cc

symbol_table.o: symbol_table.cc symbol_table.hh
	$(CXX) $(CXXFLAGS) -c symbol_table.cc

clean:
	rm -f *.o lex.yy.c parser.tab.c parser.tab.h parser.output sclp *.toks *.ast *.log