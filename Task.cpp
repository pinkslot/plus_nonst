#include "Task.h"
#include "Utils.h"
#include <atlimage.h>
#include <fstream>
#include <ctime>
#include <iomanip>
#include "Screen.h"

bool Task::make_folder() {
	system((string("rm -rf ") + task_name).c_str());
	return !system((string("mkdir ") + task_name).c_str());
}

inline double sigma(double f, double sqrf, int count) {
	if (count <= 1)
		return 1. / EPS;
	return sqrt((sqrf - f*f) * count / (count - 1));
}

void Task::go() {
	if (!make_folder()) return;
	CImage img, opt_image;
	img.Create(res, res, 24), opt_image.Create(res, res, 24);
	clock_t start = clock();
	cout << "start on " << task_name << endl;
	double *img_data = new double[res * res * 2];

	Screen sc(res, size, { 0, 0, z_screen }, { 0, 0, z_screen}, G);
	int tt = 0;
	for (double t = min_time; t < max_time; t += time_step, tt++) {

		for (int old_kk = 0, kk = 5; kk < k; old_kk = kk, kk *= 2) {
			int i = 0;
			while(sc.next(t)) {
				double &f = img_data[i++], &ff = img_data[i++];
				//if (!(kk &&(f < EPS || sigma(f, ff, kk) / sqrt(kk) < .05))) {
				for (int pixkk = old_kk; pixkk < kk; pixkk++) {
					if (pixkk > 1e4 && f < 1e-5)
						break;
					double res_f = sc.cur->f(n);
					f = double(pixkk) / (pixkk + 1) * f + res_f / (pixkk + 1);
					//ff = double(pixkk) / (pixkk + 1) * ff + sqr(res_f) / (pixkk + 1);
					/*if (f != f) {
						f = 0;
						throw NanResult();
					}*/
				}

			}
		//}
			
			i = 0;
			//int skip_count = 0;
			for (auto y = 0; y < res; y++) {
				for (auto x = 0; x < res; x++) {
					double &f = img_data[i++], &ff = img_data[i++];
					int val = no_more255(f * color_mul);
					img.SetPixel(x, y, RGB(val, val, val));
					if (x == res / 2 && y == res / 2) {
						img.SetPixel(x, y, RGB(255, 0, 0));
					}
					/*
					opt_image.SetPixel(x, y, RGB(val, val, val));
					if (!(kk && (f < EPS || sigma(f, ff, kk) / sqrt(kk) < .05))) {
						opt_image.SetPixel(x, y, RGB(0, 0, 255));
						skip_count++;
					}
					*/
				}
			}
			char img_fname[30], log_fname[30];
			cout << endl << endl;
			//sprintf_s(img_fname, "%s/%02i_%04i.bmp", task_name.c_str(), tt, kk);
			//sprintf_s(log_fname, "%s/%02i_%02i.log", task_name.c_str(), tt, kk);

			sprintf_s(img_fname, "%s/%04i.bmp", task_name.c_str(), kk);
			sprintf_s(log_fname, "%s/%04i.log", task_name.c_str(), kk);
			ofstream out(log_fname);

			img.Save(img_fname);
			out << '\xd' << "t = "<< tt << ")DONE " << t << " k = " << kk << " \tin " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;

			/*
			opt_image.Save(opt_img_name);
			sprintf_s(opt_img_name, "%s/%02i_%02i_opt.bmp", task_name.c_str(), tt, kk);
			cout << "\nskiped: " << double(skip_count) / res / res * 100 << "%\n";
			if (double(skip_count) / res / res > 0.95) {
				break;
			}*/
		}
		cout << '\xd' << tt << ")DONE " << t << " \tin " << (clock() - start ) / (double)CLOCKS_PER_SEC << endl;
 	}
	img.Destroy();
}

