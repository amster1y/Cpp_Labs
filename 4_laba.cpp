#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <list>
#include <cctype>

class Expression
{
public:
    virtual Expression* eval() = 0;
    virtual ~Expression()
    {}
};

class Env
{
    std::unordered_map<std::string, Expression*> env;
    friend class Var;
    friend class Let;
    friend class Call;
public:
    Expression* fromEnv(std::string str)
    {
        try 
        {
            return env[str];
        }
        catch (...)
        {
            throw "В env нет <expression>, соответствующего этому <id>";
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

    Expression* eval()
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
        throw "Подаваемое на вход выражение не соответстует типу <val>";
}

class Var: public Expression
{
    std::string id;
    Env* data;
public:
    Var(std::string id, Env* data): id(id), data(data)
    {
        data->env[id] = this;
    }

    Expression* eval()
    {
        try
        {
            return data->fromEnv(id);
        }
        catch(const char* msg)
        {
            throw msg;
        }
    }

    ~Var()
    {
        data->env.erase(id);
    }
};

class Add: public Expression
{
    Expression* e1;
    Expression* e2;
public:
    Add(Expression* left, Expression* right): e1(left), e2(right)
    {}

    Expression* eval()
    {
        try
        {
            return new Val(getValue(e1->eval()) + getValue(e2->eval()));
        }
        catch(const char* msg)
        {
            throw msg;
        }
        
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

    Expression* eval()
    {
        try
        {
            if (getValue(e1->eval()) > getValue(e2->eval()))
                return e_then->eval();
            else
                return e_else->eval();
        }
        catch(const char* msg)
        {
            throw msg;
        }
        
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
    Env* data;
public:
    Let(std::string id, Expression* e_value, Expression* e_body, Env* data): id(id), e_value(e_value), e_body(e_body), data(data)
    {}

    Expression* eval()
    {
        try
        {
            data->env[id] = e_value->eval();
            return e_body->eval();
        }
        catch(const char* msg)
        {
            throw msg;
        }
        
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
    Function(std::string id, Expression* expr): id(id), expr(expr)
    {}

    Expression* eval()
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
    Env* data;
public:
    Call(Expression* f_expr, Expression* arg_expr, Env* data): f_expr(f_expr), arg_expr(arg_expr), data(data)
    {}

    Expression* eval()
    {
        try
        {
            Expression* f_evaled = f_expr->eval();
            Function* func = dynamic_cast<Function*>(f_evaled);
            if (func == nullptr)
                throw "eval(f_expr) не является <function>";
            Var* var_expr = dynamic_cast<Var*>(f_evaled);
            if (var_expr != nullptr)
                data->env[func->id] = func->expr;
            data->env[func->id] = arg_expr->eval();
            return func->expr->eval();
        }
        catch(const char* msg)
        {
            throw msg;
        }
    }

    ~Call()
    {
        delete f_expr;
        delete arg_expr;
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

Expression* get_expr(std::list<std::string>::iterator& pos, Env* data)
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
        result = new Var(*pos, data);
        pos++;
        pos++;
        return result;
    }
    if (*pos == "add")
    {
        pos++;
        Expression* left = get_expr(pos, data);
        Expression* right = get_expr(pos, data);
        result = new Add(left, right);
        pos++;
        return result;
    }
    if (*pos == "if")
    {
        pos++;
        Expression* e1 = get_expr(pos, data);
        Expression* e2 = get_expr(pos, data);
        pos++;
        Expression* e_then = get_expr(pos, data);
        pos++;
        Expression* e_else = get_expr(pos, data);
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
        Expression* e_value = get_expr(pos, data);
        pos++;
        Expression* e_body = get_expr(pos, data);
        result = new Let(id, e_value, e_body, data);
        pos++;
        return result;
    }
    if (*pos == "function")
    {
        pos++;
        std::string id = *pos;
        pos++;
        Expression* expr = get_expr(pos, data);
        result = new Function(id, expr);
        pos++;
        return result;
    }
    if (*pos == "call")
    {
        pos++;
        Expression* f_expr = get_expr(pos, data);
        Expression* arg_expr = get_expr(pos, data);
        result = new Call(f_expr, arg_expr, data);
        pos++;
        return result;
    }
}

int main()
{
    std::ifstream input;
    std::string str;
    std::list<std::string> str_list;
    Env data;
    try
    {
        input.open("input.txt");
        str = read(input);
        input.close();
        str_list = make_list(str);
        std::list<std::string>::iterator pos = str_list.begin();
        Expression* expr = get_expr(pos, &data);
        Expression* evaled_expr = expr->eval();
        std::cout << "(val " << getValue(evaled_expr) << ")";
    }
    catch (const char* msg)
    {
        std::cout << "ERROR";
    }
    return 0;
}