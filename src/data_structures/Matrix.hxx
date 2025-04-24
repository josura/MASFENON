/**
 * @file Matrix.hxx
 * @ingroup Core
 * @brief Template class for representing a 2D matrix with row-major ordering.
 * @details Provides accessors, mutators, and operations for internal storage and manipulation of matrix elements.
 */
#pragma once

#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <armadillo>
#include <cmath>

/**
 * @class Matrix
 * @brief Generic 2D matrix implementation using flat row-major storage.
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class Matrix {
    public:
        /**
         * @brief Constructor for creating a matrix with specified dimensions.
         * @param rows Number of rows.
         * @param cols Number of columns.
         * @details Allocates memory for the matrix and initializes all elements to zero.
         */
        Matrix(int, int);
        /**
         * @brief Constructor for creating a matrix with specified dimensions and initial values.
         * @param a Pointer to a 2D array of type T.
         * @param rows Number of rows.
         * @param cols Number of columns.
         * @details Allocates memory for the matrix and initializes all elements with the provided values in the 2D array.
         * @note The caller is responsible for managing the memory of the 2D array.
         * @warning This constructor does not check for memory leaks.
         */
        Matrix(T**, int, int);
        /**
         * @brief Default constructor for creating a 1x1 matrix initialized to zero.
         * @details Allocates memory for the matrix and initializes the single element to zero.
         */
        Matrix();
        /**
         * @brief Copy constructor for creating a matrix from another matrix.
         * @param m The matrix to copy from.
         * @details Allocates memory for the new matrix and copies the values from the provided matrix.
         */
        Matrix(const Matrix&);
        /**
         * @brief Constructor for creating a matrix from a vector.
         * @param _wvector The vector to initialize the matrix with.
         * @param nrows Number of rows (default is 0).
         * @param ncols Number of columns (default is 1).
         * @details Allocates memory for the matrix and initializes the elements with the values from the vector.
         */
        Matrix(const std::vector<T>& _wvector, uint nrows=0, uint ncols=1);
        ~Matrix();
        Matrix& operator=(const Matrix&);

        inline T& operator()(int x, int y) { if(x>=0 && y>=0 && x < rows_ && y < cols_)return _matrix[x*cols_ +y]; else throw std::invalid_argument("Matrix::getValue: x or y not in range"); }
        inline T& getValue(int x, int y)const{ if(x>=0 && y>=0 && x < rows_ && y < cols_)return _matrix[x*cols_ +y]; else throw std::invalid_argument("Matrix::getValue: x or y not in range"); }
        inline void setValue(int x, int y, T value){ if(x>=0 && y>=0 && x < rows_ && y < cols_) _matrix[x*cols_ +y] = value; else throw std::invalid_argument("Matrix::setValue: x or y not in range"); }

        Matrix& operator+=(const Matrix&);
        Matrix& operator-=(const Matrix&);
        Matrix& operator*=(const Matrix&);
        Matrix& operator*=(T);
        Matrix& operator/=(T);
        Matrix operator^(int);  //integer power
        Matrix operator+(const Matrix<T>&)const;
        Matrix operator-(const Matrix<T>&)const;
        Matrix operator*(const Matrix<T>&)const;
        Matrix operator*(const std::vector<T>&)const;
        Matrix operator*(T)const;
        template<typename U>
        friend Matrix<U> operator*(double, const Matrix<U>&);
        template<typename U>
        friend Matrix<U> operator*(U*,const Matrix<U>&);  //vector multiplication leftwise
        template<typename U>
        friend Matrix<U> operator*(std::vector<U>&,const Matrix<U>&);
        Matrix operator/(double);
        
        template<typename U>
        friend std::ostream& operator<<(std::ostream&, const Matrix<U>&);
        template<typename U>
        friend std::istream& operator>>(std::istream&, Matrix<U>&);

        void swapRows(int, int);
        Matrix transpose()const;

        static Matrix createIdentity(int);
        static Matrix createRandom(int,int);
        static Matrix solve(Matrix, Matrix);
        static Matrix bandSolve(Matrix, Matrix, int);

        // functions on vectors
        static double dotProduct(Matrix, Matrix);
        static Matrix getMinor(const Matrix<T>&,int, int,int);
        static T determinant(const Matrix<T>& A);
        Matrix& operator*=(const std::vector<T>&);
        /*
        * \brief   divide the values in column i by the value in 
        *           vector at the index i
        */
        Matrix& normalizeByVectorColumn(const std::vector<double>& normVector);
        /*
        * \brief   divide the values in row i by the value in 
        *           vector at the index i
        */
        Matrix& normalizeByVectorRow(const std::vector<double>& normVector);
        // functions for reduction and inverse
        Matrix concatenateRight(const Matrix&)const;
        T determinant()const;
        Matrix gaussianElimination();
        Matrix rowReduceGaussian();
        Matrix inverse();  // to implement

        //get functions
        int getRows()const{return rows_;}
        int getCols()const{return cols_;}
        bool isVector()const{return (rows_ >= 1 && cols_ == 1); }
        std::vector<T> asVector()const; 

        //functions to add rows and columns while mantaining the original data in the upperleft corner(these functions are bad, better use a vector when trying to work with dynamically instantiated data)
        // also these functions create a copy and do not work on the original
        Matrix copyAndAddRowsColsWithZeros(int additionalRows, int additionalCols) const;

        /**
         * \brief   add a row to the matrix and return a new matrix
         * \param   row :the row to add
         * \param   position : the position where to add the row
         * \return  the pointer to new matrix with the added row
         */
        Matrix* addRowNew(const std::vector<T>& row, int position);

        /**
         * \brief   add a column to the matrix and return a new matrix
         * \param   column :the column to add
         * \param   position : the position where to add the column
         * \return  the pointer to new matrix with the added column
         */
        Matrix* addColumnNew(const std::vector<T>& column, int position);

        /**
         * \brief  add a row to the matrix
         * \param  row : the row to add
         * \param  position : the position where to add the row
         * \return nothing
         */
        void addRow(const std::vector<T>& row, int position);

        /**
         * \brief  add a column to the matrix
         * \param  column : the column to add
         * \param  position : the position where to add the column
         * \return nothing
         */
        void addColumn(const std::vector<T>& column, int position);

        /**
         * \brief  add a row to the matrix at the end
         * \param  row : the row to add
         * \return nothing
         */
        void addRowAtTheEnd(const std::vector<T>& row);

        /**
         * \brief  add a column to the matrix at the end
         * \param  column : the column to add
         * \return nothing
         */
        void addColumnAtTheEnd(const std::vector<T>& column);



        //functions to convert to armadillo
        arma::Mat<T> asArmadilloMatrix()const;
        arma::Col<T> asArmadilloColumnVector()const;
        arma::Row<T> asArmadilloRowVector()const;
        void printMatrix()const;


    protected:
        int rows_, cols_;
        T *_matrix=nullptr;

        void allocateMatrixSpace();
        Matrix expHelper(const Matrix&, int);
};