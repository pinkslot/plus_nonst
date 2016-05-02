#include "Utils.h"
#include "atlimage.h"
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"

typedef boost::mt19937 RNGType;
RNGType rng;
boost::uniform_01<> dist;


//double randf() {
//	return (double)rand() / RAND_MAX;
//}
double randf() {
	return dist(rng);
}

void reset_rand() {
	rng.seed();
	dist.reset();
}

arrayd make_array3d(double x, double y, double z) {
	double a[] = { x, y, z };
	return arrayd(a, 3);
}

double dot(arrayd x, arrayd y) {
	return (x * y).sum();
}

double norm(arrayd x) {
	return sqrt(dot(x, x));
}

double indic_isotrophy() {
	return randf();
}
double indic_direct() {
	return 1;
}

double begining(sp<BorderPoint> x) {
	if (x->t < EPS) {
		return 1.;
	}
	else return 0.;
}

int no_more(int value)
{
	return 0;
}

void print3d(arrayd x) {
	cout << x[0] << ' ' << x[1] << ' ' << x[2] << endl;
}

arrayd w1 = make_array3d(-1, 0, -1.), w = w1 / norm(w1);
double up2w(sp<BorderPoint> x) {
	return x->t > EPS ? 
		.3 + exp(-.5 * norm(x->dir - w) * norm(x->dir - w)) :
		0;
}

double monus(double x, double y) {
	return x > y ? x - y : 0;
}
arrayd c = make_array3d(0, 0, -10);
double r = 10;
double around_c(sp<BorderPoint> x) {
	double dif = norm(x->pos - c);
	return x->t > -1 ? monus(r, dif) / r * 5. : 0;
}

int no_more255(int value) {
	return value > 255 ? 255 : value;
}

double take_point_ret_zero(sp<Point>)
{
	return 0.0;
}

stringstream loger;
int counter = 0;