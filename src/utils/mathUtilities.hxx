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
 * @param  min : the minimum value
 * @param  max : the maximum value
*/
int randomNumber(int min, int max);
/**
 * @brief   Generate a random real number between min and max
 * @return  the random number
 * @param  min : the minimum value
 * @param  max : the maximum value
*/
double randomRealNumber(double min, double max);
/**
 * @brief   Generate a random character between a and z
 * @return  the random character
 * @details the character is generated using the ASCII code, the function generates a random number between 0 and 25 and adds it to the ASCII code of 'a' (97)
*/
char generateRandomCharacter();

/**
 * @brief   Generate a random integer vector of length size
 * @return  the random vector
 * @param  min : the minimum value
 * @param  max : the maximum value
 * @param  size : the size of the vector
*/
std::vector<int> randomVector(int min, int max , int size);

/**
 * @brief   Generate a random Boolean vector of length size
 * @return  the random vector
 * @param  size : the size of the vector
 * @details the vector is generated using the random number generator, the values are between 0 and 1, if the value is less than 0.5, it is set to false, otherwise it is set to true
 * @warning it is a dynamic array, the caller is responsible for deleting it
*/
bool* randomBooleanArray(int size);

/**
 * @brief   szudzik function to generate a unique integer from two integers
 * @return  the unique integer
 * @param  x : the first integer
 * @param  y : the second integer
 * @details the function is used to generate a unique integer from two integers, the function is used to generate a hash value for a pair of integers
 * @see https://en.wikipedia.org/wiki/Szudzik%27s_function
*/
long int szudzik(int x, int y);


/**
 * @brief  Generate the intersection of two sets
 * @return  the intersection of the two sets
 * @param  set1 : the first set
 * @param  set2 : the second set
*/
std::unordered_set<int> intersectionSet(std::unordered_set<int> set1,std::unordered_set<int> set2);

// setters for random values
void setRandom(int& val);
void setRandom(double& val);
void setRandom(char& val);

//generate random matrix
/**
 * @brief   Generate a random matrix of size rows x cols
 * @return  the random matrix
 * @param  rows : the number of rows
 * @param  cols : the number of columns
 * @details the matrix is generated using the random number generator.
 * This function is mainly used for testing purposes.
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
/**
 * @brief  Convert a size_t to an int
 * @return  the int value
 * @param  u : the size_t value
 * @details the function is used to convert a size_t to an int, the function throws an exception if the size_t value is greater than the maximum value of an int
 * This function is mainly used to avoid overflow errors when using size_t values in functions that expect int values, and to get rid of the warnings.
 */
int SizeToInt(size_t u);

// accessory functions for the comparison of floating point numbers
/**
 * @brief  Compare two floating point numbers
 * @return  true if the numbers are approximately equal, false otherwise
 * @param  a : the first number
 * @param  b : the second number
 * @param  epsilon : the tolerance value
 * @details the function is used to compare two floating point numbers, the function returns true if the numbers are approximately equal, false otherwise
 * approximately Equal means that |a-b| <= epsilon * max(|a|, |b|)
 */
bool approximatelyEqual(float a, float b, float epsilon);
/**
 * @brief  Compare two floating point numbers
 * @return  true if the numbers are essentially equal, false otherwise
 * @param  a : the first number
 * @param  b : the second number
 * @param  epsilon : the tolerance value
 * @details the function is used to compare two floating point numbers, the function returns true if the numbers are essentially equal, false otherwise
 * essentially Equal means that |a-b| <= epsilon * min(|a|, |b|)
 */
bool essentiallyEqual(float a, float b, float epsilon);
/**
 * @brief  Compare two floating point numbers
 * @return  true if the first number is definitely greater than the second number, false otherwise
 * @param  a : the first number
 * @param  b : the second number
 * @param  epsilon : the tolerance value
 * @details the function is used to compare two floating point numbers, the function returns true if the first number is definitely greater than the second number, false otherwise
 * definitelyGreaterThan means that a-b > epsilon * max(|a|, |b|)
 */
bool definitelyGreaterThan(float a, float b, float epsilon);
/**
 * @brief  Compare two floating point numbers
 * @return  true if the first number is definitely less than the second number, false otherwise
 * @param  a : the first number
 * @param  b : the second number
 * @param  epsilon : the tolerance value
 * @details the function is used to compare two floating point numbers, the function returns true if the first number is definitely less than the second number, false otherwise
 * definitelyLessThan means that b-a > epsilon * max(|a|, |b|)
 */
bool definitelyLessThan(float a, float b, float epsilon);

/**
 * @brief  Compare two floating point numbers (double precision)
 * @return  true if the numbers are approximately equal, false otherwise
 * @param  a : the first number
 * @param  b : the second number
 * @param  epsilon : the tolerance value
 * @details the function is used to compare two floating point numbers, the function returns true if the numbers are approximately equal, false otherwise
 * approximately Equal means that |a-b| <= epsilon * max(|a|, |b|)
 */
bool approximatelyEqual(double a, double b, double epsilon);
/**
 * @brief  Compare two floating point numbers (double precision)
 * @return  true if the numbers are essentially equal, false otherwise
 * @param  a : the first number
 * @param  b : the second number
 * @param  epsilon : the tolerance value
 * @details the function is used to compare two floating point numbers, the function returns true if the numbers are essentially equal, false otherwise
 * essentially Equal means that |a-b| <= epsilon * min(|a|, |b|)
 */
