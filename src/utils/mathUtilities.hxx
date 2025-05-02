/**
 * @file   mathUtilities.hxx
 * @ingroup Core
 * @brief  This file contains the declaration of the math utilities functions
 * @details  The functions are used to work on numbers, vectors and matrices
 */
#pragma once

#include <random>
#include <vector>
#include <unordered_set>
#include <set>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <map>
#include <algorithm>
/**
 * @brief   Generate a random integer number between min and max
 * @return  the random number
*/
int randomNumber(int min, int max);
/**
 * @brief   Generate a random real number between min and max
 * @return  the random number
*/
double randomRealNumber(double min, double max);
/**
 * @brief   Generate a random character between A and Z
 * @return  the random character
*/
char generateRandomCharacter();

/**
 * @brief   Generate a random integer vector of length size
 * @return  the random vector
*/
std::vector<int> randomVector(int min, int max , int size);

/**
 * @brief   Generate a random Boolean vector of length size
 * @return  the random vector
*/
bool* randomBooleanArray(int size);

/**
 * @brief   szudzik function to generate a unique integer from two integers
 * @return  the unique integer
 * @param  x : the first integer
 * @param  y : the second integer
*/
long int szudzik(int x, int y);


/**
 * @brief  Generate the intersection of two sets
 * @return  the intersection of the two sets
*/
std::unordered_set<int> intersectionSet(std::unordered_set<int> set1,std::unordered_set<int> set2);

void setRandom(int& val);
void setRandom(double& val);
void setRandom(char& val);

//generate random matrix
/**
 * @brief   Generate a random matrix of size rows x cols
 * @return  the random matrix
*/
template<typename T>
std::vector<std::vector< T>> createRandomMatrixVector(int rows,int cols){
    std::vector<std::vector< T>> retMat=std::vector<std::vector<double>>(rows, std::vector<double>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            setRandom(retMat[i][j]);
        }
    }
    return retMat;
}


// convert size to int and  launch an exception if it is not possible
int SizeToInt(size_t u);

bool approximatelyEqual(float a, float b, float epsilon);
bool essentiallyEqual(float a, float b, float epsilon);
bool definitelyGreaterThan(float a, float b, float epsilon);
bool definitelyLessThan(float a, float b, float epsilon);


bool approximatelyEqual(double a, double b, double epsilon);
bool essentiallyEqual(double a, double b, double epsilon);
bool definitelyGreaterThan(double a, double b, double epsilon);
bool definitelyLessThan(double a, double b, double epsilon);

/**
scale the hyperbolic tangent function, the return value is always < c , the function is also scaled to grow linearly before reaching the transient
*/
double hyperbolicTangentScaled(double xInput, double scaleFactor );

/**
 * @brief  Linear interpolation between two values, a and b, with a parameter t going from 0 to 1
 * @return  the interpolated value
*/

double lerping(double a, double b, double t);

/**
 * @brief  Convert an array to a vector
 * @return  the vector
*/

template<typename T>
std::vector<T> arrayToVector(T* array, int size){
    return std::vector<T>(array, array + size);
}


// function to return the keys in a map
template<typename K, typename V>
std::vector<K> getKeys(std::map<K,V> const& input_map) {
    std::vector<K> retval;
    for (auto const& element : input_map) {
        retval.push_back(element.first);
    }
    return retval;
}

// A hash function used to hash a pair of any kind
struct hash_pair_strings {
    size_t operator()(const std::pair<std::string, std::string>& p) const
    {
        std::string tmp;
        // in case the order of the pair does not matter
        // if( p.first < p.second ) {
        //     tmp = p.first + p.second;
        // }
        // else {
        //     tmp = p.second + p.first;
        // }

        // in case the order of the pair matters
        tmp = p.first + p.second;

        auto hashStrings = std::hash<std::string>();
         
        // If hash1 == hash2, their XOR is zero.
        return hashStrings(tmp);
    }
};

