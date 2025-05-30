/**
 * @file Matrix.cxx
 * @ingroup Core
 * @brief Implementation of the Matrix class template.
 * @details This file contains the implementation of the Matrix class template, which provides a 2D matrix representation with row-major ordering.
 */
#include "data_structures/Matrix.hxx"
#include "utils/mathUtilities.hxx"
#include <iostream>
#include <stdexcept>
#include <vector>


// helper functions
template<typename T>
void Matrix<T>::allocateMatrixSpace()
{
    if(_matrix) {
        delete[] _matrix;
        _matrix=nullptr;
    }
    int totalLength = rows_ * cols_;
    _matrix = new T[totalLength]; //columns first
    
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            _matrix[i*cols_ + j] = 0;
        }    
    }
}

template void Matrix<double>::allocateMatrixSpace();


//constructors and destructors
template<typename T>
Matrix<T>::Matrix(int rows, int cols) : rows_(rows), cols_(cols)
{
    allocateMatrixSpace();
}

template Matrix<double>::Matrix(int rows, int cols);

template<typename T>
Matrix<T>::Matrix(T** a, int rows, int cols) : rows_(rows), cols_(cols)
{
    allocateMatrixSpace();
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            _matrix[i*cols_ + j] = a[i][j];
        }
    }
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix constructor with double (with 2D array)
 */
template Matrix<double>::Matrix(double** a, int rows, int cols);

template<typename T>
Matrix<T>::Matrix() : rows_(1), cols_(1)
{
    allocateMatrixSpace();
    _matrix[0] = 0;
}

template Matrix<double>::Matrix();

template<typename T>
Matrix<T>::Matrix(const std::vector<T>& vec, uint nrows, uint ncols):rows_(vec.size()),cols_(ncols){
    bool isAvector = false;
    if((nrows == vec.size() || nrows == 0) && ncols == 1){
        isAvector=true;
    }
    else if( (nrows != vec.size() && (approximatelyEqual(vec.size()/(double)nrows, (double)ncols, std::numeric_limits<T>::epsilon())))){
        rows_ = nrows;
    } else {
        Logger::getInstance().printError("Matrix<T>::Matrix(vec,nrows=0,ncols=1): the number of resulting columns from the division of rows is not equal to the one passed in the ncols parameter: vec.size=" + std::to_string(vec.size()) + " nrows=" + std::to_string(nrows) + " ncols=" + std::to_string(ncols)); 
        throw std::invalid_argument("[ERROR] Matrix<T>::Matrix(vec,nrows=0,ncols=1): the number of resulting columns from the division of rows is not equal to the one passed in the ncols parameter");
    }
    allocateMatrixSpace();
    if(isAvector){
        for (int i = 0; i < rows_; i++) {
            for (int j = 0; j < cols_;j++) {
                int index = i * cols_ + j; 
                _matrix[index] = vec[index];
            }
        }
    }else {
        for (int i = 0; i < rows_; i++) {
            for (int j = 0; j < cols_;j++) {
                int index = i * cols_ + j; 
                _matrix[index] = vec[index];
            }
        }
    }
    
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix constructor with double (with vector)
 */
template Matrix<double>::Matrix(const std::vector<double>& vec,uint nrows, uint ncols);


template<typename T>
Matrix<T>::~Matrix()
{
    if(_matrix) {
        delete[] _matrix;
        _matrix=nullptr;
    }
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix destructor with double 
 */
template Matrix<double>::~Matrix();

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& m) : rows_(m.rows_), cols_(m.cols_)
{
    allocateMatrixSpace();
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            _matrix[i*cols_ + j] = m.getValue(i,j);
        }
    }
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix copy constructor with double 
 */
template Matrix<double>::Matrix(const Matrix<double>& m);

//static methods
// generate random matrix, values ranging from -DOUBLE_MAX to DOUBLE_MAX
template<typename T>
Matrix<T> Matrix<T>::createRandom(int rows,int cols){
    Matrix<T> retMat=Matrix<T>(rows,cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            setRandom(retMat(i,j));
        }
    }
    return retMat;
}

