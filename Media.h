#pragma once
#include "Point.h"
#include <vector>
#include <iostream>
using namespace std;

struct Media
{
	double v, mu, mu_s, k, (*indic)();
	arrayd rand_dir();
	vector<Media *> submedia;
	Media *overmedia;
	Media(double v, double mu, double mu_s, double k, double(*indic)());
	void add_submedia(Media * sub);
	virtual double dist2border(Point *x) { cerr << "using pure virtual"; return 0; }
	virtual arrayd normal(arrayd x) { cerr << "using pure virtual"; return arrayd(0); }
	virtual bool is_global() = 0;
	virtual double border(sp<BorderPoint> x) { return 0; }
};

struct SphereMedia: Media {
	arrayd c;
	double r;
	arrayd normal(arrayd x) { arrayd ret = x - c; return ret / norm(ret); }
	double dist2border(Point *x);
	SphereMedia(double v, double mu, double mu_s, double k, double r, arrayd c, double(*indic)());
	bool is_global() { return false; }
};

struct GlobalMedia : SphereMedia {
	double (*_border)(sp<BorderPoint> x);
	virtual double border(sp<BorderPoint> x) { return _border(x); }
	static GlobalMedia* instance(double(*b)(sp<BorderPoint> x) = 0) {
		static GlobalMedia inst;
		if (b) {
			inst._border = b;
		}
		return &inst;
	}
	bool is_global() { return true; }
private:
	GlobalMedia(): SphereMedia(0., 0., 0., 0., 0., make_array3d(0, 0, 0),[]() { return 0.; }) {}
};
