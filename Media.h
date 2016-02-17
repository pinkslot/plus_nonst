#pragma once
#include "Point.h"
#include <vector>
#include <iostream>
using namespace std;

struct GlobalMedia;
struct Media
{
	double v, mu, mu_s, k, (*indic)();
	arrayd rand_dir();
	vector<Media *> submedia;
	Media *overmedia;
	Media(double mu, double mu_s, double k, double(*indic)(), double(*intern)(sp<Point> x) = take_point_ret_zero);
	void add_submedia(Media * sub);
	virtual double ext_dist(Point *x) { cerr << "using pure virtual"; return 0; }
	virtual double int_dist(Point *x) { cerr << "using pure virtual"; return 0; }
	virtual arrayd normal(arrayd x) { cerr << "using pure virtual"; return arrayd(0); }
	virtual bool is_global() = 0;
	virtual double border(sp<BorderPoint> x) = 0;
	double (*intern)(sp<Point> x);
};

struct GlobalMedia : Media {
	double (*_border)(sp<BorderPoint> x);
	virtual double border(sp<BorderPoint> x) { return _border(x); }
	int m;
	static GlobalMedia* instance(double(*b)(sp<BorderPoint> x) = 0, int m = 1) {
		static GlobalMedia inst;
		if (b) {
			inst._border = b;
			inst.m = m;
		}
		return &inst;
	}
	bool is_global() { return true; }
private:
	GlobalMedia(): Media(0., 0., 0., indic_direct) {}
};

struct SphereMedia: Media {
	arrayd c;
	double r;
	arrayd normal(arrayd x) { arrayd ret = x - c; return ret / norm(ret); }
	virtual double ext_dist(Point *x);
	virtual double int_dist(Point *x);
	SphereMedia(double mu, double mu_s, double k, double r, arrayd c, double(*indic)(), double(*intern)(sp<Point> x) = take_point_ret_zero);
	bool is_global() { return false; }
	virtual double border(sp<BorderPoint> x) { return x->t > EPS ? 0 : GlobalMedia::instance()->border(x); }
};