template Matrix<double> Matrix<double>::createRandom(int rows,int cols);

template<typename T>
Matrix<T> Matrix<T>::createIdentity(int size)
{
    Matrix<T> temp(size, size);
    for (int i = 0; i < temp.rows_; ++i) {
        for (int j = 0; j < temp.cols_; ++j) {
            if (i == j) {
                temp(i,j) = 1;
            } else {
                temp(i,j) = 0;
            }
        }
    }
    return temp;
}
template Matrix<double> Matrix<double>::createIdentity(int size);

//operators
template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& m)
{
    if (this == &m) {
        return *this;
    }

    if (rows_ != m.rows_ || cols_ != m.cols_) {
        if(_matrix){
            delete[] _matrix;
            _matrix = nullptr;
        }

        rows_ = m.rows_;
        cols_ = m.cols_;
        allocateMatrixSpace();
    }

    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            _matrix[i*cols_ + j] = m.getValue(i,j);
        }
    }
    return *this;
}

template Matrix<double>& Matrix<double>::operator=(const Matrix<double>& m);

template<typename T>
Matrix<T>& Matrix<T>::operator*=(Matrix<T> const& rhs) {
    if(getCols() == rhs.getRows()){
        Matrix<T> tmp(rows_, rhs.cols_);
        for (int i = 0; i < tmp.rows_; ++i) {
            for (int j = 0; j < tmp.cols_; ++j) {
                for (int k = 0; k < cols_; ++k) {
                    tmp(i,j) += (getValue(i, k) * rhs.getValue(k,j));
                }
            }
        }
        return (*this = tmp);
    } else {
        throw std::invalid_argument("column dimension of lhs for operation *= is not equal to row dimension of lhs\n");
    }
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix operator*= with double 
 */
template Matrix<double>& Matrix<double>::operator*=(const Matrix<double>& m);

template<typename T>
Matrix<T>& Matrix<T>::operator*=(T rhs) {
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            _matrix[i * cols_ + j] *= rhs;
        }
    }
    return *this;
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix operator*= with double 
 */
template Matrix<double>& Matrix<double>::operator*=(double m);


template<typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& m)
{
    if(getCols() == m.getCols() && getRows() == m.getRows()){
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                _matrix[i * cols_ + j] += m.getValue(i,j);
            }
        }
        return *this;
    } else {
        throw std::invalid_argument("Matrix dimensions are not the same when applying addition\n");
    }
}

template Matrix<double>& Matrix<double>::operator+=(const Matrix<double>& m);


template<typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& m)
{
    if(getCols() == m.getCols() && getRows() == m.getRows()){
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                _matrix[i * cols_ + j] -= m.getValue(i,j);
            }
        }
        return *this;
    } else {
        throw std::invalid_argument("column dimension of lhs for operation -= is not equal to row dimension of lhs\n");
    }
}

template Matrix<double>& Matrix<double>::operator-=(const Matrix<double>& m);

template<typename T>
Matrix<T>& Matrix<T>::operator/=(T num)
{
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            _matrix[i * cols_ + j] /= num;
        }
    }
    return *this;
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix operator/= with double 
 */
template Matrix<double>& Matrix<double>::operator/=(double m);


template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& rhs)const{
    Matrix<T> result(*this);
    return result *= rhs;
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix operator* with double 
 */
template Matrix<double> Matrix<double>::operator*(const Matrix<double>& rhs)const;

template<typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& rhs)const{
    Matrix<T> result(*this);
    return result -= rhs;
}

template Matrix<double> Matrix<double>::operator-(const Matrix<double>& rhs)const;

template<typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& rhs)const{
    Matrix<T> result(*this);
    return result += rhs;
}

template Matrix<double> Matrix<double>::operator+(const Matrix<double>& rhs)const;


