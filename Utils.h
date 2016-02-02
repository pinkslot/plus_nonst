#include <cstdlib>
#define _USE_MATH_DEFINES
#include <math.h>
#include <valarray>
#include <memory>
using namespace std;
#include <numeric>      // std::inner_product
#include <sstream>
#define EPS 1E-10

#define sp shared_ptr
typedef valarray<double> arrayd;

double randf();
arrayd make_array3d(double x, double y, double z);
double dot(arrayd x, arrayd y);
double norm(arrayd x);
double indic_isotrophy();
double indic_direct();
struct BorderPoint;
double begining(sp<BorderPoint> x);
double up2w(sp<BorderPoint> x);
double around_c(sp<BorderPoint> x);
void print3d(arrayd x);
extern stringstream loger;
int no_more255(int value);

#include "Point.h"
