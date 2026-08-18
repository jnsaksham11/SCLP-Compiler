#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include "ast.hh"

using namespace std;

/*==================================================*/
/*              Variable Entry                      */
/*==================================================*/


class Symbol_Table_Entry {
    string variable_name;
    Data_Type variable_type;

public:
    Symbol_Table_Entry(string name, Data_Type type);

    string get_variable_name() const;
    Data_Type get_data_type() const;
};


/*==================================================*/
/*              Function Information                */
/*==================================================*/

class Function_Info {
    string function_name;
    Data_Type return_type;
    vector<Data_Type> param_types;
    vector<string> param_names;
    bool declared;
    bool defined;

public:
    Function_Info(string name,
              Data_Type ret_type,
              vector<Data_Type> params,
              vector<string> param_names);

    string get_name() const;
    Data_Type get_return_type() const;
    vector<Data_Type> get_param_types() const;
    vector<string> get_param_names() const;

    bool is_declared() const;
    bool is_defined() const;

    void mark_declared();
    void mark_defined();
};


/*==================================================*/
/*                Symbol Table                      */
/*==================================================*/

class Symbol_Table {

    /* Variables */
    map<string, Symbol_Table_Entry*> variable_table;

    /* Only one function allowed in A2: main */
    Function_Info* function_info;

public:

    Symbol_Table();

    /* Variable Handling */
    void add_variable(string name, Data_Type type);
    Symbol_Table_Entry* lookup_variable(string name);
    bool variable_present(string name);
    void print();

    /* Function Handling */
    void declare_function(string name,
                          Data_Type return_type,
                          vector<Data_Type> params, vector<string> param_names);

    void define_function(string name,
                         Data_Type return_type,
                         vector<Data_Type> params, vector<string> param_names);

    Function_Info* get_function();
};

std::string data_type_to_string(Data_Type t);


#endif