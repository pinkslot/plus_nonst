#include "Media.h"

Media::Media(double v, double mu, double mu_s, double k, double(*border)(sp<BorderPoint> x), double(*indic)()) :
v(v), mu(mu), mu_s(mu_s), k(k), border(border), indic(indic), overmedia(0)
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

SphereMedia::SphereMedia(double v, double mu, double mu_s, double k, double r, arrayd c, 
	double(*border)(sp<BorderPoint> x), double(*indic)()) :
	Media(v, mu, mu_s, k, border, indic), r(r), c(c)
{
}

double SphereMedia::dist2border(Point *x){			
		// dist to 1st border in -this->dir(minus dirictional)!
	arrayd c_pos = c - x->pos;
	double norm_c_pos = norm(c_pos);
	if (norm_c_pos < EPS) {
		return r;
	}
	double b = dot(x->dir, c_pos),
		c = norm_c_pos * norm_c_pos - r * r,
		D = b * b - c;
	if (D > EPS) {
		D = sqrt(D);
		return -b - D > EPS ? -b - D : -b + D > EPS ? -b + D : -1;
	}
	return -1;
}

