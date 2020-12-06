#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <list>
#include <cctype>
#include <vector>

class Expression
{
public:
    virtual Expression* eval(Env* data) = 0;
    virtual ~Expression()
    {}
};

class Env
{
    std::unordered_map<std::string, Expression*> env;
    friend class Var;
    friend class Let;
    friend class Call;
    friend class Set;
public:
    Env(std::unordered_map<std::string, Expression*> env): env(env)
    {}

    Env& operator=(const Env& other)
    {
        if (this != &other)
        {
            env = other.env;
        }
        return *this;
    }

    Expression* fromEnv(std::string& str)
    {
        try 
        {
            return env[str];
        }
        catch (...)
        {
            throw std::runtime_error("В env нет <expression>, соответствующего этому <id>");
        }
    }
};

class Val: public Expression
{
    int val;
    friend int getValue(Expression* expr);
public:
    Val(int val): val(val)
    {}

    Expression* eval(Env* data)
    {
        return this;
    }

    ~Val()
    {}
};

int getValue(Expression* expr)
{
    Val* value = dynamic_cast<Val*>(expr);
    if (value != nullptr)
        return value->val;
    else
        throw std::invalid_argument("Подаваемое на вход выражение не соответстует типу <val>");
}

class Var: public Expression
{
    std::string id;
public:
    Var(std::string& id): id(id)
    {}

    Expression* eval(Env* data)
    {
        data->env[id] = this;
        return data->fromEnv(id);
    }

    ~Var()
    {}
};

class Add: public Expression
{
    Expression* e1;
    Expression* e2;
public:
    Add(Expression* left, Expression* right): e1(left), e2(right)
    {}

    Expression* eval(Env* data)
    {
        Env data1 = *data;
        return new Val(getValue(e1->eval(&data1)) + getValue(e2->eval(&data1)));
    }

    ~Add()
    {
        delete e1;
        delete e2;
    }
};

class If: public Expression
{
    Expression* e1;
    Expression* e2;
    Expression* e_then;
    Expression* e_else;
public:
    If(Expression* lc, Expression* rc, Expression* then, Expression* else_): e1(lc), e2(rc), e_then(then), e_else(else_)
    {}

    Expression* eval(Env* data)
    {
        Env data1 = *data;
        if (getValue(e1->eval(&data1)) > getValue(e2->eval(&data1)))
            return e_then->eval(&data1);
        else
            return e_else->eval(&data1);
        
    }

    ~If()
    {
        delete e1;
        delete e2;
        delete e_then;
        delete e_else;
    }
};

class Let: public Expression
{
    std::string id;
    Expression* e_value;
    Expression* e_body;
public:
    Let(std::string& id, Expression* e_value, Expression* e_body): id(id), e_value(e_value), e_body(e_body)
    {}

    Expression* eval(Env* data)
    {
        data->env[id] = e_value->eval(data);
        Env data1 = *data;
        return e_body->eval(&data1);
    }

    ~Let()
    {
        delete e_value;
        delete e_body;
    }
};

class Function: public Expression
{
    std::string id;
    Expression* expr;
    friend class Call;
public:
    Function(std::string& id, Expression* expr): id(id), expr(expr)
    {}

    Expression* eval(Env* data)
    {
        return this;
    }

    ~Function()
    {
        delete expr;
    }
};

class Call: public Expression
{
    Expression* f_expr;
    Expression* arg_expr;
public:
    Call(Expression* f_expr, Expression* arg_expr): f_expr(f_expr), arg_expr(arg_expr)
    {}

    Expression* eval(Env* data)
    {
        Env data1 = *data;
        Expression* f_evaled = f_expr->eval(&data1);
        Function* func = dynamic_cast<Function*>(f_evaled);
        if (func == nullptr)
            throw std::invalid_argument("eval(f_expr) не является <function>");
        Var* var_expr = dynamic_cast<Var*>(f_evaled);
        if (var_expr != nullptr)
        {
            data->env[func->id] = func->expr;
            throw std::invalid_argument("Неверный тип данных");
        }
        data->env[func->id] = arg_expr->eval(&data1);
        return func->expr->eval(&data1);
    }

    ~Call()
    {
        delete f_expr;
        delete arg_expr;
    }
};

class Set: public Expression
{
    std::string id;
    Expression* e_val;
public:
    Set(std::string id, Expression* e_val): id(id), e_val(e_val)
    {}

    Expression* eval(Env* data)
    {
        data->env[id] = e_val;
        return this;
    }

    ~Set()
    {
        delete e_val;
    }
};

class Block: public Expression
{
    std::vector<Expression*> expr_list;
public:
    Block(std::vector<Expression*>& expr_list): expr_list(expr_list)
    {}

    Expression* eval(Env* data)
    {
        Env data1 = *data;
        for (size_t i = 0; i < expr_list.size(); i++)
            expr_list[i] = expr_list[i]->eval(&data1);
        return expr_list[expr_list.size() - 1];
    }
};

