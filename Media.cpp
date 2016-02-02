#include "Media.h"
#include <assert.h>

Media::Media(double v, double mu, double mu_s, double k, double(*indic)()):
	v(v), mu(mu), mu_s(mu_s), k(k), indic(indic), overmedia(0)
{
}

arrayd Media::rand_dir() {
	double phi = randf() * 2. * M_PI,
		cosi = indic(),
		sini = sqrt(1. - cosi * cosi);
	return make_array3d(cos(phi) * sini, sin(phi) * sini, cosi);
}

void Media::add_submedia(Media * s) {
	submedia.push_back(s);
	s->overmedia = this;
}

double SphereMedia::ext_dist(Point * x) {
	arrayd c_pos = c - x->pos;
	double norm_c_pos = norm(c_pos);
//	assert(x->media != this && norm_c_pos > r - EPS);
	double b = dot(x->dir, c_pos),
		c = norm_c_pos * norm_c_pos - r * r,
		D = b * b - c;
	if (D > EPS) {
		double ans = -b - sqrt(D);
		return ans > EPS ? ans : -1;
	}
	return -1;
}

double SphereMedia::int_dist(Point * x) {
	arrayd c_pos = c - x->pos;
	double norm_c_pos = norm(c_pos);
//	assert(x->media == this && norm_c_pos < r + EPS);
	double b = dot(x->dir, c_pos),
		c = norm_c_pos * norm_c_pos - r * r,
		D = b * b - c;
//	assert(D >= 0|| cout << D && false);
	double ans = -b + sqrt(D);
//	assert(-b - sqrt(D) < EPS*100 && ans >= 0 || cout << ans << ' ' << -b - sqrt(D) && false);
	return -b + sqrt(D);
}

SphereMedia::SphereMedia(double v, double mu, double mu_s, double k,
	double r, arrayd c, double(*indic)()) :
	Media(v, mu, mu_s, k, indic), r(r), c(c)
{
}

