#include "ast.hh"
#include "symbol_table.hh"
#include <cstdlib>

extern FILE *astfile;

/*======================================================*/
/*                 Utility Function                     */
/*======================================================*/

static void print_indent(int indent)
{
    for(int i = 0; i < indent; i++)
        fprintf(astfile, " ");
}


/*======================================================*/
/*                 Number_Ast                           */
/*======================================================*/

Number_Ast::Number_Ast(int val)
    : Base_Expr_Ast(int_data_type), int_value(val) {}

Number_Ast::Number_Ast(double val)
    : Base_Expr_Ast(float_data_type), float_value(val) {}

void Number_Ast::print_ast(int indent)
{
    // print_indent(indent);

    if(node_data_type == int_data_type)
        fprintf(astfile, "Num : %d<int>\n", int_value);
    else
        fprintf(astfile, "Num : %.2f<float>\n", float_value);
}

/*======================================================*/
/*                 String_Ast                           */
/*======================================================*/

String_Ast::String_Ast(string s)
    : Base_Expr_Ast(string_data_type), str(s) {}

void String_Ast::print_ast(int indent)
{
    // print_indent(indent);
    fprintf(astfile, "String : %s<string>\n", str.c_str());
}

/*======================================================*/
/*                 Name_Ast                             */
/*======================================================*/

Name_Ast::Name_Ast(Symbol_Table_Entry* e)
    : Base_Expr_Ast(e->get_data_type()), entry(e) {}

Symbol_Table_Entry* Name_Ast::get_entry() const
{
    return entry;
}

void Name_Ast::print_ast(int indent)
{
    // print_indent(indent);

    const char *type_str;

    switch(node_data_type) {
        case int_data_type: type_str = "int"; break;
        case float_data_type: type_str = "float"; break;
        case bool_data_type: type_str = "bool"; break;
        case string_data_type: type_str = "string"; break;
        default: type_str = "unknown";
    }

    fprintf(astfile, "Name : %s_<%s>\n",
            entry->get_variable_name().c_str(),
            type_str);
}

/*======================================================*/
/*                 Unary Base                           */
/*======================================================*/

Unary_Expr_Ast::Unary_Expr_Ast(Expression_Ast *expr,
                               Data_Type dt)
    : Expression_Ast(dt), operand(expr) {}

/*======================================================*/
/*                 UMinus_Expr_Ast                      */
/*======================================================*/

UMinus_Expr_Ast::UMinus_Expr_Ast(Expression_Ast *expr)
    : Unary_Expr_Ast(expr, expr->get_data_type())
{
    Data_Type t = expr->get_data_type();

    if(t != int_data_type && t != float_data_type)
    {
        cerr << "Type error in unary minus" << endl;
        exit(1);
    }
}

void UMinus_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);

    const char *type_str = (node_data_type == int_data_type) ? "int" : "float";

    fprintf(astfile, "Arith: UMinus<%s>\n", type_str);

    print_indent(indent);
    fprintf(astfile, "L_Opd (");
    operand->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");
}


/*======================================================*/
/*                 Not_Expr_Ast                         */
/*======================================================*/

Not_Expr_Ast::Not_Expr_Ast(Expression_Ast *expr)
    : Unary_Expr_Ast(expr, bool_data_type)
{
    if(expr->get_data_type() != bool_data_type)
    {
        cerr << "Type error in NOT expression" << endl;
        exit(1);
    }
}

void Not_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);
    fprintf(astfile, "Condition: NOT<bool>\n");

    print_indent(indent);
    fprintf(astfile, "L_Opd ( ");
    operand->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");
}

/*======================================================*/
/*                 Binary Base                          */
/*======================================================*/

Binary_Expr_Ast::Binary_Expr_Ast(Expression_Ast *l,
                                 Expression_Ast *r,
                                 Data_Type dt)
    : Expression_Ast(dt), lhs(l), rhs(r) {}

/*======================================================*/
/*                 Arithmetic Expressions               */
/*======================================================*/

static Data_Type check_arithmetic(Expression_Ast *l,
                                  Expression_Ast *r)
{
    if(l->get_data_type() == int_data_type &&
       r->get_data_type() == int_data_type)
        return int_data_type;

    if(l->get_data_type() == float_data_type &&
       r->get_data_type() == float_data_type)
        return float_data_type;

    cerr << "Type error in arithmetic expression" << endl;
    exit(1);
}

