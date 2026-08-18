%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

#include "ast.hh"
#include "symbol_table.hh"

extern int sa_parse;
extern int show_ast;
extern FILE *astfile;


int yylex(void);
int yyerror(const char *);

/*================ SYMBOL TABLES =================*/

Symbol_Table global_symbol_table;
Symbol_Table local_symbol_table;

Symbol_Table* current_symbol_table = &global_symbol_table;

/*================ TEMPORARY DECL TYPE =================*/

Data_Type current_decl_type;

/*================ ROOT AST =================*/

Sequence_Stmt_Ast* root_ast = nullptr;

/*================ FUNCTION HEADER STORAGE =================*/

std::string current_function_name;
Data_Type current_function_return_type;

/* Global helper */
std::vector<Param> *current_param_list = nullptr;


%}

/*================ UNION =================*/

%union{
    int intval;
    double floatval;
    char* str;

    Data_Type dtype;

    Expression_Ast* expr;
    Statement_Ast* stmt;
    Sequence_Stmt_Ast* seq;

    Param* param;
    std::vector<Param>* param_list;
}

/*================ TOKENS =================*/

%token INTEGER FLOAT STRING BOOL VOID
%token READ WRITE

%token ASSIGN_OP
%token UMINUS

%token <intval> INT_NUM
%token <floatval> FLOAT_NUM
%token <str> STR_CONST
%token <str> NAME

%token AND OR NOT
%token LT LE GT GE NE EQ

/*================ PRECEDENCE =================*/

%right '?' ':'
%left OR
%left AND
%right NOT
%nonassoc LT LE GT GE EQ NE
%left '+' '-'
%left '*' '/'
%right UMINUS

/*================ TYPE DECLARATIONS =================*/

%type <expr> expression rel_expression variable_name constant_as_operand
%type <stmt> statement assignment_statement print_statement read_statement
%type <seq> statement_list
%type <dtype> named_type param_type

%type <param> formal_param
%type <param_list> formal_param_list

%type <seq> body_content

%%


program
    : program_body
      {
          if (!sa_parse) {

              Function_Info* f = global_symbol_table.get_function();

              if (f == nullptr || !f->is_defined()) {
                  fprintf(stderr, "Error: main definition missing\n");
                  exit(1);
              }

              if (show_ast && astfile) {

                  /* ---- Print Procedure Header ---- */

                  fprintf(astfile, "**PROCEDURE: %s\n",
                          f->get_name().c_str());

                  fprintf(astfile,
                          "    Return Type: <%s>\n",
                          data_type_to_string(
                              f->get_return_type()).c_str());

                  fprintf(astfile,
                          "    Formal Parameters:\n");

                  std::vector<Data_Type> types =
                      f->get_param_types();

                  std::vector<std::string> names =
                      f->get_param_names();

                  for (size_t i = 0; i < types.size(); i++) {

                      fprintf(astfile,
                              "        %s_  Type:<%s>\n",
                              names[i].c_str(),
                              data_type_to_string(
                                  types[i]).c_str());
                  }

                  root_ast->print_ast(2);

              }
          }
      }
;

program_body
    : global_decl_list func_decl global_decl_list func_def
    | global_decl_list func_decl func_def
    | func_decl global_decl_list func_def
    | global_decl_list func_def
    | func_decl func_def
    | func_def
;



/*================ GLOBAL DECL =================*/

global_decl_list
    : global_decl_list var_decl_stmt
    | var_decl_stmt
    ;

/*================ FUNCTION DECL =================*/

func_decl
    : func_header '(' formal_param_list ')' ';'
      {
          if (!sa_parse) {

              if (current_function_name != "main") {
                  fprintf(stderr,"Only main declaration allowed\n");
                  exit(1);
              }

              if (current_function_return_type != void_data_type) {
                  fprintf(stderr,"main must be void\n");
                  exit(1);
              }

              std::vector<Data_Type> types;
              std::vector<std::string> names;

              for (auto &p : *$3) {
                  types.push_back(p.type);
                  names.push_back(p.name);
              }

              global_symbol_table.declare_function(
                  current_function_name,
                  current_function_return_type,
                  types,
                  names
              );
          }
      }

    | func_header '(' ')' ';'
      {
          if (!sa_parse) {

              if (current_function_name != "main") {
                  fprintf(stderr,"Only main declaration allowed\n");
                  exit(1);
              }

              if (current_function_return_type != void_data_type) {
                  fprintf(stderr,"main must be void\n");
                  exit(1);
              }

              std::vector<Data_Type> types;
              std::vector<std::string> names;

              global_symbol_table.declare_function(
                  current_function_name,
                  current_function_return_type,
                  types,
                  names
              );
          }
      }
    ;

/*================ FUNCTION DEF =================*/



