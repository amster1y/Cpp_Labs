#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Expression
{;
public:
    friend Expression* read(string str);
    friend Expression* simplify(Expression* ex);
    virtual ostream & print(ostream &type) const = 0;
    virtual Expression* derivative(string v) const = 0;
    virtual int eval(string str) const = 0;
    virtual bool operator==(Expression* other) const = 0;
    virtual ~Expression()
    {}
};

class Number: public Expression
{
protected:
    int num;
public:
    friend Expression* simplify(Expression* ex);
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

    bool operator==(Expression* other) const
    {
        Number* n = dynamic_cast<Number*>(other);
        if (n == nullptr)
            return false;
        else
            return (n->num == num);
    }

    ~Number()
    {}
};

class Variable: public Expression
{
protected:
    string var;
public:
    friend Expression* simplify(Expression* ex);
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

    bool operator==(Expression* other) const
    {
        Variable* v = dynamic_cast<Variable*>(other);
        if (v == nullptr)
            return false;
        else
            return (v->var == var);
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
    friend Expression* simplify(Expression* ex);
    Add(Expression* left, Expression* right): left(left), right(right)
    {}

    bool operator==(Expression* other) const;
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
    friend Expression* simplify(Expression* ex);
    Sub(Expression *left, Expression *right): left(left), right(right)
    {}

    bool operator==(Expression* other) const;
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
    friend Expression* simplify(Expression* ex);
    Mul(Expression *left, Expression *right): left(left), right(right)
    {}

    bool operator==(Expression* other) const;
    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
    ~Mul();
};

class Div: public Expression
{
protected:
    friend Expression* simplify(Expression* ex);
    Expression *left;
    Expression *right;
    friend class Add;
    friend class Sub;
    friend class Mul;
public:
    Div(Expression *left, Expression *right): left(left), right(right)
    {}

    bool operator==(Expression* other) const;
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

bool Add::operator==(Expression* other) const
{
    Add* a = dynamic_cast<Add*>(other);
    if (a == nullptr)
        return false;
    else
        return (a->left == left && a->right == right);
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

bool Sub::operator==(Expression* other) const
{
    Sub* s = dynamic_cast<Sub*>(other);
    if (s == nullptr)
        return false;
    else
        return (s->left == left && s->right == right);
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

bool Mul::operator==(Expression* other) const
{
    Mul* m = dynamic_cast<Mul*>(other);
    if (m == nullptr)
        return false;
    else
        return (m->left == left && m->right == right);
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

bool Div::operator==(Expression* other) const
{
    Div* d = dynamic_cast<Div*>(other);
    if (d == nullptr)
        return false;
    else
        return (d->left == left && d->right == right);
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

Expression* simplify(Expression* ex)
{
    Number* num = dynamic_cast<Number*>(ex);
    Variable* var = dynamic_cast<Variable*>(ex);
    if (num != nullptr || var != nullptr)
        return ex;
    Add* add_exp = dynamic_cast<Add*>(ex);
    if (add_exp != nullptr)
    {
        add_exp->left = simplify(add_exp->left);
        add_exp->right = simplify(add_exp->right);
        Number* l = dynamic_cast<Number*>(add_exp->left);
        Number* r = dynamic_cast<Number*>(add_exp->right);
        if (l != nullptr && r != nullptr)
            return new Number(ex->eval(""));
    }
    Sub* sub_exp = dynamic_cast<Sub*>(ex);
    if (sub_exp != nullptr)
    {
        sub_exp->left = simplify(sub_exp->left);
        sub_exp->right = simplify(sub_exp->right);
        Number* l = dynamic_cast<Number*>(sub_exp->left);
        Number* r = dynamic_cast<Number*>(sub_exp->right);
        if (l != nullptr && r != nullptr)
            return new Number(ex->eval(""));
    }
    Mul* mul_exp = dynamic_cast<Mul*>(ex);
    if (mul_exp != nullptr)
    {
        mul_exp->left = simplify(mul_exp->left);
        mul_exp->right = simplify(mul_exp->right);
        Number* l = dynamic_cast<Number*>(mul_exp->left);
        Number* r = dynamic_cast<Number*>(mul_exp->right);
        if (l != nullptr)
        {
            if (l->num == 0)
                return new Number(0);
            if (l->num == 1)
                return mul_exp->right;
        }
        if (r != nullptr)
        {
            if (r->num == 0)
                return new Number(0);
            if (r->num == 1)
                return mul_exp->right;
        }
        if (l != nullptr && r != nullptr)
            return new Number(ex->eval(""));
    }
    Div* div_exp = dynamic_cast<Div*>(ex);
    if (div_exp != nullptr)
    {
        div_exp->left = simplify(div_exp->left);
        div_exp->right = simplify(div_exp->right);
        Number* l = dynamic_cast<Number*>(div_exp->left);
        Number* r = dynamic_cast<Number*>(div_exp->right);
        if (l != nullptr && r != nullptr)
            return new Number(ex->eval(""));
    }
    return ex;    
}

int main()
{
    string str;
    std::cin >> str;
    Expression* e = read(str);
    Expression* de = simplify(e);

    //Expression* de = e->derivative("x");
    de->print(cout);
    return 0;
}