template<typename T>
Matrix<T> Matrix<T>::operator*(const std::vector<T>& rhs)const{
    if (getCols()==SizeToInt(rhs.size())) {
        // Matrix<T> retvec = Matrix<T>(rows_,1);
        // int m = getRows(), n = getCols();
        // for (int i = 0; i < m; i++) {
        //     for (int k = 0; k < n; k++) {
        //         retvec(i,0) += getValue(i, k) * rhs[k];
        //     }
        // }
        Matrix<T> retvec = Matrix<T>(rhs);
        return (*this)*retvec;
    } else throw std::invalid_argument("arrayMult vector has not the right size to be multiplied");
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix operator* with double (with vector)
 */
template Matrix<double> Matrix<double>::operator*(const std::vector<double>& rhs)const;



template<typename T>
Matrix<T> Matrix<T>::transpose()const
{
    Matrix ret(cols_, rows_);
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            ret(j,i) = getValue(i, j);
        }
    }
    return ret;
}

template Matrix<double> Matrix<double>::transpose()const;


template<typename T>
Matrix<T> Matrix<T>::copyAndAddRowsColsWithZeros(int additionalRows, int additionalCols)const{
    Matrix<T> ret = Matrix<T>(this->getRows()+additionalRows,this->getCols()+additionalCols);
    for (int i = 0 ; i<this->getRows(); i++) {
        for (int j = 0 ; j<this->getRows(); j++) {
            ret(i,j) = getValue(i, j);
        }
    }
    for( int i = this->getRows(); i<this->getRows()+additionalRows; i++){
        for (int j = 0 ; j<this->getCols(); j++) {
            ret(i,j) = 0;
        }
    }
    for( int i = 0; i<this->getRows(); i++){
        for (int j = this->getCols(); j<this->getCols()+additionalCols; j++) {
            ret(i,j) = 0;
        }
    }

    return ret;
}

template Matrix<double> Matrix<double>::copyAndAddRowsColsWithZeros(int additionalRows, int additionalCols)const;


template<typename T>
Matrix<T>* Matrix<T>::addRowNew(const std::vector<T>& row, int position){
    if(position >= 0 && position <= rows_){
        Matrix<T>* ret = new Matrix<T>(rows_+1,cols_);
        for (int i = 0; i<position; i++) {
            for (int j = 0; j<cols_; j++) {
                ret->setValue(i,j,getValue(i, j));
            }
        }
        for (int j = 0; j<cols_; j++) {
            ret->setValue(position,j,row[j]);
        }
        for (int i = position+1; i<rows_+1; i++) {
            for (int j = 0; j<cols_; j++) {
                ret->setValue(i,j,getValue(i-1, j));
            }
        }
        return ret;
    } else {
        throw std::invalid_argument("Matrix::addRowNew : position is not in the range of the rows");
    }
}

template Matrix<double>* Matrix<double>::addRowNew(const std::vector<double>& row, int position);

template<typename T>
Matrix<T>* Matrix<T>::addColumnNew(const std::vector<T>& column, int position){
    if(position >= 0 && position <= cols_){
        Matrix<T>* ret = new Matrix<T>(rows_,cols_+1);
        for (int i = 0; i<rows_; i++) {
            for (int j = 0; j<position; j++) {
                ret->setValue(i,j,getValue(i, j));
            }
            ret->setValue(i,position,column[i]);
            for (int j = position+1; j<cols_+1; j++) {
                ret->setValue(i,j,getValue(i, j-1));
            }
        }
        return ret;
    } else {
        throw std::invalid_argument("Matrix::addColumnNew : position is not in the range of the columns");
    }
}

template Matrix<double>* Matrix<double>::addColumnNew(const std::vector<double>& column, int position);