class Arr: public Expression
{
    std::list<Expression*> expr_list;
    friend class At;
public:
    Arr(std::list<Expression*>& expr_list): expr_list(expr_list)
    {}

    Expression* eval(Env *data)
    {
        Env data1 = *data;
        for (auto i = expr_list.begin(); i != expr_list.end(); i++)
        {
            *i = (*i)->eval(&data1);
        }
        return this;
    }
};

class Gen: public Expression
{
    Expression* e_length;
    Expression* e_function;
public:
    Gen(Expression* e_length, Expression* e_function): e_length(e_length), e_function(e_function)
    {}

    Expression* eval(Env* data)
    {
        int size = getValue(e_length->eval(data));
        std::list<Expression*> result;
        for (int i = 0; i < size; i++)
            result.push_back((new Call(e_function, new Val(i+1)))->eval(data));
        return new Arr(result);
    }

    ~Gen()
    {
        delete e_length;
        delete e_function;
    }
};

class At: public Expression
{
    Expression* e_array;
    Expression* e_index;
public:
    At(Expression* e_array, Expression* e_index): e_array(e_array), e_index(e_index)
    {}

    Expression* eval(Env* data)
    {
        int index = getValue(e_index->eval(data));
        Arr* array = dynamic_cast<Arr*>(e_array);
        if (array == nullptr)
            throw std::invalid_argument("e_array не является массивом");
        auto pos = array->expr_list.begin();
        int int_pos = 0;
        while (pos != array->expr_list.end())
        {
            if (int_pos == index)
                return *pos;
            pos++;
            int_pos++;
        }
        throw std::runtime_error("Выход за границы массива");
    }

    ~At()
    {
        delete e_array;
        delete e_index;
    }
};

std::string read(std::istream& in)
{
    std::string str;
    for (std::string line; std::getline(in, line); )
    {
        if (line != "\n")
            str += line;
        in.clear();
    }
    return str;
}

std::list<std::string> make_list(std::string& str)
{
    std::string elem;
    std::list<std::string> result;
    for (char sym: str)
    {
        if ((sym >= '0' && sym <= '9') || (sym >= 'A' && sym <= 'z') || sym == '-')
            elem += sym;
        else
        {
            if (elem.length())
                result.push_back(elem);
            elem.clear();
            if (sym != ' ')
            {
                elem += sym;
                result.push_back(elem);
                elem.clear();
            }
        }
    }
    if (elem.length())
        result.push_back(elem);
    return result;
}

Expression* get_expr(std::list<std::string>::iterator& pos)
{
    Expression* result;
    pos++;
    if (*pos == "val")
    {
        pos++;
        result = new Val(stoi(*pos));
        pos++;
        pos++;
        return result;
    }
    if (*pos == "var")
    {
        pos++;
        result = new Var(*pos);
        pos++;
        pos++;
        return result;
    }
    if (*pos == "add")
    {
        pos++;
        Expression* left = get_expr(pos);
        Expression* right = get_expr(pos);
        result = new Add(left, right);
        pos++;
        return result;
    }
    if (*pos == "if")
    {
        pos++;
        Expression* e1 = get_expr(pos);
        Expression* e2 = get_expr(pos);
        pos++;
        Expression* e_then = get_expr(pos);
        pos++;
        Expression* e_else = get_expr(pos);
        result = new If(e1, e2, e_then, e_else);
        pos++;
        return result;
    }
    if (*pos == "let")
    {
        pos++;
        std::string id = *pos;
        pos++;
        pos++;
        Expression* e_value = get_expr(pos);
        pos++;
        Expression* e_body = get_expr(pos);
        result = new Let(id, e_value, e_body);
        pos++;
        return result;
    }
    if (*pos == "function")
    {
        pos++;
        std::string id = *pos;
        pos++;
        Expression* expr = get_expr(pos);
        result = new Function(id, expr);
        pos++;
        return result;
    }
    if (*pos == "call")
    {
        pos++;
        Expression* f_expr = get_expr(pos);
        Expression* arg_expr = get_expr(pos);
        result = new Call(f_expr, arg_expr);
        pos++;
        return result;
    }
}

int main()
{
    std::ifstream input;
    std::ofstream output;
    std::string str;
    std::list<std::string> str_list;
    std::unordered_map<std::string, Expression*> data_map;
    Env data(data_map);
    try
    {
        input.open("input.txt");
        output.open("output.txt");
        str = read(input);
        input.close();
        str_list = make_list(str);
        std::list<std::string>::iterator pos = str_list.begin();
        Expression* expr = get_expr(pos);
        Expression* evaled_expr = expr->eval(&data);
        output << "(val " << getValue(evaled_expr) << ")";
        output.close();
        delete expr;
        delete evaled_expr;
    }
    catch (std::exception &exception)
    {
        std::cout << "ERROR";
    }
    return 0;
}
