/**
 * @file optimization.cxx
 * @ingroup Core
 * @brief Implements matrix multiplication and related operations.
 * @details This file contains functions for matrix multiplication, vector multiplication, and array multiplication, with a focus on optimization.
 * @warning This module is deprecated and will be removed in the future.
 */
#include "data_structures/Matrix.hxx"
#include <chrono>
#include <functional>
#include <stdexcept>

template<typename F, typename M>
double run(F f, M const& a, M const& b, M& c)
{
    double time = 0;
    for (int i = 0; i < 10; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            f(a,b,c);
            auto stop = std::chrono::high_resolution_clock::now();

            auto duration = (stop - start);

            time += duration.count();
    }
    return time / 10;
}

template<typename T>
Matrix<T> MatMul(const Matrix<T>& lhs,const Matrix<T>& rhs,Matrix<T>& result){
    //TODO controls over the feasibility of the multiplication
    if(lhs.getCols() == rhs.getRows()){
        for (int r = 0; r < lhs.getRows(); r++) {
                  for (int c = 0; c < rhs.getCols(); c++) {
                          //result(r,c) = 0;   //already initialized at 0
                          for (int i = 0; i < rhs.getRows(); i++)
                                  result(r,c) += lhs.getValue(r,i) * rhs.getValue(i,c);
                  }
          }
    return result;
    } else {
        throw std::invalid_argument("column dimension of lhs is not equal to row dimension of lhs\n");
    }
    
}

template<typename T>
std::vector<std::vector<T>> matrixMultiplyVector(const std::vector<std::vector<T>> &A,const std::vector<std::vector<T>> &B, std::vector<std::vector<T>> &C) {
    int m = A.size(), n = A[0].size(), p = B[0].size();
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

template<typename T>
T* arrayMult(const Matrix<T>& lhs,T* vec,int size){
    if (lhs.getCols()==size) {
        T* retvec = new T[size];
        for (int i = 0; i< size; i++) {
            retvec[i] = 0;  //not good if it is a non numerical value
        }
        int m = lhs.getRows(), n = lhs.getCols();
        for (int i = 0; i < m; i++) {
            for (int k = 0; k < n; k++) {
                retvec[i] += lhs[i][k] * vec[k];
            }
        }
        return retvec;
    } else throw std::invalid_argument("arrayMult vector has not the right size to be multiplied");
}
