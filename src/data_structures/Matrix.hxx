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
         * @throw std::invalid_argument if the vector size does not match the specified dimensions.
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
         * @throw std::invalid_argument if the dimensions of the matrices are not compatible for addition.
         */
        Matrix& operator+=(const Matrix&);
        /**
         * @brief Assignment operator for subtracting a matrix from another matrix.
         * @param rhs The right-hand side matrix to subtract.
         * @details Allocates memory for the new matrix and subtracts the values from the provided matrix.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         * @throw std::invalid_argument if the dimensions of the matrices are not compatible for subtraction.
         */
        Matrix& operator-=(const Matrix&);
        /**
         * @brief Assignment operator for multiplying a matrix with another matrix.
         * @param rhs The right-hand side matrix to multiply with.
         * @details Allocates memory for the new matrix and multiplies the values from the provided matrix.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         * @throw std::invalid_argument if the dimensions of the matrices are not compatible for multiplication.
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
        /**
         * @brief Static function to create a random matrix of specified size.
         * @param rows The number of rows.
         * @param cols The number of columns.
         * @details Allocates memory for the new matrix and initializes it with random values.
         */
        static Matrix createRandom(int,int);
        /**
         * @brief Static function to create a zero matrix of specified size.
         * @param rows The number of rows.
         * @param cols The number of columns.
         * @details Allocates memory for the new matrix and initializes all elements to zero.
         * @warning This function is not implemented yet.
         */
        static Matrix solve(Matrix, Matrix);
        /**
         * @brief Static function to create a banded matrix of specified size.
         * @param rows The number of rows.
         * @param cols The number of columns.
         * @param bandWidth The width of the band.
         * @details Allocates memory for the new matrix and initializes it as a banded matrix.
         * @warning This function is not implemented yet.
         */
        static Matrix bandSolve(Matrix, Matrix, int);

        // functions on vectors
        /**
         * @brief Function to calculate the dot product of two matrices.
         * @param A The first matrix.
         * @param B The second matrix.
         * @details Allocates memory for the new matrix and calculates the dot product of the two matrices.
         * @warning This function is not implemented yet.
         */
        static double dotProduct(Matrix, Matrix);
        /**
         * @brief Function to the the minor of a matrix.
         * @param matrix The matrix to get the minor from.
         * @param i The row index to exclude.
         * @param j The column index to exclude.
         * @param n The size of the matrix.
         * @details Allocates memory for the new matrix and calculates the minor of the specified matrix.
         */
        static Matrix getMinor(const Matrix<T>&,int, int,int);
        /**
         * @brief Static function to calculate the determinant of a matrix.
         * @param A The matrix to calculate the determinant of.
         * @details Allocates memory for the new matrix and calculates the determinant of the specified matrix.
         */
        static T determinant(const Matrix<T>& A);
        /**
         * @brief assignment operator for multiplying a matrix with a vector.
         * @param rhs The right-hand side vector to multiply with.
         * @details Allocates memory for the new matrix and multiplies the values from the provided vector.
         * @warning This operator does not check for memory leaks.
         * @return A reference to the current object.
         */
        Matrix& operator*=(const std::vector<T>&);
        /**
        * @brief   divide the values in column i by the value in vector at the index i
        * @param   normVector : the vector to divide by
        * @details The function normalizes the matrix by dividing each element in the column by the corresponding element in the vector.
        */
        Matrix& normalizeByVectorColumn(const std::vector<double>& normVector);
        /**
        * @brief   divide the values in row i by the value in vector at the index i
        * @param   normVector : the vector to divide by
        * @details The function normalizes the matrix by dividing each element in the row by the corresponding element in the vector.
        * @return the reference to the new matrix
        */
        Matrix& normalizeByVectorRow(const std::vector<double>& normVector);
        // functions for reduction and inverse
        /**
         * @brief Function to concatenate two matrices.
         * @param rhs The right-hand side matrix to concatenate with.
         * @details Allocates memory for the new matrix and concatenates the two matrices.
         * @return A new matrix containing the result of the concatenation.
         */
        Matrix concatenateRight(const Matrix&)const;
        /**
         * @brief Function to calculate the determinant of the matrix.
         * @details Allocates memory for the new matrix and calculates the determinant of the specified matrix.
         * @return The determinant of the matrix.
         */
        T determinant()const;
        // not implemented yet
        /**
         * @brief Function to calculate Gaussian elimination.
         * @details Allocates memory for the new matrix and calculates the Gaussian elimination of the specified matrix.
         * @warning This function is not implemented yet.
         * @return A new matrix containing the result of the Gaussian elimination.
         */
        Matrix gaussianElimination();
        /**
         * @brief Function to calculate Gaussian elimination with partial pivoting.
         * @details Allocates memory for the new matrix and calculates the Gaussian elimination with partial pivoting of the specified matrix.
         * @warning This function is not implemented yet.
         * @return A new matrix containing the result of the Gaussian elimination with partial pivoting.
         */
        Matrix rowReduceGaussian();
        /**
         * @brief Function to calculate the inverse of the matrix.
         * @details Allocates memory for the new matrix and calculates the inverse of the specified matrix.
         * @warning This function is not implemented yet.
         * @return A new matrix containing the result of the inverse.
         */
        Matrix inverse();

        //get functions
        /**
         * @brief Function to get the number of rows in the matrix.
         * @return The number of rows in the matrix.
         */
        int getRows()const{return rows_;}
        /**
         * @brief Function to get the number of columns in the matrix.
         * @return The number of columns in the matrix.
         */
        int getCols()const{return cols_;}
        /**
         * @brief Function to check if the matrix is a vector.
         * @return True if the matrix is a vector(vertical, meaning that columns == 1), false otherwise.
         */
        bool isVector()const{return (rows_ >= 1 && cols_ == 1); }
        /**
         * @brief Function to convert the matrix to a vector. Only works if the matrix is a vector.
         * @return A vector containing the elements of the matrix.
         * @throw std::domain_error if the matrix is not a vector.
         */
        std::vector<T> asVector()const; 

        //functions to add rows and columns while mantaining the original data in the upperleft corner(these functions are bad, better use a vector when trying to work with dynamically instantiated data)
        // also these functions create a copy and do not work on the original
        /**
         * @brief Function to create a new matrix with additional rows and columns filled with zeros.
         * @param additionalRows The number of additional rows to add.
         * @param additionalCols The number of additional columns to add.
         * @details Allocates memory for the new matrix and initializes the additional rows and columns with zeros.
         * @return A new matrix containing the result of the addition.
         */
        Matrix copyAndAddRowsColsWithZeros(int additionalRows, int additionalCols) const;

        /**
         * @brief   add a row to the matrix and return a new matrix
         * @param   row :the row to add
         * @param   position : the position where to add the row
         * @return  the pointer to new matrix with the added row
         */
        Matrix* addRowNew(const std::vector<T>& row, int position);

        /**
         * @brief   add a column to the matrix and return a new matrix
         * @param   column :the column to add
         * @param   position : the position where to add the column
         * @return  the pointer to new matrix with the added column
         */
        Matrix* addColumnNew(const std::vector<T>& column, int position);

        /**
         * @brief  add a row to the matrix
         * @param  row : the row to add
         * @param  position : the position where to add the row
         */
        void addRow(const std::vector<T>& row, int position);

        /**
         * @brief  add a column to the matrix
         * @param  column : the column to add
         * @param  position : the position where to add the column
         */
        void addColumn(const std::vector<T>& column, int position);

        /**
         * @brief  add a row to the matrix at the end
         * @param  row : the row to add
         */
        void addRowAtTheEnd(const std::vector<T>& row);

        /**
         * @brief  add a column to the matrix at the end
         * @param  column : the column to add
         */
        void addColumnAtTheEnd(const std::vector<T>& column);



        //functions to convert to armadillo
        /**
         * @brief Function to convert the matrix to an Armadillo matrix(immutable).
         * @return An Armadillo matrix containing the elements of the matrix.
         */
        arma::Mat<T> asArmadilloMatrix()const;
        /**
         * @brief Function to convert the matrix to an Armadillo column vector(immutable).
         * @return An Armadillo column vector containing the elements of the matrix.
         */
        arma::Col<T> asArmadilloColumnVector()const;
        /**
         * @brief Function to convert the matrix to an Armadillo row vector(immutable).
         * @return An Armadillo row vector containing the elements of the matrix.
         */
        arma::Row<T> asArmadilloRowVector()const;
        /**
         * @brief Function to print the matrix.
         * @details Prints the matrix in a readable format.
         */
        void printMatrix()const;


    protected:
        int rows_; ///< Number of rows in the matrix.
        int cols_; ///< Number of columns in the matrix.
        T *_matrix=nullptr; ///< Pointer to the matrix data.

        /**
         * @brief Protected function to allocate memory for the matrix.
         * @details Allocates memory for the matrix and initializes all elements to zero.
         */
        void allocateMatrixSpace();
        Matrix expHelper(const Matrix&, int);
};