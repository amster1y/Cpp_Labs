#include <iostream>
#include <fstream>

class Matrix;
//класс для столбцов матрицы
class Column
{
    Matrix &m;
    const unsigned col_index;
public:       
    Column(unsigned index, Matrix &m1): col_index(index), m(m1)
    {}

    int& operator[](unsigned indx) const;
};

//класс для строк матрицы
class Raw
{
    Matrix &m;
    const unsigned r_index;
public:
    Raw(unsigned index, Matrix &m1): r_index(index), m(m1)
    {}

    int& operator[](unsigned indx) const;
};

class Matrix
{
    unsigned size;
    int **mtrx;
    friend class Column;
    friend class Raw;
    void allocation(unsigned s)
    {
        size = s;
        mtrx = new int* [size];
        for (int i = 0; i < size; i++)
            mtrx[i] = new int [size];
    }
public:
    Matrix(): size(0), mtrx(nullptr)
    {}

    Matrix(unsigned s)
    {
        allocation(s);
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                mtrx[i][j] = (i == j);   
    }

    Matrix(unsigned s, int* array)
    {
        allocation(s);
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                if (i == j)
                    mtrx[i][j] = array[i];
                else
                    mtrx[i][j] = 0;
                
    }

    Matrix(std::istream &type)
    {
        unsigned s;
        type >> s;
        allocation(s);
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                type >> mtrx[i][j];
    }

    ~Matrix()
    {
        for (int i = 0; i < size; i++)
            delete [] mtrx[i];
        delete [] mtrx;
    }

    Matrix& operator=(const Matrix& orig)
    {
        if (this != &orig)
        {
            this->~Matrix();
            allocation(orig.size);
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    mtrx[i][j] = orig.mtrx[i][j];
        }
        return *this;
    }

    Matrix(const Matrix& orig): size(orig.size)
    {
        *this = orig;
    }

    void print() const;
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;
    Matrix operator!() const;
    Matrix operator()(unsigned n, unsigned m) const;
    Raw operator[](unsigned n);
    Column operator()(unsigned n);
    friend std::istream &operator>>(std::istream &type, Matrix &m);
};
//функция для вывода матрицы на экран
void Matrix::print() const
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            std::cout << mtrx[i][j] << ' ';
        std::cout << std::endl;
    }       
}

Matrix Matrix::operator+(const Matrix& other) const
{
    if (size!=other.size)
    {
        throw "Размерности матриц не совпадают.";
    }
    Matrix new_m(size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            new_m.mtrx[i][j] = mtrx[i][j] + other.mtrx[i][j];
    return new_m;
}

Matrix Matrix::operator-(const Matrix& other) const
{
    if (size!=other.size)
    {
        throw "Размерности матриц не совпадают.";
    }
    Matrix new_m(size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            new_m.mtrx[i][j] = mtrx[i][j] - other.mtrx[i][j];
    return new_m;
}

Matrix Matrix::operator*(const Matrix& other) const
{
    if (size!=other.size)
    {
        throw "Размерности матриц не совпадают.";
    }
    Matrix new_m(size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            new_m.mtrx[i][j] = 0;
            for (int k = 0; k < size; k++)
                new_m.mtrx[i][j] += mtrx[i][k]*other.mtrx[k][j];
        }
    return new_m;
}

bool Matrix::operator==(const Matrix& other) const
{
    if (size!=other.size)
        return false;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (mtrx[i][j] != other.mtrx[i][j])
                return false;
    return true;
}

bool Matrix::operator!=(const Matrix& other) const
{
    return (*this==other)? false : true;
}

std::istream & operator>>(std::istream &type, Matrix &m)
{
    for (int i = 0; i < m.size; i++)
        for (int j = 0; j < m.size; j++)
            type >> m.mtrx[i][j];
    return type;
}
//оператор нахождения обратной матрицы
Matrix Matrix::operator!() const
{
    Matrix new_m(size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            new_m.mtrx[i][j] = mtrx[j][i];
        }
    return new_m;
}

Matrix Matrix::operator()(unsigned n, unsigned m) const
{
    if (n > size || m > size)
        throw "Индекс выходит за границы массива.";
    Matrix new_m(size - 1);
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < m-1; j++)
            new_m.mtrx[i][j] = mtrx[i][j];
    for (int i = n; i < size; i++)
        for (int j = m; j < size; j++)
            new_m.mtrx[i-1][j-1] = mtrx[i][j];
    return new_m;
}

//обращение к строке матрицы
Raw Matrix::operator[](unsigned n)
{
    if (n > size)
        throw "Индекс выходит за границы массива.";
    return {n - 1, *this};
}
//обращение к столбцу матрицы
Column Matrix::operator()(unsigned n)
{
    if (n > size)
        throw "Индекс выходит за границы массива.";
    return {n - 1, *this};
}

int& Column::operator[](unsigned indx) const
{
    if (indx > m.size)
        throw "Индекс выходит за границы массива.";
    return m.mtrx[indx - 1][col_index];
}

int& Raw::operator[](unsigned indx) const
{
    if (indx > m.size)
        throw "Индекс выходит за границы массива.";
    return m.mtrx[r_index][indx - 1];
}

int main()
{
    unsigned n;
    int k;
    std::cin >> n;
    std::cin >> k;
    int *k1 = new int [n];
    for (int i = 0; i < n; i++)
        k1[i] = k;
    Matrix km(n, k1);
    delete [] k1;
    Matrix am(n), bm(n), cm(n), dm(n);
    std::cin >> am;
    std::cin >> bm;
    std::cin >> cm;
    std::cin >> dm;
    try
    {
        Matrix answer = (am + bm * (!cm) + km) * (!dm);
        answer.print();
    }
    catch(const char* msg)
    {
        std::cout << "Error: " << msg << std::endl;
    }
    system("pause");
    return 0;
}