template<typename T>
void Matrix<T>::addRow(const std::vector<T>& row, int position){
    if(position >= 0 && position <= rows_){
        T* old_matrix = _matrix;
        int totalLength = (rows_+1) * cols_;
        _matrix = new T[totalLength];
        rows_++;
        for (int i = 0; i<position; i++) {
            for (int j = 0; j<cols_; j++) {
                this->setValue(i,j,old_matrix[i*cols_ + j]);
            }
        }
        for (int j = 0; j<cols_; j++) {
            this->setValue(position,j,row[j]);
        }
        for (int i = position+1; i<rows_; i++) {
            for (int j = 0; j<cols_; j++) {
                this->setValue(i,j,old_matrix[(i-1)*cols_ + j]);
            }
        }
        delete[] old_matrix;
    } else {
        throw std::invalid_argument("Matrix::addRow : position is not in the range of the rows");
    }
}

template void Matrix<double>::addRow(const std::vector<double>& row, int position);

template<typename T>
void Matrix<T>::addColumn(const std::vector<T>& column, int position){
    if(position >= 0 && position <= cols_){
        T* old_matrix = _matrix;
        int totalLength = rows_ * (cols_+1);
        _matrix = new T[totalLength];
        cols_++;
        for (int i = 0; i<rows_; i++) {
            for (int j = 0; j<position; j++) {
                this->setValue(i,j,old_matrix[i*(cols_-1) + j]);
            }
            _matrix[i*cols_ + position] = column[i];
            for (int j = position+1; j<cols_; j++) {
                this->setValue(i,j,old_matrix[i*(cols_-1) + j-1]);
            }
        }
        delete[] old_matrix;
    } else {
        throw std::invalid_argument("Matrix::addColumn : position is not in the range of the columns");
    }
}

template void Matrix<double>::addColumn(const std::vector<double>& column, int position);

template<typename T>
void Matrix<T>::addRowAtTheEnd(const std::vector<T>& row){
    addRow(row,rows_);
}

template void Matrix<double>::addRowAtTheEnd(const std::vector<double>& row);

template<typename T>
void Matrix<T>::addColumnAtTheEnd(const std::vector<T>& column){
    addColumn(column,cols_);
}

template void Matrix<double>::addColumnAtTheEnd(const std::vector<double>& column);

template<typename T>
std::vector<T> Matrix<T>::asVector()const{
    if(this->isVector()){
        std::vector<T> ret(rows_,0);
        for (int i = 0; i<rows_; i++) {
            ret[i]=getValue(i,0);
        }
        return ret;
    } else {
        std::string empty = "";
        Logger::getInstance().printError(empty + "Matrix::asVector: the matrix is not a vector (1 column, n rows)" + "rows_=" + std::to_string(rows_) + " cols_=" + std::to_string(cols_));
        throw std::domain_error("[ERROR] Matrix::asVector: the matrix is not a vector (1 column, n rows)");
    }
}

template std::vector<double> Matrix<double>::asVector()const;


template<typename T>
Matrix<T> Matrix<T>::concatenateRight(const Matrix<T>& rhs)const{
    if(rhs.getRows()==rows_){
        Matrix<T> ret = this->copyAndAddRowsColsWithZeros(0, rhs.cols_);
        for (int i = 0; i<rows_; i++) {
            for (int j = 0; j<rhs.getCols(); j++) {
                ret(i,j+cols_) = rhs.getValue(i,j);    
            }
            
        }
        return ret;
    } else {
        throw std::invalid_argument("[ERROR] Matrix::concatenateRight: rhs is not of the same size(rows)");
    }
}


template Matrix<double> Matrix<double>::concatenateRight(const Matrix<double>& rhs)const;


//minor
template<typename T>
Matrix<T> Matrix<T>::getMinor(const Matrix<T>& A,int p, int q,int n) {
    int i = 0, j = 0;
    Matrix minor(n-1,n-1);
    for (int row = 0; row < n; row++){
        for (int col = 0; col<n; col++){
            if (row != p && col != q){
                minor(i,j++) = A.getValue(row,col);
                if (j == n - 1){
                    j = 0;
                    i++;
                }
            }
        }
    }
    return minor;
}

template Matrix<double> Matrix<double>::getMinor(const Matrix<double>& A,int p, int q,int n);

