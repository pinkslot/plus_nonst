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
	system((string("rm -r ") + task_name).c_str());
	if (system((string("mkdir ") + task_name).c_str())) {
		return;
	}
	clock_t start = clock();
	cout << "start on " << task_name << endl;
	for (double t = min_time; t < max_time; t += time_step, tt++) {
		char img_fname[20], log_fname[20];
		//loger << "QQQQQQQQQQQ" << tt<<endl;
		sprintf_s(img_fname, "%s/%02i.bmp", task_name.c_str(), tt);
		sprintf_s(log_fname, "%s/%02i.log", task_name.c_str(), tt);
		img.Create(res, res, 24);
		double i = -size2;
		for (auto y = res - 1; y >= 0; i += step, y--) {
			double j = -size2;
			for (auto x = 0; x < res; j += step, x++) {
				Point * p = new Point(t, make_array3d(j, i, z_screen), make_array3d(0, 0, 1), G);
				double f = 0;
				for (int i = 0; i < k; i++) {
					f += p->f(n);
				}
				//loger.str("");
				//loger << "t" << t << "y" << y << "x" << x << endl << "------------";
				int val = no_more255(color_mul * f / k);
				img.SetPixel(x, y, RGB(val, val, val));
			}
		}
		img.Save(img_fname);
		img.Destroy();
		cout << "DONE " << t << " \tin " << (clock() - start ) / (double)CLOCKS_PER_SEC << endl;
 	}
}

void Task::point_go() {
	cout << "start on " << task_name << endl;
	clock_t start = clock();
	for (double t = min_time; t < max_time; t += time_step) {
		Point * p = new Point(t, make_array3d(0., 0., 0), make_array3d(0, 0, 1), G);
		double f = 0, df = 0;
		for (int i = 0; i < k; i++) {
			double ff = p->f(n);
			f += ff;
			df += ff * ff;
		}
		cout << t << ' ' << f
			<< " with " << counter / k << " steps" << endl;
		counter = 0;
	}
	cout << "DONE " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
}

void Task::calc_stat() {
	cout << "start on " << task_name << endl;

	int TIME = 10;
	for (double n = 2; n < 100; n++) {
		Point * p = new Point(TIME, make_array3d(0., 0., 5), make_array3d(0, 0, 1), G);
		clock_t start = clock();
		double f = 0, df = 0;
		for (int i = 0; i < k; i++) {
			double ff = p->f(n);
			f += ff;
			df += ff * ff;
			if (i % 1000 == 0) cout << '\xd' << i / 1000 << '/' << k / 1000;
		}
		double avg_time = (clock() - start) / (double)CLOCKS_PER_SEC / k;
		f /= k;
		df = sqrt((df - f * f * k) / (k - 1)) / f;

		cout << '\xd' <<  "f = " << f << " \teps=" << f / exp(TIME) - 1  << "\tdelf(" << n << ")= " << df << "\tcomplexity = " << df * avg_time<< endl;
	}
}
void Task1::init() {
	// ADD BEGINING COND
	G = new SphereMedia(1., .001, .001, 4., 10., make_array3d(0, 0, 0), []() { return 1.; });
	GlobalMedia::instance(up2w)->add_submedia(G);
	G->add_submedia(new SphereMedia(1., .1, .1, 1., 1., make_array3d(1.5, 0, 0), []() { return 1.; }));
	G->add_submedia(new SphereMedia(1., .1, .001, 16., 1., make_array3d(-1.5, 0, 0), []() { return 1.; }));
	G->add_submedia(new SphereMedia(1., .1, .001, 16., .3, make_array3d(-1.5, 2, 0), []() { return 1.; }));

	res = 200, color_mul = 100;

	n = 5, m = 1, k = 5;
	size = 6., time_step = .3, min_time = 3, max_time = 15, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = string("fst");
}

void Task2::init() {
	G = new SphereMedia(1., .001, .001, 1., 10., make_array3d(0, 0, 0), []() { return randf() / 2 + .5; });
	GlobalMedia::instance(up2w)->add_submedia(G);
	G->add_submedia(new SphereMedia(1., .5, .5, 5., 1., make_array3d(0, 0, -2), []() { return randf() / 2 + .5; }));

	res = 200, color_mul = 150;

	n = 5, m = 1, k = 5;
	size = 5., time_step = .5, min_time = 10, max_time = 30, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = string("sec");
}

void Task3::init() {
	G = new SphereMedia(1., .01, .01, 1., 10., make_array3d(2, 0, 0), []() { return 1.; });
	GlobalMedia::instance( [](sp<BorderPoint> x) { return 1.; })->add_submedia(G);
	G->add_submedia(new SphereMedia(1., .5, .5, 4., 1., make_array3d(0, 0, 0), []() { return 1.; }));
	res = 200, color_mul = 150;
	n = 15, m = 1, k = 1;
	size = 5., time_step = .5, min_time = 10, max_time = 30, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = string("trd");
}



