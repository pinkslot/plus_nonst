#include "Point.h"
#include "Utils.h"
#include <assert.h>
#include <fstream>

Point::Point(double t, arrayd pos, arrayd dir, Media * media):
t(t), pos(pos), dir(dir/norm(dir)), media(media)
{
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

double Point::f(int n, int m) {
	//loger << "f";
	double min_dist = media->dist2border(this);
	Media *nearest = media->overmedia;
	for (auto &i : media->submedia) {
		double cur = i->dist2border(this);
		if (cur > 0 && cur < min_dist) {
			min_dist = cur;
			nearest = i;
		}
	}
	//loger << min_dist<<"###"<< endl;
	if (loger.str().size() > 100000) loger.str("");
	if (min_dist < 0) {
		//loger << "QQQQ" << endl;
		media->dist2border(this);
	}
	assert(min_dist > 0 || ((cout << pos[0] << ' ' << pos[1] << ' ' << pos[2] << ' ' << endl) && false));
	double border_t = t - min_dist / media->v;
	if (border_t < 0) {
		border_t = 0;
		min_dist = media->v * t;
	}
	arrayd border_pos = pos - dir * min_dist;
	double optical_dist = exp(-media->mu * min_dist);
	sp<BorderPoint> border_point(new BorderPoint(border_t, border_pos, dir, nearest, media));
	if (!n) {
		return optical_dist * media->border(border_point);
	}
	double sumI = 0;
	if (m) {
		arrayd Q = rorate2array_matrix(dir);
		for (int i = 0; i < m; i++) {
			double rand_dist = exp_rand(media->mu, min_dist - EPS);
			arrayd new_pos = pos - dir * rand_dist;
			arrayd new_dir = Matmul(Q, media->rand_dir());
			/*print3d(pos);
			print3d(new_pos);
			print3d(new_dir);
			cout <<t << ' ' << t - rand_dist / media->v << ' '<< min_dist << ' ' << rand_dist << "+++++" << endl;*/
			sumI += Point(t - rand_dist / media->v, new_pos, new_dir, media).f(n - 1, m);
		}
		sumI = sumI / m * (1 - optical_dist) * media->mu_s / media->mu;
	}
	//cout << media->border(border_point) << "----" << optical_dist << "----" << min_dist << endl;
	return (nearest->border(border_point) + border_point->f(n - 1, m)) * optical_dist + sumI;
}

BorderPoint::BorderPoint(double t, arrayd pos, arrayd dir, Media *media_from, Media *media_to):
	Point(t, pos, dir, media_from), media_to(media_to) {
}

double BorderPoint::f(int n, int m) {
	if (t < EPS || media->is_global()) {
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
	//cout << "R" << endl;
	double partR = Point(t, pos, dirR, media_to).f(n, m);
	if (1 - k * k * (1 - cosAfter) < 0) {
		return partR;
	}
	double cosT = sqrt(1 - k * k * (1 - cosAfter));
	arrayd dirT = cosT * normal + k * (dir - cosAfter * normal);
	
	double a = (k * cosT + cosAfter),
		b = (cosT + k * cosAfter),
		c = 2. * cosT,
		Rpar = (k * cosT - cosAfter) / a,
		Rper = (cosT - k * cosAfter) / b,
		Tpar = c / a,
		Tper = c / b,
		R = 1. / 2. * (Rpar * Rpar + Rper * Rper),
		T = 1. / 2. * (Tpar * Tpar + Tper * Tper) * k / cosT;
	if (dot(dirT, normal) < 0.0001 || dot(dirR, normal) > 0) {
		double a = dot(dirT, normal), b = dot(dirR, normal);
	}
	//cout << "T: " <<  T << ' ' << R << endl;
	return R * partR + T * Point(t, pos, dirT, media).f(n, m);
}

Point::~Point()
{
}
