#include <armadillo>
#include <vector>

template<typename T>
std::vector<std::vector<T>> armaMatrixToVector(arma::Mat<T> matr){return arma::conv_to< std::vector<T> >::from(arma::vectorise( matr));}

template<typename T>
std::vector<T> armaColumnToVector(arma::Col<T> matr){return arma::conv_to< std::vector<T> >::from(matr);}

template<typename T>
std::vector<T> armaRowToVector(arma::Row<T> matr){return arma::conv_to< std::vector<T> >::from(matr);}

template<typename T>
arma::Col<T> vectorToArmaColumn(std::vector<T> vec){return arma::Col<T>(vec);}

template<typename T>
arma::Row<T> vectorToArmaRow(std::vector<T> vec){return arma::Row<T>(vec);}

template<typename T>
arma::Mat<T> vectorToArmaMatrixRepeatColumns(std::vector<T> vec, int n){return arma::repmat(arma::Col<T>(vec), 1, n);}

template<typename T>
arma::Mat<T> normalizeColumns(arma::Mat<T> matr);

template<typename T>
arma::Mat<T> normalizeRows(arma::Mat<T> matr);

template<typename T>
arma::Mat<T> normalize1Columns(arma::Mat<T> matr);

template<typename T>
arma::Mat<T> normalize1Rows(arma::Mat<T> matr);

void print_mat(arma::mat my_matrix);