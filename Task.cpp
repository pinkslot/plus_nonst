#include "Task.h"
#include "Utils.h"
#include <atlimage.h>
#include <fstream>
#include <ctime>
#include <iomanip>

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
		sprintf_s(img_fname, "%s/%02i.bmp", task_name.c_str(), tt);
		sprintf_s(log_fname, "%s/%02i.log", task_name.c_str(), tt);
		img.Create(res, res, 24);
		double max_val = 0;
		double i = 0;
		for (auto y = res - 1; y >= 0; i += step/sqrt(2.), y--) {
			double j = -size2;
			for (auto x = 0; x < res; j += step, x++) {
				reset_rand();
				Point * p = new Point(t, make_array3d(j, 1 + i, z_screen - i), make_array3d(0, 1, 1), G);
				double f = 0;
				for (int kk = 0; kk < k; kk++) {
					double cur_f = p->f(n);
					f = double(kk) / (kk + 1) * f + cur_f / (kk + 1);
				}
				if (f != f) {
					f = 0;
					//throw NanResult();
				}
				img_data[y * res + x] = f;
				max_val = max(max_val, f);
				delete p;
			}
			cout << '\xd' << y;
		}
		for (auto y = res - 1; y >= 0; y--) {
			for (auto x = 0; x < res; x++) {
				int val = no_more255(255. * img_data[y * res + x] / max_val);
				img.SetPixel(x, y, RGB(val, val, val));
			}
		}
		img.Save(img_fname);
		img.Destroy();
		cout << '\xd' << "DONE " << t << " \tin " << (clock() - start ) / (double)CLOCKS_PER_SEC << endl;
 	}
}
void Task::point_go() {
	clock_t start = clock();
	cout << "point calc on " << task_name << endl;
	int tt = 0;
	ofstream out(task_name + ".log");
	for (double t = min_time; t < max_time; t += time_step, tt++) {
		reset_rand();
		Point * p = new Point(t, make_array3d(1.5, 1.5, z_screen), make_array3d(0, 1, 1), G);
		double f = 0;
		for (int kk = 0; kk < k; kk++) {
			double cur_f = p->f(n);
			f = double(kk) / (kk + 1) * f + cur_f / (kk + 1);
		}
		if (f != f) {
			throw NanResult();
		}
		(out << std::scientific).precision(5);
		out << f << endl;
		delete p;
		cout << '\xd' << "DONE " << t << " \tin " << (clock() - start) / (double)CLOCKS_PER_SEC << " f=" << f << endl;
	}
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
		double Df = double(k) / (k - 1) *(Ef2 - Ef * Ef);
		cout << '\xd' << n << '\t'; log_out << n << '\t';
		print_to_streams({Ef, Ef / ans(TIME) - 1, Df , Df / Ef, Df * avg_time});
	}
}

void Task1::init() {
	// ADD BEGINING COND
	G = new SphereMedia(.001, .001, 3., 10., make_array3d(0, 0, 0), indic_isotrophy);
	GlobalMedia::instance(up2w_pos_dir)->add_submedia(G);
	G->add_submedia(new SphereMedia(.01, .01, 30., 1., make_array3d(1.2, 1, 0), indic_isotrophy));
	G->add_submedia(new SphereMedia(.01, .01, 30., 1., make_array3d(-1.2, 0, 0), indic_isotrophy));
	G->add_submedia(new SphereMedia(.01, .01, 30., 1., make_array3d(1.2, -1.5, 0), indic_isotrophy));
	//	G->add_submedia(new SphereMedia(.001, .001, 1., .2, make_array3d(0, 0, 0), indic_isotrophy, [](sp<Point> x) { return 1.; } ));

	res = 100, color_mul = 170;
	 
	n = 5, k = 5;
	size = 5., time_step = 5, min_time = 20, max_time = 200, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = "fst";
	go();
}

