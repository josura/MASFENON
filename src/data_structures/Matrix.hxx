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
        /**
         * @brief Destructor for cleaning up the allocated memory.
         * @details Deallocates the memory used for the matrix.
         */
        ~Matrix();
        /**
         * @brief Assignment operator for copying a matrix.
         * @param m The matrix to copy from.
         * @details Allocates memory for the new matrix and copies the values from the provided matrix.
         * @details deletes the old matrix and allocates a new one with the same size as the new matrix.
         * @details Copies the values from the provided matrix to the new matrix.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         */
        Matrix& operator=(const Matrix&);
        /**
         * @brief Selector operator for accessing matrix elements.
         * @param x Row index.
         * @param y Column index.
         * @details Provides access to the element at the specified row and column.
         * @details Throws an exception if the indices are out of bounds.
         */
        inline T& operator()(int x, int y) { if(x>=0 && y>=0 && x < rows_ && y < cols_)return _matrix[x*cols_ +y]; else throw std::invalid_argument("Matrix::getValue: x or y not in range"); }
        /**
         * @brief Selector operator for accessing matrix elements (const version).
         * @param x Row index.
         * @param y Column index.
         * @details Provides access to the element at the specified row and column.
         * @details Throws an exception if the indices are out of bounds.
         * @return A reference to the element at the specified position.
         */
        inline T& getValue(int x, int y)const{ if(x>=0 && y>=0 && x < rows_ && y < cols_)return _matrix[x*cols_ +y]; else throw std::invalid_argument("Matrix::getValue: x or y not in range"); }
        /**
         * @brief Mutator operator for setting matrix elements.
         * @param x Row index.
         * @param y Column index.
         * @param value The value to set.
         * @details Sets the element at the specified row and column to the provided value.
         * @details Throws an exception if the indices are out of bounds.
         */
        inline void setValue(int x, int y, T value){ if(x>=0 && y>=0 && x < rows_ && y < cols_) _matrix[x*cols_ +y] = value; else throw std::invalid_argument("Matrix::setValue: x or y not in range"); }

        /**
         * @brief Assignment operator for adding a matrix to another matrix.
         * @param rhs The right-hand side matrix to add.
         * @details Allocates memory for the new matrix and adds the values from the provided matrix.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         */
        Matrix& operator+=(const Matrix&);
        /**
         * @brief Assignment operator for subtracting a matrix from another matrix.
         * @param rhs The right-hand side matrix to subtract.
         * @details Allocates memory for the new matrix and subtracts the values from the provided matrix.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         */
        Matrix& operator-=(const Matrix&);
        /**
         * @brief Assignment operator for multiplying a matrix with another matrix.
         * @param rhs The right-hand side matrix to multiply with.
         * @details Allocates memory for the new matrix and multiplies the values from the provided matrix.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         */
        Matrix& operator*=(const Matrix&);
        /**
         * @brief Assignment operator for multiplying a matrix with a scalar.
         * @param rhs The scalar value to multiply with.
         * @details Allocates memory for the new matrix and multiplies the values from the provided scalar.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         */
        Matrix& operator*=(T);
        /**
         * @brief Assignment operator for dividing a matrix by a scalar.
         * @param rhs The scalar value to divide by.
         * @details Allocates memory for the new matrix and divides the values from the provided scalar.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         */
        Matrix& operator/=(T);
        /**
         * @brief Addition operator for adding two matrices.
         * @param rhs The right-hand side matrix to add.
         * @details Allocates memory for the new matrix and adds the values from the provided matrix.
         * @warning This operator does not check for memory leaks.
         * @return A new matrix containing the result of the addition.
         */
        Matrix operator+(const Matrix<T>&)const;
        /**
         * @brief Subtraction operator for subtracting two matrices.
         * @param rhs The right-hand side matrix to subtract.
         * @details Allocates memory for the new matrix and subtracts the values from the provided matrix.
         * @warning This operator does not check for memory leaks.
         * @return A new matrix containing the result of the subtraction.
         */
        Matrix operator-(const Matrix<T>&)const;
        /**
         * @brief Multiplication operator for multiplying two matrices.
         * @param rhs The right-hand side matrix to multiply with.
         * @details Allocates memory for the new matrix and multiplies the values from the provided matrix.
         * @warning This operator does not check for memory leaks.
         * @return A new matrix containing the result of the multiplication.
         */
        Matrix operator*(const Matrix<T>&)const;
        /**
         * @brief Multiplication operator for multiplying a matrix with a vector.
         * @param rhs The right-hand side vector to multiply with.
         * @details Allocates memory for the new matrix and multiplies the values from the provided vector.
         * @warning This operator does not check for memory leaks.
         * @return A new matrix containing the result of the multiplication.
         */
        Matrix operator*(const std::vector<T>&)const;
        /**
         * @brief Multiplication operator for multiplying a matrix with a scalar.
         * @param rhs The scalar value to multiply with.
         * @details Allocates memory for the new matrix and multiplies the values from the provided scalar.
         * @warning This operator does not check for memory leaks.
         * @return A new matrix containing the result of the multiplication.
         */
        Matrix operator*(T)const;

        // friend functions
        /**
         * @brief Multiplication operator for multiplying a scalar with a matrix.
         * @param lhs The scalar value to multiply with.
         * @param rhs The right-hand side matrix to multiply with.
         * @details Allocates memory for the new matrix and multiplies the values from the provided scalar.
         */
        template<typename U>
        friend Matrix<U> operator*(U, const Matrix<U>&);
        /**
         * @brief Multiplication operator for multiplying a vector with a matrix.
         * @param lhs The left-hand side vector to multiply with.
         * @param rhs The right-hand side matrix to multiply with.
         * @returns A new vector containing the result of the multiplication.
         * @details Allocates memory for the new vector and multiplies the values from the provided vector.
         * @warning This operator does not check for memory leaks.
         */
        template<typename U>
        friend std::vector<U> operator*(U*,const Matrix<U>&);  //vector multiplication leftwise
        /**
         * @brief Multiplication operator for multiplying a vector with a matrix.
         * @param lhs The left-hand side vector to multiply with.
         * @param rhs The right-hand side matrix to multiply with.
         * @returns A new vector containing the result of the multiplication.
         * @details Allocates memory for the new vector and multiplies the values from the provided vector.
         */
        template<typename U>
        friend std::vector<U> operator*(std::vector<U>&,const Matrix<U>&);
        
        /**
         * @brief Output stream operator for printing a matrix.
         * @param os The output stream to print to.
         * @param m The matrix to print.
         * @details Prints the matrix in a readable format.
         */
        template<typename U>
        friend std::ostream& operator<<(std::ostream&, const Matrix<U>&);

        /**
         * @brief Function to swap two rows in the matrix.
         * @param i The index of the first row.
         * @param j The index of the second row.
         * @details Swaps the values of the two specified rows in the matrix.
         * @warning This function is not implemented yet.
         */
        void swapRows(int, int);
        /**
         * @brief Function to swap two columns in the matrix.
         * @param i The index of the first column.
         * @param j The index of the second column.
         * @details Swaps the values of the two specified columns in the matrix.
         */
        Matrix transpose()const;

        /**
         * @brief Static function to create an identity matrix of specified size.
         * @param size The size of the identity matrix.
         * @details Allocates memory for the new matrix and initializes it as an identity matrix.
         */
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