#include "Point.h"
#include "Utils.h"
#include <assert.h>
#include <fstream>



Point::Point(double t, arrayd pos, arrayd dir, Media * media):
t(t), pos(pos), dir(dir/norm(dir)), media(media)
{
//	assert(media->in(this));
}

arrayd Matmul(arrayd mat, arrayd vec) {
	arrayd res(3);
	for (int i = 0; i < 9; i += 3) {
		res[i / 3] = dot(make_array3d(mat[i], mat[i+1], mat[i+2]), vec);
	}
	return res;
}

arrayd rorate2array_matrix(arrayd x) {
	double x1 = x[0], x2 = x[1], x3 = x[2];
	if (abs(x3 - 1) < EPS) {
		double buf[] = { 1, 0, 0,
			0, 1, 0,
			0, 0, 1 };

		return arrayd(buf, 9);
	}
	double denom = sqrt(1 - x3);
	double buf[] = { x1 * x3 / denom, -x2 / denom, x1,
		x2 * x3 / denom, x1 / denom, x2,
		-denom, 0, x3 };
	return arrayd(buf, 9);
}

double exp_rand(double l, double maxv) {
	return log((exp(-l * maxv) - 1) * randf() + 1) / -l;
}

#define NONBRANCH 5
double Point::f(int n) {
	//loger << "f";
	double min_dist = media->int_dist(this);
	min_dist = min_dist < 0 ? 0 : min_dist;
	Media *nearest = media->overmedia;
	for (auto &i : media->submedia) {
		double cur = i->ext_dist(this);
		if (cur > 0 && cur < min_dist) {
			min_dist = cur;
			nearest = i;
		}
	}
	double border_t = t - min_dist / media->v;
	if (border_t < 0) {
		nearest = media;
		border_t = 0;
		min_dist = media->v * t;
	}
	arrayd border_pos = pos - dir * min_dist;
	double optical_dist = exp(-media->mu * min_dist);

	sp<BorderPoint> border_point(new BorderPoint(border_t, border_pos, dir, nearest, media));
#ifdef NONBRANCH
	if (n == -10) {
#else
	if (n == 0) {
#endif // NONBRANCH
		return optical_dist * nearest->border(border_point);
	}

#ifdef NONBRANCH
	double coin;
	if ((n <= NONBRANCH) && (coin = randf()) < optical_dist) {
		return nearest->border(border_point) + border_point->f(n - 1);
	}
#endif // NONBRANCH
	double scat = 0;
	int m = GlobalMedia::instance()->m;
	if (m && min_dist > EPS) {
		arrayd Q = rorate2array_matrix(dir);
		arrayd new_dir = Matmul(Q, media->rand_dir());
		double rand_dist = exp_rand(media->mu, min_dist - EPS);
		arrayd new_pos = pos - dir * rand_dist;
		sp<Point> p(new Point(t - rand_dist / media->v, new_pos, new_dir, media));

		scat = 1. / media->mu / m * (p->f(n - 1) * media->mu_s + media->intern(p));
	}
#ifdef NONBRANCH
	if (n > NONBRANCH) {
		return optical_dist * (nearest->border(border_point) + border_point->f(n - 1)) + (1 - optical_dist) * scat;
	}
	else {
		return scat;
	}
#endif // NONBRANCH
	return optical_dist * (nearest->border(border_point) + border_point->f(n - 1)) + (1 - optical_dist) * scat;
}

BorderPoint::BorderPoint(double t, arrayd pos, arrayd dir, Media *media_from, Media *media_to):
	Point(t, pos, dir, media_from), media_to(media_to) {
}
int counter = 0;
double avg_n = 0;
double BorderPoint::f(int n) {
	if (t < EPS || media->is_global()) {
		//avg_n = avg_n * counter / (counter + 1) + (20. - n) / (counter + 1);
		//counter++;
		return 0;
	}
	arrayd normal;
	if (media->overmedia == media_to) {
		normal = media->normal(pos);
	}
	else {
		normal = -media_to->normal(pos);
	}
	double k = media_to->k / media->k;
	double cosAfter = dot(dir, normal);

	arrayd dirR = dir - 2 * cosAfter * normal;
	//loger << "R " <<   n <<endl;
	double cosT = 1 - k * k * (1 - cosAfter * cosAfter);
	if (cosT < 0) {
		return Point(t, pos, dirR, media_to).f(n);
	}
	cosT = sqrt(cosT);
	arrayd dirT = cosT * normal + k * (dir - cosAfter * normal);
	
	double a = (k * cosT + cosAfter),
		b = (cosT + k * cosAfter),
		c = 2. * cosT,
		Rpar = (k * cosT - cosAfter) / a,
		Rper = (cosT - k * cosAfter) / b,
		Tpar = c / a,
		Tper = c / b,
		R = 1. / 2. * (Rpar * Rpar + Rper * Rper),
		T = 1. / 2. * (Tpar * Tpar + Tper * Tper) * k * cosAfter / cosT;
		double q = dot(dirT, normal), w = dot(dirR, normal);
		// cout << "T: " << T << " R: " << R << ' ' << partR <<  endl;
#ifdef NONBRANCH
		if (n <= NONBRANCH)
		return randf() < R / (R + T) ?
			Point(t, pos, dirR, media_to).f(n) :
			Point(t, pos, dirT, media).f(n);
#endif // NONBRANCH
		return R * Point(t, pos, dirR, media_to).f(n) + T * Point(t, pos, dirT, media).f(n);
}

Point::~Point()
{
}
