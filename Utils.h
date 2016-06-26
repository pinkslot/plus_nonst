#pragma once

#include <cstdlib>
#define _USE_MATH_DEFINES
#include <math.h>
#include <valarray>
#include <memory>
using namespace std;
#include <numeric>      // std::inner_product
#include <sstream>
#include <cassert>
#define EPS 1E-10

#define sp shared_ptr

struct BorderPoint;
struct Point;
typedef valarray<double> arrayd;

double randf();
void reset_rand();
arrayd make_array3d(double x, double y, double z);
double dot(arrayd x, arrayd y);
inline double norm(arrayd x);
inline arrayd normalize(arrayd x) { return x / norm(x); }
double begining(sp<BorderPoint> x);
double up2w(sp<BorderPoint> x);
double up2w_pos_dir(sp<BorderPoint> x);
double around_c(sp<BorderPoint> x);
void print3d(arrayd x);
extern stringstream loger;
int no_more255(double value);
arrayd make_rand_norm_3d();
inline double sqr(double x) { return x * x; }
double take_point_ret_zero(sp<Point>);

inline double indic_direct() { return 1; }
inline double indic_isotrophy() { return 2 * randf() - 1; }
template<int a, int b = 1>
double indic_henyey() {
	double g = double(b) / double(a);
	assert(g < 1);
	return abs(g) < EPS ? indic_isotrophy() :
		1. / 2. / g * (1 + g*g - sqr((1 - g*g) / (1 - g + 2 * g*randf())));
}

#include "Point.h"

class NanResult {};
