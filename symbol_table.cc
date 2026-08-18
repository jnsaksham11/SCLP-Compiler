#include "symbol_table.hh"

/*==================================================*/
/*            Symbol_Table_Entry                    */
/*==================================================*/

Symbol_Table_Entry::Symbol_Table_Entry(string name,
                                       Data_Type type)
{
    variable_name = name;
    variable_type = type;
}

string Symbol_Table_Entry::get_variable_name() const
{
    return variable_name;
}

Data_Type Symbol_Table_Entry::get_data_type() const
{
    return variable_type;
}


/*==================================================*/
/*              Function_Info                       */
/*==================================================*/

Function_Info::Function_Info(string name,
                             Data_Type ret_type,
                             vector<Data_Type> params,
                             vector<string> names)
{
    function_name = name;
    return_type = ret_type;
    param_types = params;
    param_names = names;
    declared = false;
    defined = false;
}


string Function_Info::get_name() const
{
    return function_name;
}

Data_Type Function_Info::get_return_type() const
{
    return return_type;
}

vector<Data_Type> Function_Info::get_param_types() const
{
    return param_types;
}

vector<string> Function_Info::get_param_names() const
{
    return param_names;
}


bool Function_Info::is_declared() const
{
    return declared;
}

bool Function_Info::is_defined() const
{
    return defined;
}

void Function_Info::mark_declared()
{
    declared = true;
}

void Function_Info::mark_defined()
{
    defined = true;
}


/*==================================================*/
/*               Symbol_Table                       */
/*==================================================*/

Symbol_Table::Symbol_Table()
{
    function_info = nullptr;
}


/*================ VARIABLE =================*/

void Symbol_Table::add_variable(string name,
                                Data_Type type)
{
    if(type == void_data_type)
    {
        cerr << "Variable cannot be of type void: "
             << name << endl;
        exit(1);
    }

    if(variable_table.find(name) != variable_table.end())
    {
        cerr << "Variable redeclared: "
             << name << endl;
        exit(1);
    }

    variable_table[name] =
        new Symbol_Table_Entry(name, type);
}

Symbol_Table_Entry* Symbol_Table::lookup_variable(string name)
{
    auto it = variable_table.find(name);

    if(it == variable_table.end())
        return nullptr;

    return it->second;
}

bool Symbol_Table::variable_present(string name)
{
    return (variable_table.find(name)
            != variable_table.end());
}


/*================ FUNCTION =================*/

void Symbol_Table::declare_function(string name,
                                    Data_Type return_type,
                                    vector<Data_Type> params, vector<string> param_names)
{
    if(name != "main")
    {
        cerr << "Only main function allowed in A2"
             << endl;
        exit(1);
    }

    if(return_type != void_data_type)
    {
        cerr << "main must have void return type"
             << endl;
        exit(1);
    }

    if(function_info != nullptr)
    {
        cerr << "Function already declared"
             << endl;
        exit(1);
    }

    function_info = new Function_Info(name, return_type, params, param_names);


    function_info->mark_declared();
}

void Symbol_Table::define_function(string name,
                                   Data_Type return_type,
                                   vector<Data_Type> params, vector<string> param_names)
{
    if(name != "main")
    {
        cerr << "Only main function allowed in A2"
             << endl;
        exit(1);
    }

    if(return_type != void_data_type)
    {
        cerr << "main must have void return type"
             << endl;
        exit(1);
    }

    if(function_info == nullptr)
    {
        /* No declaration earlier — allowed */
        function_info = new Function_Info(name, return_type, params, param_names);


        function_info->mark_defined();
        return;
    }

    if(function_info->is_defined())
    {
        cerr << "Function already defined"
             << endl;
        exit(1);
    }

    /* Check parameter count */
    vector<Data_Type> decl_params =
        function_info->get_param_types();

    if(decl_params.size() != params.size())
    {
        cerr << "Parameter count mismatch "
             << "between declaration and definition"
             << endl;
        exit(1);
    }

    /* Check parameter types */
    for(size_t i = 0; i < params.size(); i++)
    {
        if(decl_params[i] != params[i])
        {
            cerr << "Parameter type mismatch "
                 << "between declaration and definition"
                 << endl;
            exit(1);
        }
    }

    function_info->mark_defined();
}

void Symbol_Table::print()
{
    for (auto &entry : variable_table) {
        cout << "Name: " << entry.first
             << " | Type: " << entry.second->get_data_type()
             << endl;
    }
}


Function_Info* Symbol_Table::get_function()
{
    return function_info;
}

std::string data_type_to_string(Data_Type t)
{
    switch(t) {
        case int_data_type:    return "int";
        case float_data_type:  return "float";
        case string_data_type: return "string";
        case bool_data_type:   return "bool";
        case void_data_type:   return "void";
        case error_data_type:  return "error";
    }

    return "";
}