func_def
    : func_header '(' formal_param_list ')'
      {
          current_param_list = $3;
      }
      func_body
    | func_header '(' ')'
      {
          current_param_list = nullptr;
      }
      func_body
    ;

func_body
    : '{'
      {
          if (!sa_parse) {

              /* 1. Check main name */
              if (current_function_name != "main") {
                  fprintf(stderr,"Only main definition allowed\n");
                  exit(1);
              }

              /* 2. Check return type */
              if (current_function_return_type != void_data_type) {
                  fprintf(stderr,"main must be void\n");
                  exit(1);
              }

              std::vector<Data_Type> types;
              std::vector<std::string> names;

              /* 3. Extract parameters */
              if (current_param_list != nullptr) {
                  for (auto &p : *current_param_list) {
                      types.push_back(p.type);
                      names.push_back(p.name);
                  }
              }

              /* 4. Register function */
              global_symbol_table.define_function(
                  current_function_name,
                  current_function_return_type,
                  types,
                  names
              );

              /* 5. Switch to local scope BEFORE body */
              current_symbol_table = &local_symbol_table;

              /* 6. Insert parameters into local scope */
              for (size_t i = 0; i < types.size(); i++) {
                  current_symbol_table->add_variable(names[i], types[i]);
              }
          }
      }
      body_content
      '}'
      {
          if (!sa_parse) {

              /* 7. Set AST root */
              root_ast = $3;

              /* 8. Restore global scope */
              current_symbol_table = &global_symbol_table;

              current_param_list = nullptr;
          }
      }
    ;


body_content
    : var_decl_stmt_list statement_list
      {
          $$ = $2;   // Only statements form AST
      }
    | var_decl_stmt_list
      {
          $$ = new Sequence_Stmt_Ast();  // empty body
      }
    | statement_list
      {
          $$ = $1;
      }
    ;


/*================ HEADER =================*/

func_header
    : named_type NAME
      {
          if (!sa_parse) {
              current_function_name = std::string($2);
              current_function_return_type = $1;
          }
      }
;


/*================ PARAMETERS =================*/

formal_param_list
    : formal_param
      {
          if (!sa_parse) {
              $$ = new std::vector<Param>;
              $$->push_back(*$1);
          }
          else {
              $$ = nullptr;
          }
      }

    | formal_param_list ',' formal_param
      {
          if (!sa_parse) {
              $1->push_back(*$3);
              $$ = $1;
          }
          else {
              $$ = nullptr;
          }
      }
;

formal_param
    : param_type NAME
      {
          if (!sa_parse) {
              $$ = new Param;
              $$->type = $1;
              $$->name = std::string($2);
          }
          else {
              $$ = nullptr;
          }
      }
;

param_type
    : INTEGER { $$ = int_data_type; }
    | FLOAT   { $$ = float_data_type; }
    | STRING  { $$ = string_data_type; }
    | BOOL    { $$ = bool_data_type; }
;

/*================ DECLARATIONS =================*/

var_decl_stmt_list
    : var_decl_stmt
    | var_decl_stmt_list var_decl_stmt
;

var_decl_stmt
    : named_type var_decl_item_list ';'
;

var_decl_item_list
    : var_decl_item_list ',' var_decl_item
    | var_decl_item
;

var_decl_item
    : NAME
      {
          if (!sa_parse) {
              std::string var_name = std::string($1);

              current_symbol_table->add_variable(
                  var_name,
                  current_decl_type
              );
          }
      }
;

named_type
    : INTEGER
      {
          $$ = int_data_type;
          if (!sa_parse)
              current_decl_type = int_data_type;
      }

    | FLOAT
      {
          $$ = float_data_type;
          if (!sa_parse)
              current_decl_type = float_data_type;
      }

    | STRING
      {
          $$ = string_data_type;
          if (!sa_parse)
              current_decl_type = string_data_type;
      }

    | BOOL
      {
          $$ = bool_data_type;
          if (!sa_parse)
              current_decl_type = bool_data_type;
      }

    | VOID
      {
          $$ = void_data_type;
          if (!sa_parse)
              current_decl_type = void_data_type;
      }
;

/*================ STATEMENTS =================*/

statement_list
    : statement
      {
          if (!sa_parse) {
              $$ = new Sequence_Stmt_Ast();
              $$->add_statement($1);
          }
          else {
              $$ = nullptr;
          }
      }

    | statement_list statement
      {
          if (!sa_parse) {
              $1->add_statement($2);
              $$ = $1;
          }
          else {
              $$ = nullptr;
          }
      }
    ;

statement
    : assignment_statement
    | print_statement
    | read_statement
    ;