bool essentiallyEqual(double a, double b, double epsilon);
/**
 * @brief  Compare two floating point numbers (double precision)
 * @return  true if the first number is definitely greater than the second number, false otherwise
 * @param  a : the first number
 * @param  b : the second number
 * @param  epsilon : the tolerance value
 * @details the function is used to compare two floating point numbers, the function returns true if the first number is definitely greater than the second number, false otherwise
 * definitelyGreaterThan means that a-b > epsilon * max(|a|, |b|)
 */
bool definitelyGreaterThan(double a, double b, double epsilon);
/**
 * @brief  Compare two floating point numbers (double precision)
 * @return  true if the first number is definitely less than the second number, false otherwise
 * @param  a : the first number
 * @param  b : the second number
 * @param  epsilon : the tolerance value
 * @details the function is used to compare two floating point numbers, the function returns true if the first number is definitely less than the second number, false otherwise
 * definitelyLessThan means that b-a > epsilon * max(|a|, |b|)
 */
bool definitelyLessThan(double a, double b, double epsilon);

/**
 * @brief  Function that implements the hyperbolic tangent function (scaled with the scale factor)
 * @return  the scaled hyperbolic tangent value
 * @param  xInput : the input value
 * @param  scaleFactor : the scale factor
 * @details scale the hyperbolic tangent function, the return value is always < c , the function is also scaled to grow linearly before reaching the transient
*/
double hyperbolicTangentScaled(double xInput, double scaleFactor );

/**
 * @brief  Linear interpolation between two values, a and b, with a parameter t going from 0 to 1
 * @return  the interpolated value
 * @param  a : the first value
 * @param  b : the second value
 * @param  t : the parameter going from 0 to 1
 * @throws std::invalid_argument if t is not in the range [0, 1]
*/

double lerping(double a, double b, double t);

/**
 * @brief  Convert an array to a vector
 * @return  the vector
 * @param  array : the array
 * @param  size : the size of the array
 * @details the function is used to convert an array to a vector, the function is used to avoid using dynamic arrays
*/
template<typename T>
std::vector<T> arrayToVector(T* array, int size){
    return std::vector<T>(array, array + size);
}


/**
 * @brief  Get the keys of a map
 * @return  the keys of the map
 * @param  input_map : the map
 * @details the function is used to get the keys of a map, the function is used to avoid using dynamic arrays
*/
template<typename K, typename V>
std::vector<K> getKeys(std::map<K,V> const& input_map) {
    std::vector<K> retval;
    for (auto const& element : input_map) {
        retval.push_back(element.first);
    }
    return retval;
}

// hash structures for unordered_map
/**
 * @brief Hash structure for a pair of strings
 * @details the structure is used to hash a pair of strings, the function is used to generate a hash value for a pair of strings
 * the function implemented inside is used to generate a hash value for a pair of strings
 */
struct hash_pair_strings {
    /**
     * @brief  Hash function for a pair of strings, implemented with the operator() that will be used by unordered_map
     * @return  the hash value
     * @param  p : the pair of strings
     * @details the function is used to generate a hash value for a pair of strings, the function is used to generate a hash value for a pair of strings
     */
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

/**
 * @brief Hash structure for a pair of integers
 * @details the structure is used to hash a pair of integers, the function is used to generate a hash value for a pair of integers
 * the function implemented inside is used to generate a hash value for a pair of integers
 */
struct hash_pair_ints {
    /**
     * @brief  Hash function for a pair of integers, implemented with the operator() that will be used by unordered_map
     * @return  the hash value
     * @param  p : the pair of integers
     * @details the function is used to generate a hash value for a pair of integers, the function is used to generate a hash value for a pair of integers. Uses szudzik function to generate a unique integer from two integers
     * @see https://en.wikipedia.org/wiki/Szudzik%27s_function
     * @see utils/mathUtilities.hxx szudzik function
     */
    size_t operator()(const std::pair<int, int>& p) const
    {
        long int tmp;
        tmp = szudzik(p.first,p.second);

        auto hashStrings = std::hash<long int>();
         
        return hashStrings(tmp);
    }
};

/**
 * @brief Hash structure for a quadruple of strings
 * @details the structure is used to hash a quadruple of strings, the function is used to generate a hash value for a quadruple of strings
 * the function implemented inside is used to generate a hash value for a quadruple of strings
 */
struct hash_quadruple_strings {
    /**
     * @brief  Hash function for a quadruple of strings, implemented with the operator() that will be used by unordered_map
     * @return  the hash value
     * @param  t : the quadruple of strings
     * @details the function is used to generate a hash value for a quadruple of strings, the function is used to generate a hash value for a quadruple of strings
     */
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
 * @param set is the set of values
 * @param lower is the lower bound of the interval
 * @param upper  is the upper bound of the interval
 * @return true if the set contains the interval, false otherwise
 */
bool setDoubleContainsInterval(std::set<double> set, double lower, double upper);


/**
 * @brief   Returns how many values fall in the interval:
 * @param set is the set of values
 * @param lower is the lower bound of the interval
 * @param upper  is the upper bound of the interval
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