Plus_Expr_Ast::Plus_Expr_Ast(Expression_Ast *l,
                             Expression_Ast *r)
    : Binary_Expr_Ast(l, r, check_arithmetic(l, r)) {}

void Plus_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);

    const char *type_str = (node_data_type == int_data_type) ? "int" : "float";

    fprintf(astfile, "Arith: Plus<%s>\n", type_str);

    print_indent(indent);
    fprintf(astfile, "L_Opd ( ");
    lhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");

    print_indent(indent);
    fprintf(astfile, "R_Opd ( ");
    rhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");
}

Minus_Expr_Ast::Minus_Expr_Ast(Expression_Ast *l,
                               Expression_Ast *r)
    : Binary_Expr_Ast(l, r, check_arithmetic(l, r)) {}

void Minus_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);

    const char *type_str = (node_data_type == int_data_type) ? "int" : "float";

    fprintf(astfile, "Arith: Minus<%s>\n", type_str);

    print_indent(indent);
    fprintf(astfile, "L_Opd ( ");
    lhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");

    print_indent(indent);
    fprintf(astfile, "R_Opd ( ");
    rhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");
}


Mult_Expr_Ast::Mult_Expr_Ast(Expression_Ast *l,
                             Expression_Ast *r)
    : Binary_Expr_Ast(l, r, check_arithmetic(l, r)) {}

void Mult_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);

    const char *type_str = (node_data_type == int_data_type) ? "int" : "float";

    fprintf(astfile, "Arith: Mult<%s>\n", type_str);

    print_indent(indent);
    fprintf(astfile, "L_Opd ( ");
    lhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");

    print_indent(indent);
    fprintf(astfile, "R_Opd ( ");
    rhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");
}

Div_Expr_Ast::Div_Expr_Ast(Expression_Ast *l,
                           Expression_Ast *r)
    : Binary_Expr_Ast(l, r, check_arithmetic(l, r)) {}

void Div_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);

    const char *type_str = (node_data_type == int_data_type) ? "int" : "float";

    fprintf(astfile, "Arith: Div<%s>\n", type_str);

    print_indent(indent);
    fprintf(astfile, "L_Opd ( ");
    lhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");

    print_indent(indent);
    fprintf(astfile, "R_Opd ( ");
    rhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");
}

/*======================================================*/
/*                 Relational                           */
/*======================================================*/

Relational_Expr_Ast::Relational_Expr_Ast(Expression_Ast *l,
                                         Expression_Ast *r,
                                         Relational_Op oper)
    : Binary_Expr_Ast(l, r, bool_data_type), op(oper)
{
    Data_Type lt = l->get_data_type();
    Data_Type rt = r->get_data_type();

    if(lt != rt)
    {
        cerr << "Type mismatch in relational expression" << endl;
        exit(1);
    }

    if(lt != int_data_type && lt != float_data_type)
    {
        cerr << "Relational operators require int or float" << endl;
        exit(1);
    }
}

void Relational_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);

    const char *op_str;

    switch(op)
    {
        case lt_op: op_str = "LT"; break;
        case le_op: op_str = "LE"; break;
        case gt_op: op_str = "GT"; break;
        case ge_op: op_str = "GE"; break;
        case eq_op: op_str = "EQ"; break;
        case ne_op: op_str = "NE"; break;
    }

    fprintf(astfile, "Condition: %s<bool>\n", op_str);

    print_indent(indent);
    fprintf(astfile, "L_Opd ( ");
    lhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");

    print_indent(indent);
    fprintf(astfile, "R_Opd ( ");
    rhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");
}


/*======================================================*/
/*                 Boolean                              */
/*======================================================*/

Boolean_Expr_Ast::Boolean_Expr_Ast(Expression_Ast *l,
                                   Expression_Ast *r,
                                   Boolean_Op oper)
    : Binary_Expr_Ast(l, r, bool_data_type), op(oper)
{
    if(l->get_data_type() != bool_data_type ||
       r->get_data_type() != bool_data_type)
    {
        cerr << "Type error in boolean expression" << endl;
        exit(1);
    }
}

