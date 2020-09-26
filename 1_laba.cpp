#include <iostream>
#include <fstream>
using namespace std;

//класс для столбцов матрицы
class column
{
    int size;
    int **mtrx;
    unsigned col_index;
    public:
        column(): size(0), col_index(0), mtrx(nullptr)
        {}
        
        column(unsigned s, unsigned index, int **m): size(s), col_index(index), mtrx(m)
        {}

        int& operator[](unsigned indx) const
        {
            if (indx > size)
                throw;
            return mtrx[indx - 1][col_index];
        }
};

//класс для строк матрицы
class raw
{
    int size;
    int **mtrx;
    unsigned r_index;
    public:
        raw(): size(0), r_index(0), mtrx(nullptr)
        {}
        
        raw(unsigned s, unsigned index, int **m): size(s), r_index(index), mtrx(m)
        {}

        int& operator[](unsigned indx) const
        {
            if (indx > size)
                throw;
            return mtrx[r_index][indx - 1];
        }
};

class Matrix
{
    unsigned size;
    int **mtrx;
    public:
        Matrix(): size(0), mtrx(nullptr)
        {}

        Matrix(unsigned s)
        {
            size = s;
            mtrx = new int* [size];
            for (int i = 0; i < size; i++)
                mtrx[i] = new int [size];
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    if (i == j)
                        mtrx[i][j] = 1;
                    else
                        mtrx[i][j] = 0;     
        }

        Matrix(unsigned s, int* array)
        {
            size = s;
            mtrx = new int* [size];
            for (int i = 0; i < size; i++)
                mtrx[i] = new int [size];
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    if (i == j)
                        mtrx[i][j] = array[i];
                    else
                        mtrx[i][j] = 0;
                    
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
                mtrx = new int* [size];
                for (int i = 0; i < size; i++)
                    mtrx[i] = new int [size];
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
        Matrix operator!() const;
        Matrix operator()(unsigned n, unsigned m) const;
        raw operator[](unsigned n);
        column operator()(unsigned n);
        void scan_matrix(unsigned s);
        void scan_matrix_f(unsigned s, char* fname);
};
//функция для вывода матрицы на экран
void Matrix::print() const
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            cout << mtrx[i][j] << ' ';
        cout << '\n';
    }       
}

Matrix Matrix::operator+(const Matrix& other) const
{
    if (size!=other.size)
    {
        throw;
    }
    Matrix new_m;
    new_m.size = size;
    new_m.mtrx = new int* [size];
    for (int i = 0; i < size; i++)
        new_m.mtrx[i] = new int [size];
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            new_m.mtrx[i][j] = mtrx[i][j] + other.mtrx[i][j];
    return new_m;
}

Matrix Matrix::operator-(const Matrix& other) const
{
    if (size!=other.size)
    {
        throw;
    }
    Matrix new_m;
    new_m.size = size;
    new_m.mtrx = new int* [size];
    for (int i = 0; i < size; i++)
        new_m.mtrx[i] = new int [size];
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            new_m.mtrx[i][j] = mtrx[i][j] - other.mtrx[i][j];
    return new_m;
}

Matrix Matrix::operator*(const Matrix& other) const
{
    if (size!=other.size)
    {
        throw;
    }
    Matrix new_m;
    new_m.size = size;
    new_m.mtrx = new int* [size];
    for (int i = 0; i < size; i++)
        new_m.mtrx[i] = new int [size];
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
//оператор нахождения обратной матрицы
Matrix Matrix::operator!() const
{
    Matrix new_m;
    new_m.size = size;
    new_m.mtrx = new int* [size];
    for (int i = 0; i < size; i++)
        new_m.mtrx[i] = new int [size];
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
        throw;
    Matrix new_m;
    new_m.size = size - 1;
    new_m.mtrx = new int* [new_m.size];
    for (int i = 0; i < new_m.size; i++)
        new_m.mtrx[i] = new int [new_m.size];
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < m-1; j++)
            new_m.mtrx[i][j] = mtrx[i][j];
    for (int i = n; i < size; i++)
        for (int j = m; j < size; j++)
            new_m.mtrx[i-1][j-1] = mtrx[i][j];
    return new_m;
}
//считывание матрицы с консоли
void Matrix::scan_matrix(unsigned s)
{
    this->~Matrix();
    size = s;
    mtrx = new int* [size];
    for (int i = 0; i < size; i++)
        mtrx[i] = new int [size];
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            cin >> mtrx[i][j];
}
//считывание матрицы из файла
void Matrix::scan_matrix_f(unsigned s, char* fname)
{
    this->~Matrix();
    size = s;
    mtrx = new int* [size];
    for (int i = 0; i < size; i++)
        mtrx[i] = new int [size];
    ifstream fin(fname);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            fin >> mtrx[i][j];
    fin.close();
}
//обращение к строке матрицы
raw Matrix::operator[](unsigned n)
{
    if (n > size)
        throw;
    return {size, n - 1, mtrx};
}
//обращение к столбцу матрицы
column Matrix::operator()(unsigned n)
{
    if (n > size)
        throw;
    return {size, n - 1, mtrx};
}

int main()
{
     unsigned n;
    int k;
    cin >> n;
    cin >> k;
    int *k1 = new int [n];
    for (int i = 0; i < n; i++)
        k1[i] = k;
    Matrix km(n, k1);
    Matrix am, bm, cm, dm;
    am.scan_matrix(n);
    bm.scan_matrix(n);
    cm.scan_matrix(n);
    dm.scan_matrix(n);
    Matrix answer = (am + bm * (!cm) + km) * (!dm);
    answer.print();
    system("pause");
    return 0;
}