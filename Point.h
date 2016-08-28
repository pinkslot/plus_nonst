#pragma once
#include "Utils.h"
struct Media;
using namespace std;

struct Point {
	arrayd pos, dir;
	double t;
	Media *media;
	Point(double t, arrayd pos, arrayd dir, Media * media);
	double f(int n);
	~Point();
};

struct BorderPoint: Point {
	Media *media_to;
	BorderPoint(double t, arrayd pos, arrayd dir, Media *media_from, Media *media_to);
	double f(int n);
};

extern double avg_n;
extern int counter;
#include "Media.h"