void Boolean_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);

    const char *op_str = (op == and_op) ? "AND" : "OR";

    fprintf(astfile, "Condition: %s<bool>\n", op_str);

    print_indent(indent);
    fprintf(astfile, "L_Opd ( ");
    lhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");

    print_indent(indent);
    fprintf(astfile, "R_Opd ( ");
    rhs->print_ast(indent + 2);
    // print_indent(indent);
    fprintf(astfile, ")\n");
}


/*======================================================*/
/*                 Ternary Base                         */
/*======================================================*/

Ternary_Expr_Ast::Ternary_Expr_Ast(Expression_Ast *c,
                                   Expression_Ast *t,
                                   Expression_Ast *f,
                                   Data_Type dt)
    : Expression_Ast(dt),
      condition(c),
      true_part(t),
      false_part(f) {}

/*======================================================*/
/*                 Conditional                          */
/*======================================================*/

Conditional_Expr_Ast::Conditional_Expr_Ast(Expression_Ast *c,
                                           Expression_Ast *t,
                                           Expression_Ast *f)
    : Ternary_Expr_Ast(c, t, f, t->get_data_type())
{
    if(c->get_data_type() != bool_data_type)
    {
        cerr << "Condition must be boolean" << endl;
        exit(1);
    }

    if(t->get_data_type() != f->get_data_type())
    {
        cerr << "Type mismatch in conditional expression" << endl;
        exit(1);
    }
}

void Conditional_Expr_Ast::print_ast(int indent)
{
    // print_indent(indent);

    condition->print_ast(indent + 4);

    print_indent(indent + 2);
    fprintf(astfile, "True_Part (");
    true_part->print_ast(0);  // no indent inside inline
    fprintf(astfile, ")\n");

    print_indent(indent + 2);
    fprintf(astfile, "False_Part (");
    false_part->print_ast(0);
    fprintf(astfile, ")\n");

    print_indent(indent);
}


/*======================================================*/
/*                 Assignment                           */
/*======================================================*/

Assignment_Stmt_Ast::Assignment_Stmt_Ast(Name_Ast *l,
                                         Expression_Ast *r)
    : lhs(l), rhs(r)
{
    if(l->get_data_type() != r->get_data_type())
    {
        cerr << "Type mismatch in assignment" << endl;
        exit(1);
    }
}

void Assignment_Stmt_Ast::print_ast(int indent)
{
    // print_indent(indent);
    fprintf(astfile, "Asgn:\n");

    print_indent(indent + 2);
    fprintf(astfile, "LHS (");
    lhs->print_ast(0);
    fprintf(astfile, ")\n");

    print_indent(indent + 2);
    fprintf(astfile, "RHS (\n");
    rhs->print_ast(indent + 4);
    print_indent(indent + 2);
    fprintf(astfile, ")\n");
}


/*======================================================*/
/*                 Read                                 */
/*======================================================*/

Read_Stmt_Ast::Read_Stmt_Ast(Name_Ast *v)
    : var(v)
{
    Data_Type t = v->get_data_type();

    if(t != int_data_type && t != float_data_type)
    {
        cerr << "Read supports only int or float variables" << endl;
        exit(1);
    }
}

void Read_Stmt_Ast::print_ast(int indent)
{
    // print_indent(indent);
    fprintf(astfile, "Read: ");
    var->print_ast(0);
}

/*======================================================*/
/*                 Write                                */
/*======================================================*/

Write_Stmt_Ast::Write_Stmt_Ast(Expression_Ast *e)
    : expr(e)
{
    Data_Type t = e->get_data_type();

    if(t == bool_data_type)
    {
        cerr << "Cannot print boolean values" << endl;
        exit(1);
    }
}

void Write_Stmt_Ast::print_ast(int indent)
{
    // print_indent(indent);
    fprintf(astfile, "Write: ");
    expr->print_ast(0);
}

/*======================================================*/
/*                 Sequence                             */
/*======================================================*/

void Sequence_Stmt_Ast::add_statement(Statement_Ast *stmt)
{
    stmt_list.push_back(stmt);
}

void Sequence_Stmt_Ast::print_ast(int indent)
{
    fprintf(astfile, "**BEGIN: Abstract Syntax Tree\n");

    for(auto stmt : stmt_list)
        stmt->print_ast(2);

    fprintf(astfile, "**END: Abstract Syntax Tree\n");
}
