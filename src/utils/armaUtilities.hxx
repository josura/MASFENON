/**
 * @file armaUtilities.hxx
 * @ingroup Core
 * @brief  This file contains the declaration of the Armadillo utilities functions
 * @details  The functions are used to work on Armadillo matrices and vectors
 */
#include <armadillo>
#include <vector>

/**
 * @brief  convert a Armadillo matrix to a vector of vectors
 * @param matr the Armadillo matrix
 * @return the vector of vectors
 */
template<typename T>
std::vector<std::vector<T>> armaMatrixToVector(arma::Mat<T> matr){return arma::conv_to< std::vector<T> >::from(arma::vectorise( matr));}

/**
 * @brief  convert a Armadillo column vector to a vector
 * @param matr the Armadillo column vector
 * @return the vector
 */
template<typename T>
std::vector<T> armaColumnToVector(arma::Col<T> matr){return arma::conv_to< std::vector<T> >::from(matr);}

/**
 * @brief  convert a Armadillo row vector to a vector
 * @param matr the Armadillo row vector
 * @return the vector
 */
template<typename T>
std::vector<T> armaRowToVector(arma::Row<T> matr){return arma::conv_to< std::vector<T> >::from(matr);}

/**
 * @brief  convert a vector to an armadillo column vector
 * @param vec the vector
 * @return the Armadillo column vector
 */
template<typename T>
arma::Col<T> vectorToArmaColumn(std::vector<T> vec){return arma::Col<T>(vec);}

/**
 * @brief  convert a vector to an armadillo row vector
 * @param vec the vector
 * @return the Armadillo row vector
 */
template<typename T>
arma::Row<T> vectorToArmaRow(std::vector<T> vec){return arma::Row<T>(vec);}

/**
 * @brief  convert a vector to an armadillo matrix, repeating the vector n times
 * @param vec the vector
 * @param n the number of times to repeat the vector
 * @return the Armadillo matrix
 * @details  The vector is repeated n times as columns
 * @note  The vector must be of size n > 0
 * @throw std::invalid_argument if n <= 0
 */
template<typename T>
arma::Mat<T> vectorToArmaMatrixRepeatColumns(std::vector<T> vec, int n){
    if (n<=0) {
        throw std::invalid_argument("vectorToArmaMatrixRepeatColumns: n must be greater than 0");
    }
    return arma::repmat(arma::Col<T>(vec), 1, n);
}

/**
 * @brief  normalize the columns of an armadillo matrix
 * @param matr the Armadillo matrix
 * @return the normalized Armadillo matrix
 * @details  The columns are normalized to have a norm of 1
 */
template<typename T>
arma::Mat<T> normalizeColumns(arma::Mat<T> matr);

/**
 * @brief  normalize the rows of an armadillo matrix
 * @param matr the Armadillo matrix
 * @return the normalized Armadillo matrix
 * @details  The rows are normalized to have a norm of 1
 */
template<typename T>
arma::Mat<T> normalizeRows(arma::Mat<T> matr);

/**
 * @brief  normalize the columns of an armadillo matrix to have a sum of 1
 * @param matr the Armadillo matrix
 * @return the normalized Armadillo matrix
 * @details  The columns are normalized to have a sum of 1
 * @warning  The columns are normalized to have a sum of 1, not a norm of 1
 * @warning  Negative values could lead to a final sum of 0
 */
template<typename T>
arma::Mat<T> normalize1Columns(arma::Mat<T> matr);

/**
 * @brief  normalize the rows of an armadillo matrix to have a sum of 1
 * @param matr the Armadillo matrix
 * @return the normalized Armadillo matrix
 * @details  The rows are normalized to have a sum of 1
 * @warning  The rows are normalized to have a sum of 1, not a norm of 1
 * @warning  Negative values could lead to a final sum of 0
 */
template<typename T>
arma::Mat<T> normalize1Rows(arma::Mat<T> matr);

/**
 * @brief  print a Armadillo matrix
 * @param my_matrix the Armadillo matrix
 * @details  The matrix is printed to the standard output
 */
void print_mat(arma::mat my_matrix);