#include "utils/mathUtilities.hxx"


//random generation for different types

int randomNumber(int min, int max){
    // fixed seed because repeatability
    //unsigned seed = 777;
    std::random_device r;
    // range [min,max[
    std::default_random_engine e1(r());
    max = (max-1<0) ? 0 : max-1;
    std::uniform_int_distribution<int> uniform_dist(min, max);
    return uniform_dist(e1);
}

double randomRealNumber(double min, double max){
    // fixed seed because repeatability
    //unsigned seed = 777;
    std::random_device r;
    // range [min,max]
    std::default_random_engine e1(r());
    std::uniform_real_distribution<double> uniform_dist(min, max);
    return uniform_dist(e1);
}

char generateRandomCharacter() {
  // Generate a random number between 0 and 25
  int randomNumberInt = randomNumber(0, 25);

  // Convert the random number to a character
  // by adding 'a' (the ASCII value for 'a' is 97)
  char randomCharacter = (char)(randomNumberInt + 'a');

  return randomCharacter;
}

void setRandom(int& val) { 
    int INTMAX = std::numeric_limits<int>::max();
    val =  randomNumber(-INTMAX, INTMAX);
}
void setRandom(double& val) { 
    double DOUBLEMAX = std::numeric_limits<double>::max();
    val = randomRealNumber(-DOUBLEMAX, DOUBLEMAX);
}
void setRandom(char& val) { 
    val = generateRandomCharacter();
}

int SizeToInt(size_t u)
{
    if (u > std::numeric_limits<int>::max())
    {
        throw std::overflow_error(
            "size_t value cannot be stored in a variable of type int.");
    }

    return static_cast<int>(u);
}

long int szudzik(int x, int y)
{
    return x >= y ? x * x + x + y : x + y * y;
}

bool approximatelyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool essentiallyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyGreaterThan(float a, float b, float epsilon)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyLessThan(float a, float b, float epsilon)
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool approximatelyEqual(double a, double b, double epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool essentiallyEqual(double a, double b, double epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyGreaterThan(double a, double b, double epsilon)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyLessThan(double a, double b, double epsilon)
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}


double hyperbolicTangentScaled(double xInput, double scaleFactor ){
    double firstTerm = std::exp(xInput/scaleFactor);
    double secondTerm = std::exp(-xInput/scaleFactor);
    return scaleFactor*(firstTerm - secondTerm)/(firstTerm + secondTerm);
}


double lerping(double a, double b, double t){
    return a + t * (b - a);
}


bool setDoubleContainsInterval(std::set<double> set, double lower, double upper){
    if(lower > upper){
        throw std::invalid_argument("utilities::setDoubleContainsInterval: lower bound is greater than upper bound");
    }
    if(set.lower_bound(lower) != set.end() && *set.lower_bound(lower) < upper){
        return true;
    }
    return false;
}

int setDoubleIntervalWidth(std::set<double> set, double lower, double upper){
    if(lower > upper){
        throw std::invalid_argument("utilities::setDoubleContainsInterval: lower bound is greater than upper bound");
    }
    int count = 0;
    for(auto iter = set.cbegin(); iter != set.cend(); iter++){
        if(*iter >= lower && *iter<upper){
            count++;
        }
    }
    return count;
}



double vectorNorm(std::vector<double> vec){
    double norm=0;
    for (uint i = 0; i < vec.size(); ++i) {
        norm+=vec[i]*vec[i];
    }
    norm=sqrt(norm);
    return norm;
}