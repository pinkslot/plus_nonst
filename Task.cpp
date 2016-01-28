#include "Task.h"
#include "Utils.h"
#include <atlimage.h>
#include <fstream>
#include <ctime>

Task::Task()
{
}

void Task::go() {
	CImage img;
	int tt = 0;
	//double qw = Point(15.5, make_array3d(-1.5, .5, 2), make_array3d(0, 0, 1), G).f(n, m);
	system((string("rm -r ") + task_name).c_str());
	system((string("mkdir ") + task_name).c_str());
	clock_t start = clock();

	for (double t = min_time; t < max_time; t += time_step, tt++) {
		char img_fname[20], log_fname[20];
		//loger << "QQQQQQQQQQQ" << tt<<endl;
		sprintf_s(img_fname, "%s/%02i.bmp", task_name.c_str(), tt);
		//ofstream os(log_fname);
		sprintf_s(log_fname, "%s/%02i.log", task_name.c_str(), tt);
		img.Create(res, res, 24);
		double i = -size2;
		for (auto y = 0; y < res; i += step, y++) {
			double j = -size2;
			for (auto x = 0; x < res; j += step, x++) {
				double f = Point(t, make_array3d(j, i, z_screen), make_array3d(0, 0, 1), G).f(n, m);
				//cout << "t" << t << "y" << y << "x" << x << endl << "------------";
			//	os << f;
				int val = no_more255(color_mul * f);
				img.SetPixel(x, y, RGB(val, val, val));
			}
		}
		img.Save(img_fname);
		img.Destroy();
		cout << "DONE " << t << "\tin " << (clock() - start ) / (double)CLOCKS_PER_SEC << endl;
 	}
}

void Task1::init() {
	// ADD BEGINING COND
	Media *G1 = new GlobalMedia(up2w);
	G = new SphereMedia(1., .01, .001, 4., 10., make_array3d(0, 0, 0), [](sp<BorderPoint> x) { return 0.; }, []() { return 1.; });
	G1->add_submedia(G);
	G->add_submedia(new SphereMedia(1., .1, .1, 1., 1., make_array3d(1.5, 0, 0), [](sp<BorderPoint> x) { return 0.; }, []() { return 1.; }));
	G->add_submedia(new SphereMedia(1., .1, .001, 16., 1., make_array3d(-1.5, 0, 0), [](sp<BorderPoint> x) { return 0.; }, []() { return 1.; }));
	G->add_submedia(new SphereMedia(1., .1, .001, 16., .3, make_array3d(-1.5, 2, 0), [](sp<BorderPoint> x) { return 0.; }, []() { return 1.; }));

	res = 200, color_mul = 150;

	n = 10, m = 1;
	size = 6., time_step = .3, min_time = 3, max_time = 15, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = string("first");
}

void Task2::init() {
	Media *G1 = new GlobalMedia([](sp<BorderPoint> x) { return x->t > EPS ? 1. : 0.; });
	G = new SphereMedia(1., .001, .001, 4., 10., make_array3d(0, 0, 0), [](sp<BorderPoint> x) { return 0.; }, []() { return 1.; });
	G1->add_submedia(G);
	G->add_submedia(new SphereMedia(1., .1, .1, 1., 1., make_array3d(0, 0, 0), [](sp<BorderPoint> x) { return 0.; }, []() { return 1.; }));

	res = 200, color_mul = 150;

	n = 10, m = 1;
	size = 6., time_step = .5, min_time = 3, max_time = 40, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = string("sec");
}