//determinant
template<typename T>
T Matrix<T>::determinant(const Matrix<T>& A){
    T deter=0;
    int n = A.getRows();
    switch (n) {
        case (1):
            return A.getValue(0, 0);
        case (2):
            return((A.getValue(0,0)*A.getValue(1,1))-(A.getValue(0,1)*A.getValue(1,0))); 
        case (3):
            return((A.getValue(0,0)*A.getValue(1,1)*A.getValue(2,2))+(A.getValue(1,0)*A.getValue(1,2)*A.getValue(2,0))+(A.getValue(1,0)*A.getValue(2,1)*A.getValue(0,2))-(A.getValue(0,2)*A.getValue(1,1)*A.getValue(2,0))-(A.getValue(0,1)*A.getValue(1,0)*A.getValue(2,2))-(A.getValue(1,2)*A.getValue(2,1)*A.getValue(0,0)));
    }
    Matrix tempCofactor(n,0.0); 
    int sign = 1;  // To store sign multiplier 
     // first row fixed
    for (int f = 0; f < n; f++){
        tempCofactor=getMinor(A,0, f, n);
        deter += sign * A.getValue(0,f) * determinant(tempCofactor);         //cofactor =sign * A[0][f] * determinant(tempCofactor, n - 1)
        sign = -sign;
    } 
    return deter;
}

/**
 * @relates Matrix
 * @brief force instantiation of Matrix determinant with double 
 */
template double Matrix<double>::determinant(const Matrix<double>& A);

template<typename T>
T Matrix<T>::determinant()const{
    T deter=0;
    switch (rows_) {
        case (1):
            return getValue(0, 0);
        case (2):
            return((getValue(0, 0)*getValue(1, 1))-(getValue(0, 1)*getValue(1, 0))); 
        case (3):
            return((getValue(0, 0)*getValue(1, 1)*getValue(2, 2))+(getValue(1, 0)*getValue(1, 2)*getValue(2, 0))+(getValue(1, 0)*getValue(2, 1)*getValue(0, 2))-(getValue(0, 2)*getValue(1, 1)*getValue(2, 0))-(getValue(0, 1)*getValue(1, 0)*getValue(2, 2))-(getValue(1, 2)*getValue(2, 1)*getValue(0, 0)));
    }
    Matrix tempCofactor(rows_,rows_); 
    int sign = 1;  // To store sign multiplier 
     // first row fixed
    for (int f = 0; f < rows_; f++){
    	Matrix minor=*this;
        tempCofactor=getMinor(minor, 0, f, rows_);
        deter += sign * getValue(0, f) * determinant(tempCofactor);         
        sign = -sign;
    } 
    return deter;
}

template double Matrix<double>::determinant()const;

template<typename T>
Matrix<T> Matrix<T>::inverse(){
    auto ret = Matrix<T>();
    //TODO choose between Gaussian elimination method and adjugate method, also controls on determinant after reduction to triangular form?
    return ret;
}

template Matrix<double> Matrix<double>::inverse();

template<typename T>
arma::Mat<T> Matrix<T>::asArmadilloMatrix()const{
    arma::Mat<T> ret = arma::Mat<T>(getRows(),getCols());  //filled with zeros
    for (int i =0 ; i<getRows(); i++) {
        for (int j =0 ; j<getCols(); j++) {
            ret(i,j) = getValue(i, j);
        }
    }
    return ret;
    //return arma::Mat<T>(_matrix,getRows(),getCols());
}

template arma::Mat<double> Matrix<double>::asArmadilloMatrix()const;


template<typename T>
arma::Col<T> Matrix<T>::asArmadilloColumnVector()const{
    return arma::Col<T>(asVector());
}

template arma::Col<double> Matrix<double>::asArmadilloColumnVector()const;


template<typename T>
arma::Row<T> Matrix<T>::asArmadilloRowVector()const{
    return arma::Row<T>(asVector());
}

template arma::Row<double> Matrix<double>::asArmadilloRowVector()const;