assignment_statement
    : variable_name ASSIGN_OP expression ';'
      {
          if (!sa_parse) {

              Name_Ast* lhs = dynamic_cast<Name_Ast*>($1);
              if (lhs == nullptr) {
                  fprintf(stderr,"Invalid assignment target\n");
                  exit(1);
              }

              $$ = new Assignment_Stmt_Ast(lhs, $3);
          }
          else {
              $$ = nullptr;
          }
      }
    ;

print_statement
    : WRITE expression ';'
      {
          if (!sa_parse)
              $$ = new Write_Stmt_Ast($2);
          else
              $$ = nullptr;
      }
    ;

read_statement
    : READ variable_name ';'
      {
          if (!sa_parse) {

              Name_Ast* var = dynamic_cast<Name_Ast*>($2);
              if (var == nullptr) {
                  fprintf(stderr,"Invalid read target\n");
                  exit(1);
              }

              $$ = new Read_Stmt_Ast(var);
          }
          else {
              $$ = nullptr;
          }
      }
    ;

/*================ EXPRESSIONS =================*/

expression
    : expression '+' expression
      {
          if (!sa_parse)
              $$ = new Plus_Expr_Ast($1,$3);
          else
              $$ = nullptr;
      }

    | expression '-' expression
      {
          if (!sa_parse)
              $$ = new Minus_Expr_Ast($1,$3);
          else
              $$ = nullptr;
      }

    | expression '*' expression
      {
          if (!sa_parse)
              $$ = new Mult_Expr_Ast($1,$3);
          else
              $$ = nullptr;
      }

    | expression '/' expression
      {
          if (!sa_parse)
              $$ = new Div_Expr_Ast($1,$3);
          else
              $$ = nullptr;
      }

    | '-' expression %prec UMINUS
      {
          if (!sa_parse)
              $$ = new UMinus_Expr_Ast($2);
          else
              $$ = nullptr;
      }

    | '(' expression ')'
      { $$ = $2; }

    | expression '?' expression ':' expression
      {
          if (!sa_parse)
              $$ = new Conditional_Expr_Ast($1,$3,$5);
          else
              $$ = nullptr;
      }

    | expression AND expression
      {
          if (!sa_parse)
              $$ = new Boolean_Expr_Ast($1,$3,and_op);
          else
              $$ = nullptr;
      }

    | expression OR expression
      {
          if (!sa_parse)
              $$ = new Boolean_Expr_Ast($1,$3,or_op);
          else
              $$ = nullptr;
      }

    | NOT expression
      {
          if (!sa_parse)
              $$ = new Not_Expr_Ast($2);
          else
              $$ = nullptr;
      }

    | rel_expression
      { $$ = $1; }

    | variable_name
      { $$ = $1; }

    | constant_as_operand
      { $$ = $1; }
    ;

rel_expression
    : expression LT expression
      {
          if (!sa_parse)
              $$ = new Relational_Expr_Ast($1,$3,lt_op);
          else
              $$ = nullptr;
      }

    | expression LE expression
      {
          if (!sa_parse)
              $$ = new Relational_Expr_Ast($1,$3,le_op);
          else
              $$ = nullptr;
      }

    | expression GT expression
      {
          if (!sa_parse)
              $$ = new Relational_Expr_Ast($1,$3,gt_op);
          else
              $$ = nullptr;
      }

    | expression GE expression
      {
          if (!sa_parse)
              $$ = new Relational_Expr_Ast($1,$3,ge_op);
          else
              $$ = nullptr;
      }

    | expression NE expression
      {
          if (!sa_parse)
              $$ = new Relational_Expr_Ast($1,$3,ne_op);
          else
              $$ = nullptr;
      }

    | expression EQ expression
      {
          if (!sa_parse)
              $$ = new Relational_Expr_Ast($1,$3,eq_op);
          else
              $$ = nullptr;
      }
    ;

/*================ VARIABLES =================*/

variable_name
    : NAME
      {
          if (!sa_parse) {

              std::string var_name = std::string($1);
              Symbol_Table_Entry* entry = nullptr;

              if (local_symbol_table.variable_present(var_name))
                  entry = local_symbol_table.lookup_variable(var_name);
              else
                  entry = global_symbol_table.lookup_variable(var_name);

              $$ = new Name_Ast(entry);
          }
          else {
              $$ = nullptr;
          }
      }
;

/*================ CONSTANTS =================*/

constant_as_operand
    : INT_NUM
      {
          if (!sa_parse)
              $$ = new Number_Ast($1);
          else
              $$ = nullptr;
      }

    | FLOAT_NUM
      {
          if (!sa_parse)
              $$ = new Number_Ast($1);
          else
              $$ = nullptr;
      }

    | STR_CONST
      {
          if (!sa_parse)
              $$ = new String_Ast(std::string($1));
          else
              $$ = nullptr;
      }
    ;

%%

int yyerror(const char *s)
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}