#pragma once

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


struct BorderPoint;
struct Point;
typedef valarray<double> arrayd;

double randf();
void reset_rand();
arrayd make_array3d(double x, double y, double z);
double dot(arrayd x, arrayd y);
double norm(arrayd x);
double indic_isotrophy();
double indic_direct();
double begining(sp<BorderPoint> x);
double up2w(sp<BorderPoint> x);
double up2w_pos_dir(sp<BorderPoint> x);
double around_c(sp<BorderPoint> x);
void print3d(arrayd x);
extern stringstream loger;
int no_more255(int value);
arrayd make_rand_norm_3d();

double take_point_ret_zero(sp<Point>);


#include "Point.h"

class NanResult {};