struct hash_pair_ints {
    size_t operator()(const std::pair<int, int>& p) const
    {
        long int tmp;
        tmp = szudzik(p.first,p.second);

        auto hashStrings = std::hash<long int>();
         
        // If hash1 == hash2, their XOR is zero.
        return hashStrings(tmp);
    }
};

struct hash_quadruple_strings {
    size_t operator()(const std::tuple<std::string, std::string,std::string, std::string>& t) const
    {
        std::string tmp;
        // in case the order of the pair does not matter
        // if( p.first < p.second ) {
        //     tmp = p.first + p.second;
        // }
        // else {
        //     tmp = p.second + p.first;
        // }

        // in case the order of the tuple matters
        tmp = std::get<0>(t) + std::get<1>(t) + std::get<2>(t) + std::get<3>(t);

        auto hashStrings = std::hash<std::string>();
         
        // If hash1 == hash2, their XOR is zero.
        return hashStrings(tmp);
    }
};

/**
 * @brief   Returns a boolean value if the set contains the interval(width is nonzero):
 *         - set is the set of values
 *         - lower is the lower bound of the interval
 *         - upper  is the upper bound of the interval
 * @return true if the set contains the interval, false otherwise
 */
bool setDoubleContainsInterval(std::set<double> set, double lower, double upper);


/**
 * @brief   Returns how many values fall in the interval:
 *         - set is the set of values
 *         - lower is the lower bound of the interval
 *         - upper  is the upper bound of the interval
 * @return the number of values that fall in the interval
 */
int setDoubleIntervalWidth(std::set<double> set, double lower, double upper);


template<typename T>
std::vector<T> vectorSubtraction(std::vector<T> vec1, std::vector<T> vec2){
    if(vec1.size()!=vec2.size()){
        throw std::invalid_argument( "utilities::vectorSubtraction : vectors have different sizes" );
    }
    std::vector<T> retVec;
    for (int i = 0; i < vec1.size(); ++i) {
        retVec.push_back(vec1[i]-vec2[i]);
    }
    return retVec;
}

template<typename T>
std::vector<T> vectorAddition(std::vector<T> vec1, std::vector<T> vec2){
    if(vec1.size()!=vec2.size()){
        throw std::invalid_argument( "utilities::vectorAddition : vectors have different sizes" );
    }
    std::vector<T> retVec;
    for (int i = 0; i < vec1.size(); ++i) {
        retVec.push_back(vec1[i]+vec2[i]);
    }
    return retVec;
}

template<typename T>
std::vector<T> vectorsIntersection(std::vector<T> vec1, std::vector<T> vec2){
    std::vector<T> retVec;
    for (uint i = 0; i < vec1.size(); ++i) {
        auto it = std::find(vec2.begin(), vec2.end(), vec1[i]);
        if (it != vec2.end()) {
            retVec.push_back(vec1[i]);
        }
    }
    return retVec;
}


template<typename T>
std::vector<T> vectorNormalization(std::vector<T> vec){
    T norm=0;
    for (int i = 0; i < vec.size(); ++i) {
        norm+=vec[i]*vec[i];
    }
    norm=sqrt(norm);
    for (int i = 0; i < vec.size(); ++i) {
        vec[i]=vec[i]/norm;
    }
    return vec;
}

double vectorNorm(std::vector<double> vec);

template<typename T>
std::vector<T> vectorScalarMultiplication(std::vector<T> vec, T scalar){
    for (uint i = 0; i < vec.size(); ++i) {
        vec[i]=vec[i]*scalar;
    }
    return vec;
}

template<typename T>
bool vectorContains(std::vector<T> vec, const T element){
    for (uint i = 0; i < vec.size(); ++i) {
        if(vec[i]==element){
            return true;
        }
    }
    return false;
}

template<typename T>
int getIndex(std::vector<T> v, T K)
{
    auto it = find(v.begin(), v.end(), K);
  
    // If element was found
    if (it != v.end()) 
    {
        return (it - v.begin());
    }
    else {
        return -1;
    }
}
