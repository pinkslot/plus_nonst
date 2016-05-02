#include "Task.h"
#include "Utils.h"
#include <atlimage.h>
#include <fstream>
#include <ctime>

Task::Task()
{
}

bool Task::make_folder() {
	system((string("rm -r ") + task_name).c_str());
	return !system((string("mkdir ") + task_name).c_str());
}

void Task::go() {
	if (!make_folder()) return;
	CImage img;
	int tt = 0;
	clock_t start = clock();
	cout << "start on " << task_name << endl;
	double *img_data = new double[res * res];
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
				reset_rand();
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
		reset_rand();
		}
		cout << t << ' ' << f
			<< " with " << counter / k << " steps" << endl;
		counter = 0;
	}
	cout << "DONE " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
}

double ans(double t) {
	return exp(-t / 10.);
}
double exp_J(sp<Point> x) {
	return (x->media->mu - x->media->mu_s - 1. / 10. / x->media->v) * ans(x->t);
}
ofstream log_out;
void print_to_streams(vector<double> v) {
	for (auto &i : v) {
		cout << i << '\t';
		log_out << i << '\t';
	}
	cout << endl; log_out << endl;
}

void Task::calc_stat() {
	cout << "start on " << task_name << endl;
	char log_fname[20];
	sprintf_s(log_fname, "%s.log", task_name.c_str());
	log_out = ofstream(log_fname);
	(cout << std::scientific).precision(3);
	(log_out << std::scientific).precision(3);
	string header("n\t f\t\t eps\t\t delf\t\t r_delf\t\t complex\n");
	cout << header;
	log_out << header;

	double TIME = 10;
	for (int n = 2; n < 100; n++) {
		Point * p = new Point(TIME, make_array3d(1., 0., z_screen), make_array3d(0, 0, 1), G);
		clock_t start = clock();
		double Ef = 0, Ef2 = 0;
		for (int i = 0; i < k; i++) {
			double f = p->f(n);
			Ef = double(i) / (i + 1) * Ef + f / (i + 1);
			Ef2 = double(i) / (i + 1) * Ef2 + f * f / (i + 1);
			if (i % 1000 == 0) cout << '\xd' << i / 1000 << '/' << k / 1000;
		}
		double avg_time = (clock() - start) / (double)CLOCKS_PER_SEC / k;
		double Df = double(k) / (k - 1) *(Ef * Ef - Ef2);
		cout << '\xd' << n << '\t'; log_out << n << '\t';
		print_to_streams({Ef, Ef / ans(TIME) - 1, Df , Df / Ef, Df * avg_time});
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
	G = new SphereMedia(4, 2, 1., 10., make_array3d(0, 0, 0), indic_direct, exp_J);
	GlobalMedia::instance([](sp<BorderPoint> x) { return ans(x->t); }, 1)->add_submedia(G);
	G->add_submedia(new SphereMedia(2, 1, 4., 4., make_array3d(0, 0, 0), indic_direct, exp_J));

	res = 200, color_mul = 150;
	n = 5, k = 1e4;
	size = 5., time_step = 1, min_time = 2, max_time = 50, z_screen = 4,
		size2 = size / 2, step = size / res;
	task_name = string("fifth");
	calc_stat();
}



