%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "ast.hh"
#include "symbol_table.hh"
#include "parser.tab.h"

extern int show_tokens;
extern FILE *tokfile;

static void print_tok(const char *name, const char *lexeme)
{
    if (show_tokens && tokfile) {
        fprintf(tokfile,
                "Token Name: %-20s Lexeme: %-10s Lineno: %d\n",
                name, lexeme, yylineno);
    }
}
%}

%%

[ \t\r\n]+        ;
"//".*            ;

int               { print_tok("INTEGER", yytext); return INTEGER; }
float             { print_tok("FLOAT", yytext); return FLOAT; }
bool              { print_tok("BOOL", yytext); return BOOL; }
string            { print_tok("STRING", yytext); return STRING; }
void              { print_tok("VOID", yytext); return VOID; }

read              { print_tok("READ", yytext); return READ; }
print             { print_tok("WRITE", yytext); return WRITE; }

(if|else|do|while|return) {
                fprintf(stderr, "Illegal keyword '%s' at line %d\n", yytext, yylineno);
                return -1;
                }

[0-9]+"."[0-9]+ { yylval.floatval = atof(yytext);
                print_tok("FLOAT_NUM", yytext); return FLOAT_NUM; }

[0-9]+ { yylval.intval = atoi(yytext);
        print_tok("INT_NUM", yytext); return INT_NUM; }

\"([^\"\n]*)\" { yylval.str = strdup(yytext);
                print_tok("STR_CONST", yytext); return STR_CONST; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext);
                        print_tok("NAME", yytext); return NAME; }

"&&"              { print_tok("AND", yytext); return AND; }
"||"              { print_tok("OR", yytext); return OR; }
"!"               { print_tok("NOT", yytext); return NOT; }

"<="              { print_tok("LESS_THAN_EQUAL", yytext); return LE; }
">="              { print_tok("GREATER_THAN_EQUAL", yytext); return GE; }
"=="              { print_tok("EQUAL", yytext); return EQ; }
"!="              { print_tok("NOT_EQUAL", yytext); return NE; }
"<"               { print_tok("LESS_THAN", yytext); return LT; }
">"               { print_tok("GREATER_THAN", yytext); return GT; }

"="               { print_tok("ASSIGN_OP", yytext); return ASSIGN_OP; }

"+"               { print_tok("PLUS", yytext); return '+'; }
"-"               { print_tok("MINUS", yytext); return '-'; }
"*"               { print_tok("MULT", yytext); return '*'; }
"/"               { print_tok("DIV", yytext); return '/'; }

"?"               { print_tok("QUESTION_MARK", yytext); return '?'; }
":"               { print_tok("COLON", yytext); return ':'; }

";"               { print_tok("SEMICOLON", yytext); return ';'; }
","               { print_tok("COMMA", yytext); return ','; }

"("               { print_tok("LEFT_ROUND_BRACKET", yytext); return '('; }
")"               { print_tok("RIGHT_ROUND_BRACKET", yytext); return ')'; }

"{"               { print_tok("LEFT_CURLY_BRACKET", yytext); return '{'; }
"}"               { print_tok("RIGHT_CURLY_BRACKET", yytext); return '}'; }

.                 { fprintf(stderr, "Illegal character '%c' at line %d\n",
                            yytext[0], yylineno);
                    return -1; }

%%

int yywrap(void)
{
    return 1;
}