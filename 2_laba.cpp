#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Expression
{;
public:
    friend Expression* read(string str);
    friend Expression* read_without_brackets(string str);
    virtual ostream & print(ostream &type) const = 0;
    virtual Expression* derivative(string v) const = 0;
    virtual int eval(string str) const = 0;
    virtual ~Expression()
    {}
};

class Number: public Expression
{
protected:
    int num;
public:
    Number(int num): num(num) 
    {}

    ostream & print(ostream &type) const
    {
        type << num;
        return type;
    }

    Expression* derivative(string v) const
    {
        return new Number(0);
    }

    int eval(string str) const
    {
        return num;
    }

    ~Number()
    {}
};

class Variable: public Expression
{
protected:
    string var;
public:
    Variable(string var): var(var)
    {}

    ostream & print(ostream &type) const
    {
        type << var;
        return type;
    }

    Expression* derivative(string v) const
    {
        if (v==var)
            return new Number(1);
        else
            return new Number(0);    
    }

    int eval(string str) const
    {
        if (str.find(var) == string::npos)
            throw "Нет означивания одной из переменнных в выражении";
        str = str.substr(str.find(var), str.length());
        if (str.find(";") != string::npos)
            str.substr(0, str.find(";"));
        int pos = str.find("<-");
        int answer = atoi(str.substr(pos+3, str.length()).c_str());
        return answer;
    }

    ~Variable()
    {}
};

class Add: public Expression
{
protected:
    Expression* left;
    Expression* right;
    friend class Sub;
    friend class Mul;
    friend class Div;
public:
    Add(Expression* left, Expression* right): left(left), right(right)
    {}

    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
    ~Add();
};

class Sub: public Expression
{
protected:
    Expression *left;
    Expression *right;
    friend class Add;
    friend class Mul;
    friend class Div;
public:
    Sub(Expression *left, Expression *right): left(left), right(right)
    {}

    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
    ~Sub();
};

class Mul: public Expression
{
protected:
    Expression *left;
    Expression *right;
    friend class Add;
    friend class Sub;
    friend class Div;
public:
    Mul(Expression *left, Expression *right): left(left), right(right)
    {}

    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
    ~Mul();
};

class Div: public Expression
{
protected:
    Expression *left;
    Expression *right;
    friend class Add;
    friend class Sub;
    friend class Mul;
public:
    Div(Expression *left, Expression *right): left(left), right(right)
    {}

    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
    ~Div();
};

Add::~Add()
{
    delete left;
    delete right;
} 

ostream & Add::print(ostream &type) const
{
    type << "(";
    left->print(type);
    type << "+";
    right->print(type);
    type << ")";
    return type;
}

int Add::eval(string str) const
{
    return left->eval(str) + right->eval(str);
}

Expression* Add::derivative(string v) const
{
    return new Add(left->derivative(v), right->derivative(v));
}

ostream & Sub::print(ostream &type) const
{
    type << "(";
    left->print(type);
    type << "-";
    right->print(type);
    type << ")";
    return type;
}

int Sub::eval(string str) const
{
    return left->eval(str) - right->eval(str);
}

Expression* Sub::derivative(string v) const
{
    return new Sub(left->derivative(v), right->derivative(v));
}

Sub::~Sub()
{
    delete left;
    delete right;
} 

ostream & Mul::print(ostream &type) const
{
    type << "(";
    left->print(type);
    type << "*";
    right->print(type);
    type << ")";
    return type;
}

int Mul::eval(string str) const
{
    return left->eval(str) * right->eval(str);
}

Expression* Mul::derivative(string v) const
{
    return new Add(new Mul(left->derivative(v), right), new Mul(left, right->derivative(v)));
}

Mul::~Mul()
{
    delete left;
    delete right;
} 

ostream & Div::print(ostream &type) const
{
    type << "(";
    left->print(type);
    type << "/";
    right->print(type);
    type << ")";
    return type;
}

int Div::eval(string str) const
{
    return left->eval(str) / right->eval(str);
}

Expression* Div::derivative(string v) const
{
    return new Div(new Sub(new Mul(left->derivative(v), right), new Mul(left, right->derivative(v))), 
    new Mul(right, right));
}

Div::~Div()
{
    delete left;
    delete right;
}

Expression* read_without_brackets(string str)
{
    int pos = str.length() - 1;
    while (pos != 0)
    {
        if (str[pos] == '+')
            return new Add(read(str.substr(0, pos-1)), read(str.substr(pos+1, str.length())));
        if (str[pos] == '-')
            return new Sub(read(str.substr(0, pos-1)), read(str.substr(pos+1, str.length())));
        pos--;
    }
    pos = str.length() - 1;
    while (pos != 0)
    {
        if (str[pos] == '*')
            return new Mul(read(str.substr(0, pos-1)), read(str.substr(pos+1, str.length())));
        if (str[pos] == '/')
            return new Div(read(str.substr(0, pos-1)), read(str.substr(pos+1, str.length())));
        pos--;
    }
}

Expression* read(string str)
{
    int brackets = 0;
    int pos = 0;
    if (str.find('(') == string::npos)
    {
        if (str[0] >= '0' && str[0] <= '9')
            return new Number(atoi(str.c_str()));
        else
            if (str.find('+') == string::npos && str.find('-') == string::npos && str.find('*') == string::npos && str.find('/') == string::npos)
                return new Variable(str);
            else
                return read_without_brackets(str);
    }
    str = str.substr(1, str.length() - 2);
    while (pos <= str.length())
    {
        if (str[pos] == '(')
            brackets++;
        if (str[pos] == ')')
            brackets--;
        if (brackets == 0)
        {
            string l = str.substr(0, pos);
            string r = str.substr(pos+1, str.length() - pos - 1);
            if (str[pos] == '+')
                return new Add(read(l), read(r));
            if (str[pos] == '-')
                return new Sub(read(l), read(r));
            if (str[pos] == '*')
                return new Mul(read(l), read(r));
            if (str[pos] == '/')
                return new Div(read(l), read(r));
        }
        pos++;
    }
}

int main()
{
    string str;
    std::cin >> str;
    Expression* e = read(str);
    Expression* de = e->derivative("x");
    de->print(cout);
    return 0;
}