void Task::point_go() {
	if (!make_folder()) return;
	int tt = 0;
	clock_t start = clock();
	cout << "start on " << task_name << endl;

	//for (int n = 8; n < 9; n += 5) 
	{
	char log_fname[20];
	sprintf_s(log_fname, "%s/%02i.log", task_name.c_str(), n);
	ofstream out(log_fname);
	for (double t = min_time; t < max_time; t += time_step, tt++) {
		vector<Point> ps = { Point(t, make_array3d(0, 0, z_screen), make_array3d(0, 0, 1.), G),
			//Point(t, make_array3d(0, -10., z_screen), make_array3d(0, 0, 1.), G) 
		};
			for (auto p : ps) {
				clock_t start = clock();
				double ff = 0;
				double f = 0;
				for (int l = 0; l < k; l++) {
					double val = p.f(n);
					f = double(l) / (l + 1) * f + val / (l + 1);
					ff = double(l) / (l + 1) * ff + val * val / (l + 1);
					if (l % 1000 == 0) cout << "\r    \r" << l / 1000;
				}
				out << scientific;	
				cout << scientific;
				ff = sigma(f, ff, k);
				double time_per_val = (clock() - start) / (double)CLOCKS_PER_SEC / k;
				out << "t = " << t << " nn = " << n << "\tf = " << f << "\tdf = " << ff <<
					"\trel_df = " << ff / f << "\tcalc_t = " << time_per_val <<
					"\tcompl: " << ff * time_per_val << endl;
				cout << "t = " << t << " nn = " << n << "\tf = " << f << "\tdf = " << ff <<
					"\trel_df = " << ff / f << "\tcalc_t = " << time_per_val  <<
					"\tcompl: " << ff * time_per_val << endl;
			}
		cout << '\xd' << tt << ")DONE " << t << " \tin " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
		cout << "avg_n: " << avg_n << endl;
		counter = 0, avg_n = 0;
	}
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

/*
void Task1::init() {
	// ADD BEGINING COND
	G = new SphereMedia(.001, .001, 3., 10., make_array3d(0, 0, 0), Indic_isotrophy);
	GlobalMedia::instance(up2w_pos_dir)->add_submedia(G);
	G->add_submedia(new SphereMedia(.01, .01, 30., 1., make_array3d(1.2, 1, 0), Indic_isotrophy));
	G->add_submedia(new SphereMedia(.01, .01, 30., 1., make_array3d(-1.2, 0, 0), Indic_isotrophy));
	G->add_submedia(new SphereMedia(.01, .01, 30., 1., make_array3d(1.2, -1.5, 0), Indic_isotrophy));
	//	G->add_submedia(new SphereMedia(.001, .001, 1., .2, make_array3d(0, 0, 0), Indic_isotrophy, [](sp<Point> x) { return 1.; } ));

	res = 100, color_mul = 170;
	 
	n = 5, k = 5;
	size = 5., time_step = 5, min_time = 20, max_time = 200, z_screen = 3,
		size2 = size / 2, step = size / res;
	task_name = "fst";
	go();
}


*/
arrayd ww1 = {0,0, -1 }, ww = normalize(ww1);

double border(sp<BorderPoint> x) {
	return x->t > EPS ?
		exp(-2 * (norm(x->dir + ww) + norm(normalize(x->pos) - ww)) + 4):
		0;
}

void Task2::init() {
	G = new SphereMedia(1e-6, 1e-6, 1, 1., make_array3d(0, 0, 0), indic_isotrophy);
	GlobalMedia::instance(border)->add_submedia(G);
	G->add_submedia(new SphereMedia(1e-6, 1e-6, 1.5, 0.1, make_array3d(0, 0, 0), indic_isotrophy))
		->add_submedia(new SphereMedia(1e-6, 1e-6, .1, 0.04, make_array3d(0, 0, 0), indic_isotrophy));
	G->add_submedia(new SphereMedia(1e-6, 1e-6, .1, 0.04, make_array3d(0.2, 0, 0), indic_isotrophy));
	res = 200, color_mul = 200;
	n = 20, k = 1e3;
	size = .5, time_step = 2, min_time = 10, max_time = 11, z_screen = .7,
		size2 = size / 2, step = size / res;
	task_name = "qweqwe";
	go();
}
arrayd ee1 = { 0,0, -1 }, ee = normalize(ee1);
double border1(sp<BorderPoint> x) {
	return 2 - norm(x->pos - ee);
}
void Task3::init() {

	G = new SphereMedia(6, 6, 1, 1., make_array3d(0, 0, 0), indic_isotrophy);
	GlobalMedia::instance(border1)->add_submedia(G);
	G->add_submedia(new SphereMedia(6, 6, 2, 100, make_array3d(0, 0, -100), indic_isotrophy));

	res = 100, color_mul = 255/2;
	n = -9, k = int(1e4);
	size = 1.5, time_step = 5, min_time = 100, max_time = 101, z_screen = 0.6,
		size2 = size / 2, step = size / res;
	task_name = "simple";
	point_go();
}

arrayd qq1 = { 0 , 1, 0 }, qq = normalize(qq1);
double border2(sp<BorderPoint> x) {
	return x->t > EPS ?
		exp(-4 * (norm(x->dir + qq) /*+  norm(normalize(x->pos) - qq)*/) + 4) :
		//		exp(-sqr(x->t/20. - 100.)) *
		0;
}	

void TwoSphere::init() {
//	auto ind = indic_henyey<4, 3>;
	G = new SphereMedia(1e-6, 1e-6, 3., 100., make_array3d(0, 0, 0), indic_isotrophy);
	GlobalMedia::instance(border2)->add_submedia(G);
	
	//G->add_submedia(new SphereMedia(1, 1, 300., 1000., make_array3d(0, -1000 - 14., 0), indic_isotrophy));
	G->add_submedia(new SphereMedia(3e-2, 1e-2, 4, 15, make_array3d(0, 0, 0), indic_isotrophy))
		->add_submedia(new SphereMedia(.2, .1, .01, 5, make_array3d(0, 2, -2), indic_isotrophy));
	//G->add_submedia(new SphereMedia(.2, .1, 4, 20, make_array3d(0, 0, 0), indic_isotrophy));
	/*G->add_submedia(new SphereMedia(1, 1, 300., 4, make_array3d(15, 0, 15), indic_isotrophy));
	G->add_submedia(new SphereMedia(1, 1, 300., 4, make_array3d(-15, 0, -15), indic_isotrophy));
	G->add_submedia(new SphereMedia(1, 1, 300., 4, make_array3d(7.5, 17, 0), indic_isotrophy));
	*/
	res = 200, color_mul = 170;
	n = 7, k = int(1e4);
	size = 50., time_step = 10, min_time = 3500, max_time = 3501, z_screen = 50,
		size2 = size / 2, step = size / res;
	task_name = "qweqwe";
	go();
}

void Multiply::init() {
	auto ind = indic_henyey<3>;
	SphereMedia *G1 = new SphereMedia(0.001, 0.001, 3., 10., make_array3d(0, 0, 0), ind);
	G = G1;
	GlobalMedia::instance(up2w_pos_dir)->add_submedia(G);
	G1->add_submedia(new SphereMedia(0.3, 3, 4, 2., make_array3d(0,0,0), ind));
	for (int i = 0; i < 50; i++) {
		arrayd nc = G1->c + make_rand_norm_3d() * (randf() - .5) * 10.;
		try {
			G1->add_submedia(new SphereMedia(0.3, randf() > .5 ? 0.5 : 2.5, 4, 2., nc, ind));
		}
		catch (AddSubmediaError) {
		}
	}
	res = 200, color_mul = 150;
	n = 15, k = int(1e3);
	size = 5., time_step = 5, min_time = 50, max_time = 10000, z_screen = 4,
		size2 = size / 2, step = size / res;
	task_name = "multiply";
	point_go();
}

double intern(shared_ptr<Point> x) {
	return 0.04;
}

void ManySphere::init() {
	auto ind = indic_henyey<3>;
	G = new SphereMedia(.0001, .0001, 3., 100., make_array3d(0, 0, 0), ind);
	GlobalMedia::instance(up2w_pos_dir)->add_submedia(G);
	G->add_submedia(new SphereMedia(.001, .001, 30., 1000., make_array3d(0, -1000., 0), ind));

	for (int i = -40; i <= 40; i+=10) {
		for (int j = -40; j <= 40; j+=10) {
			if (i == 0 && j == 0 || i == 10 && j == 0 || i == -10 && j == 10 || i == -10 && j == 0 || i == 0 && j == 10) {
				Media *n = randf() > .5 ? 
					new SphereMedia(.4, .4, 3., 5,
						make_array3d(i + j * 2. / 3., 5.5, j - i / 3.),
						ind):
					new SphereMedia(.0001, .0001, 50., 5,
						make_array3d(i + j * 2. / 3., 5.5, j - i / 3.),
						ind);

				G->add_submedia(n);
				continue;
			}
		}
	}
	
	res = 300, color_mul = 210;
	n = 7, k = 50;
	size = 35., time_step = 15, min_time = 2250, max_time = 3000, z_screen = 35.,
		size2 = size / 2, step = size / res;
	task_name = "ManySphere";
	go();
}

void Refraction::init(){
	auto ind = indic_henyey<3>;
	G = new SphereMedia(.001, .001, 3., 100., make_array3d(0, 0, 0), ind);
	GlobalMedia::instance(up2w_pos_dir)->add_submedia(G);
	G->add_submedia(new SphereMedia(.001, .001, 4., 13, make_array3d(15, 0, 0), ind))->
		add_submedia(new SphereMedia(.01, .01, 30., 5, make_array3d(13, 1, 0), ind));
	G->add_submedia(new SphereMedia(.001, .001, 2., 13, make_array3d(-15, 0, 0), ind))->
		add_submedia(new SphereMedia(.01, .01, 30., 5, make_array3d(-13, 1, 0), ind));

	res = 200, color_mul = 170;
	n = 7, k = 500;
	size = 50., time_step = 5, min_time = 2400, max_time = 3000, z_screen = 50,
		size2 = size / 2, step = size / res;
	task_name = "R1";
	go();
}