void Task2::init() {
	G = new SphereMedia(.1, .1, 1., 10., make_array3d(0, 0, 0), indic_direct);
	GlobalMedia::instance(up2w_pos_dir, 1)->add_submedia(G);
	G->add_submedia(new SphereMedia(.4, .4, 1.1, 5, make_array3d(0, 0, 0), indic_isotrophy));
	G->add_submedia(new SphereMedia(.1, .1, 10., 1., make_array3d(-7, 0, 0), indic_isotrophy));
	G->add_submedia(new SphereMedia(.1, .1, 10., 1, make_array3d(5, -5, 0), indic_isotrophy));

	res = 500, color_mul = 200;

	n = 5, k = 100;
	size = 20, time_step = 3, min_time = 10, max_time = 40, z_screen = 6,
		size2 = size / 2, step = size / res;
	task_name = "sec";
	go();
}

void Task3::init() {
	G = new SphereMedia(4, 2, 1., 10., make_array3d(0, 0, 0), indic_isotrophy, exp_J);
	GlobalMedia::instance([](sp<BorderPoint> x) { return ans(x->t); }, 2)->add_submedia(G);
	G->add_submedia(new SphereMedia(2, 1, 4., 4., make_array3d(0, 0, 0), indic_isotrophy, exp_J));

	res = 200, color_mul = 150;
	n = 5, k = 1e4;
	size = 5., time_step = 1, min_time = 2, max_time = 50, z_screen = 4,
		size2 = size / 2, step = size / res;
	task_name = "third";
	calc_stat();
}

void TwoSphere::init() {
	SphereMedia *G1 = new SphereMedia(0.001, 0.001, 3., 10., make_array3d(0, 0, 0), indic_isotrophy);
	G = G1;
	GlobalMedia::instance(up2w_pos_dir)->add_submedia(G);
	for (int i = 0; i < 50; i++) {
		arrayd nc = G1->c + make_rand_norm_3d() * (randf() - .5) * 10.;
		try {
			G1->add_submedia(new SphereMedia(0.3, randf() > .5 ? 0.5 : 2.5, 4, 2., nc, indic_isotrophy));
		}
		catch (AddSubmediaError) {
		}
	}
	res = 400, color_mul = 170;

	n = 3, k = 3;
	size = 18, time_step = 20, min_time = 20, max_time = 1000, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = "two_sphere";
	go();
}

void Multiply::init() {
	SphereMedia *G1 = new SphereMedia(0.001, 0.001, 3., 10., make_array3d(0, 0, 0), indic_isotrophy);
	G = G1;
	GlobalMedia::instance(up2w_pos_dir)->add_submedia(G);
	G1->add_submedia(new SphereMedia(0.3, 3, 4, 2., make_array3d(0,0,0), indic_isotrophy));
	for (int i = 0; i < 50; i++) {
		arrayd nc = G1->c + make_rand_norm_3d() * (randf() - .5) * 10.;
		try {
			G1->add_submedia(new SphereMedia(0.3, randf() > .5 ? 0.5 : 2.5, 4, 2., nc, indic_isotrophy));
		}
		catch (AddSubmediaError) {
		}
	}
	res = 200, color_mul = 150;
	n = 15, k = 1e3;
	size = 5., time_step = 5, min_time = 50, max_time = 10000, z_screen = 4,
		size2 = size / 2, step = size / res;
	task_name = "multiply";
	point_go();
}

void ManySphere::init() {
	G = new SphereMedia(.001, .001, 3., 10., make_array3d(0, 0, 0), indic_isotrophy);
	GlobalMedia::instance(up2w_pos_dir)->add_submedia(G);
	G->add_submedia(new SphereMedia(.001, .001, 30., 100., make_array3d(0, -100., 0), indic_isotrophy));
	for (int i = -3; i < 4; i++) {
		for (int j = -3; j < 4; j++) {
			G->add_submedia(new SphereMedia(.001, .001, 10., .5, make_array3d(i, .55, j), indic_isotrophy));
		}
	}
	res = 200, color_mul = 170;
	n = 5, k = 5;
	size = 5., time_step = 5, min_time = 20, max_time = 200, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = "ManySphere";
	go();
}
