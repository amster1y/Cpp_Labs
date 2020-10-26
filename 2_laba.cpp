#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Expression
{;
public:
    friend Expression* read(string str);
    friend Expression* simplify(Expression* ex);
    virtual Expression* copy() = 0;
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

    Expression* copy()
    {
        return new Number(num);
    }

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

    Expression* copy()
    {
        return new Variable(var);
    }

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

class Expression_of_2: public Expression
{
protected:
    Expression* left;
    Expression* right;
    Expression_of_2(Expression* left, Expression* right): left(left), right(right)
    {}
public:
    ~Expression_of_2()
    {
        delete left;
        delete right;
    }

    bool operator==(Expression* other) const
    {
        if (typeid(other) != typeid(this))
            return false;
        Expression_of_2* m = dynamic_cast<Expression_of_2*>(other);
        return (*(m->left) == left && *(m->right) == right);
    }
};

class Add: public Expression_of_2
{
    friend Expression* read(string str);
    Add(Expression* left, Expression* right): Expression_of_2(left, right)
    {}
protected:
    friend class Number;
    friend class Variable;
    friend class Sub;
    friend class Mul;
    friend class Div;
public:
    Expression* copy()
    {
        return new Add(left->copy(), right->copy());
    }
    friend Expression* simplify(Expression* ex);
    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
};

class Sub: public Expression_of_2
{
    friend Expression* read(string str);
    Sub(Expression* left, Expression* right): Expression_of_2(left, right)
    {}
protected:
    friend class Number;
    friend class Variable;
    friend class Add;
    friend class Mul;
    friend class Div;
public:
Expression* copy()
    {
        return new Sub(left->copy(), right->copy());
    }
    friend Expression* simplify(Expression* ex);
    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
};

class Mul: public Expression_of_2
{
    friend Expression* read(string str);
    Mul(Expression* left, Expression* right): Expression_of_2(left, right)
    {}
protected:
    friend class Number;
    friend class Variable;
    friend class Add;
    friend class Sub;
    friend class Div;
public:
    Expression* copy()
    {
        return new Mul(left->copy(), right->copy());
    }
    friend Expression* simplify(Expression* ex);
    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
};

class Div: public Expression_of_2
{
    friend Expression* read(string str);
    Div(Expression* left, Expression* right): Expression_of_2(left, right)
    {}
protected:
    friend class Number;
    friend class Variable;
    friend class Add;
    friend class Sub;
    friend class Mul;
public:
    Expression* copy()
    {
        return new Div(left->copy(), right->copy());
    }
    friend Expression* simplify(Expression* ex);
    ostream & print(ostream &type) const;
    int eval(string str) const;
    Expression* derivative(string v) const;
};

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
    return new Add(left->copy()->derivative(v), right->copy()->derivative(v));
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
    return new Sub(left->copy()->derivative(v), right->copy()->derivative(v));
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
    return new Add(new Mul(left->copy()->derivative(v), right->copy()), new Mul(left->copy(), right->copy()->derivative(v)));
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
    return new Div(new Sub(new Mul(left->copy()->derivative(v), right->copy()), new Mul(left->copy(), right->copy()->derivative(v))), 
    new Mul(right, right));
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
        if (sub_exp->left == sub_exp->right)
            return new Number(0);
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
    Expression* de = e->derivative("x");
    de->print(cout);
    delete e;
    delete de;
    return 0;
}