template<typename T>
Matrix<T>& Matrix<T>::normalizeByVectorColumn(const std::vector<double>& normVector){    
    if (SizeToInt( normVector.size())< cols_) {
        throw std::invalid_argument("[ERROR] Matrix<T>::normalizeByVectorColumn: normalization vector size is less than the number of columns");
    }
    for (int i =0 ; i<getRows(); i++) {
        for (int j =0 ; j<getCols(); j++) {
            _matrix[i * cols_ + j] = getValue(i, j) / (normVector[j] + 1e-20);
        }
    }
    return *this;
}

template Matrix<double>& Matrix<double>::normalizeByVectorColumn(const std::vector<double> &normVector);

template<typename T>
Matrix<T>& Matrix<T>::normalizeByVectorRow(const std::vector<double>& normVector){
    if (SizeToInt( normVector.size())< rows_) {
        throw std::invalid_argument("[ERROR] Matrix<T>::normalizeByVectorRow: normalization vector size is less than the number of rows");
    }
    for (int i =0 ; i<getRows(); i++) {
        for (int j =0 ; j<getCols(); j++) {
            _matrix[i * cols_ + j] = getValue(i, j) / (normVector[i] + 1e-20) ;
        }
    }
    return *this;
}

template Matrix<double>& Matrix<double>::normalizeByVectorRow(const std::vector<double> &normVector);

template<typename T>
void Matrix<T>::printMatrix()const{
    std::cout << "Matrix " << rows_ << "x" << cols_ << std::endl;
    for (int i =0 ; i<getRows(); i++) {
        for (int j =0 ; j<getCols(); j++) {
            std::cout << getValue(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

template void Matrix<double>::printMatrix()const;

//friend functions
template<typename U>
Matrix<U> operator*(const U lhs, const Matrix<U>& rhs){
    Matrix<U> ret = Matrix<U>(rhs.getRows(), rhs.getCols());
    for (int i = 0; i < rhs.getRows(); ++i) {
        for (int j = 0; j < rhs.getCols(); ++j) {
            ret(i,j) = lhs * rhs.getValue(i,j);
        }
    }
    return ret;
}
template Matrix<double> operator*(const double lhs, const Matrix<double>& rhs);

template<typename U>
std::vector<U> operator*(U* lhs, const Matrix<U>& rhs){  //vector multiplication leftwise, very unstable
    std::vector<U> result(rhs.getCols(),0);
    for (int i = 0; i < rhs.getRows(); ++i) {
        for (int k = 0; k < rhs.getCols(); ++k) {
            result[i] += lhs[k] * rhs.getValue(k,i);
        }
    }
    return result;
}
template std::vector<double> operator*(double* lhs, const Matrix<double>& rhs);  //vector multiplication leftwise

template<typename U>
std::vector<U> operator*(std::vector<U>& lhs, const Matrix<U>& rhs){  //vector multiplication leftwise
    if(SizeToInt(lhs.size()) != rhs.getCols()){
        throw std::invalid_argument("vector multiplication leftwise: lhs size is not equal to rhs cols");
    }
    std::vector<U> result(rhs.getCols(),0);
    for (int i = 0; i < rhs.getRows(); ++i) {
        for (int k = 0; k < rhs.getCols(); ++k) {
            result[i] += lhs[k] * rhs.getValue(k,i);
        }
    }
    return result;
}
template std::vector<double> operator*(std::vector<double>& lhs, const Matrix<double>& rhs);  //vector multiplication leftwise

template<typename U>
std::ostream& operator<<(std::ostream& os, const Matrix<U>& m)
{
    os << "Matrix " << m.getRows() << "x" << m.getCols() << std::endl;
    for (int i = 0; i < m.getRows(); ++i) {
        for (int j = 0; j < m.getCols(); ++j) {
            os << m.getValue(i,j) << " ";
        }
        os << std::endl;
    }
    return os;
}
template std::ostream& operator<<(std::ostream& os, const Matrix<double>& m);