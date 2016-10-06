#include "Utils.h"
#include "atlimage.h"
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"


boost::mt19937 rng;
boost::uniform_01<> dist;

//double randf() {
//	return (double)rand() / RAND_MAX;
//}
double randf() {
	//return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
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

arrayd w1 = make_array3d(1, 0, 0), w = w1 / norm(w1);
double up2w(sp<BorderPoint> x) {
	return x->t > EPS ? 
		.2 + 20*exp(-10 * norm(x->dir - w) * norm(x->dir - w)) :
		0;
}

double up2w_pos_dir(sp<BorderPoint> x) {
	return x->t > EPS ?
		0.3 + 
//		exp(-sqr(x->t/20. - 100.)) *
		25. * exp(-5 * (norm(x->dir + w) + norm(x->pos / norm(x->pos) - w))) :
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

int no_more255(double value) {
	return value > 255. ? 255 : int(value);
}

double take_point_ret_zero(sp<Point>)
{
	return 0.0;
}

arrayd make_rand_norm_3d() {
	arrayd ret = { randf() * 2 - 1, randf() * 2 - 1, 0 };
	ret /= norm(ret);
	return ret;
}

stringstream loger;

