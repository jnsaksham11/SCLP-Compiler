#ifndef AST_HH
#define AST_HH

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Symbol_Table_Entry;

/*================= DATA TYPES =================*/

enum Data_Type {
    int_data_type,
    float_data_type,
    bool_data_type,
    string_data_type,
    void_data_type,
    error_data_type
};

struct Param {
    Data_Type type;
    std::string name;
};

enum Relational_Op {
    lt_op, le_op, gt_op, ge_op, eq_op, ne_op
};

enum Boolean_Op {
    and_op, or_op
};


/*================= BASE AST =================*/

class Ast {
public:
    virtual void print_ast(int indent) = 0;
    virtual ~Ast() {}
};

/*================= EXPRESSIONS =================*/

class Expression_Ast : public Ast {
protected:
    Data_Type node_data_type;

public:
    Expression_Ast(Data_Type dt) : node_data_type(dt) {}
    Data_Type get_data_type() const { return node_data_type; }
};

/*--------- Base Expressions (Arity 0) ----------*/

class Base_Expr_Ast : public Expression_Ast {
public:
    Base_Expr_Ast(Data_Type dt) : Expression_Ast(dt) {}
};


class Number_Ast : public Base_Expr_Ast {
private:
    int int_value;
    double float_value;

public:
    Number_Ast(int val);
    Number_Ast(double val);
    void print_ast(int indent);
};


class String_Ast : public Base_Expr_Ast {
    string str;
public:
    String_Ast(string s);
    void print_ast(int indent);
};

class Name_Ast : public Base_Expr_Ast {
    // string name;
    Symbol_Table_Entry* entry;
public:
    // Name_Ast(string n, Data_Type dt);
    // string get_name() const;
    Name_Ast(Symbol_Table_Entry* e);
    Symbol_Table_Entry* get_entry() const;
    void print_ast(int indent);
};

/*--------- Unary Expressions ----------*/

class Unary_Expr_Ast : public Expression_Ast {
protected:
    Expression_Ast *operand;
public:
    Unary_Expr_Ast(Expression_Ast *expr, Data_Type dt);
    Expression_Ast* get_operand() const { return operand; }
};

class UMinus_Expr_Ast : public Unary_Expr_Ast {
public:
    UMinus_Expr_Ast(Expression_Ast *expr);
    void print_ast(int indent);
};

class Not_Expr_Ast : public Unary_Expr_Ast {
public:
    Not_Expr_Ast(Expression_Ast *expr);
    void print_ast(int indent);
};

/*--------- Binary Expressions ----------*/

class Binary_Expr_Ast : public Expression_Ast {
protected:
    Expression_Ast *lhs;
    Expression_Ast *rhs;
public:
    Binary_Expr_Ast(Expression_Ast *l,
                    Expression_Ast *r,
                    Data_Type dt);

    Expression_Ast* get_lhs() const { return lhs; }
    Expression_Ast* get_rhs() const { return rhs; }
};

class Plus_Expr_Ast : public Binary_Expr_Ast {
public:
    Plus_Expr_Ast(Expression_Ast *l,
                  Expression_Ast *r);
    void print_ast(int indent);
};

class Minus_Expr_Ast : public Binary_Expr_Ast {
public:
    Minus_Expr_Ast(Expression_Ast *l,
                   Expression_Ast *r);
    void print_ast(int indent);
};

class Mult_Expr_Ast : public Binary_Expr_Ast {
public:
    Mult_Expr_Ast(Expression_Ast *l,
                  Expression_Ast *r);
    void print_ast(int indent);
};

class Div_Expr_Ast : public Binary_Expr_Ast {
public:
    Div_Expr_Ast(Expression_Ast *l,
                 Expression_Ast *r);
    void print_ast(int indent);
};

class Relational_Expr_Ast : public Binary_Expr_Ast {
    Relational_Op op;
public:
    // Relational_Expr_Ast(Expression_Ast *l,
    //                     Expression_Ast *r);
    Relational_Expr_Ast(Expression_Ast* l,
                    Expression_Ast* r,
                    Relational_Op oper);
    void print_ast(int indent);
};

class Boolean_Expr_Ast : public Binary_Expr_Ast {
    Boolean_Op op;
public:
    // Boolean_Expr_Ast(Expression_Ast *l,
    //                  Expression_Ast *r);
    Boolean_Expr_Ast(Expression_Ast* l,
                 Expression_Ast* r,
                 Boolean_Op oper);
    void print_ast(int indent);
};

/*--------- Ternary Expression ----------*/

class Ternary_Expr_Ast : public Expression_Ast {
protected:
    Expression_Ast *condition;
    Expression_Ast *true_part;
    Expression_Ast *false_part;
public:
    Ternary_Expr_Ast(Expression_Ast *c,
                     Expression_Ast *t,
                     Expression_Ast *f,
                     Data_Type dt);

    Expression_Ast* get_condition() const { return condition; }
    Expression_Ast* get_true_part() const { return true_part; }
    Expression_Ast* get_false_part() const { return false_part; }

};

class Conditional_Expr_Ast : public Ternary_Expr_Ast {
public:
    Conditional_Expr_Ast(Expression_Ast *c,
                         Expression_Ast *t,
                         Expression_Ast *f);
    void print_ast(int indent);
};

/*================= STATEMENTS =================*/

class Statement_Ast : public Ast {
public:
    virtual ~Statement_Ast() {}
};


class Assignment_Stmt_Ast : public Statement_Ast {
    Name_Ast *lhs;
    Expression_Ast *rhs;
public:
    Assignment_Stmt_Ast(Name_Ast *l,
                        Expression_Ast *r);
    void print_ast(int indent);
};

class Read_Stmt_Ast : public Statement_Ast {
    Name_Ast *var;
public:
    Read_Stmt_Ast(Name_Ast *v);
    void print_ast(int indent);
};

class Write_Stmt_Ast : public Statement_Ast {
    Expression_Ast *expr;
public:
    Write_Stmt_Ast(Expression_Ast *e);
    void print_ast(int indent);
};

class Sequence_Stmt_Ast : public Statement_Ast {
    vector<Statement_Ast*> stmt_list;
public:
    void add_statement(Statement_Ast *stmt);
    void print_ast(int indent);
};

#endif