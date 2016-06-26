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

	Screen sc(res, size, { 0, 15, z_screen }, {0, 15, z_screen}, G);
	int tt = 0;
	for (double t = min_time; t < max_time; t += time_step, tt++) {
		const int kk_per_frame = k;
		for (int kk = 0; kk < k; kk += kk_per_frame) {
			int i = 0;
			while(sc.next(t)) {
				double &f = img_data[i++], &ff = img_data[i++];
				//if (!(kk &&(f < EPS || sigma(f, ff, kk) / sqrt(kk) < .05))) {
					for (int pixkk = kk; pixkk < kk + kk_per_frame; pixkk++) {
						if (pixkk > 10 && f < 1e-5)
							break;
						double res_f = sc.cur->f(n);
						f = double(pixkk) / (pixkk + 1) * f + res_f / (pixkk + 1);
						//ff = double(pixkk) / (pixkk + 1) * ff + sqr(res_f) / (pixkk + 1);
						/*if (f != f) {
							f = 0;
							throw NanResult();
						}*/
					}
				//}
			}
			
			i = 0;
			//int skip_count = 0;
			for (auto y = 0; y < res; y++) {
				for (auto x = 0; x < res; x++) {
					double &f = img_data[i++], &ff = img_data[i++];
					int val = no_more255(f * color_mul);
					img.SetPixel(x, y, RGB(val, val, val));
					/*
					opt_image.SetPixel(x, y, RGB(val, val, val));
					if (!(kk && (f < EPS || sigma(f, ff, kk) / sqrt(kk) < .05))) {
						opt_image.SetPixel(x, y, RGB(0, 0, 255));
						skip_count++;
					}
					*/
				}
			}
			char img_fname[30], opt_img_name[30];
			sprintf_s(img_fname, "%s/%02i_%02i.bmp", task_name.c_str(), tt, kk);
			img.Save(img_fname);
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

	Screen sc(res, size, { 0, 15, z_screen }, { 0, 15, z_screen }, G);
	for (double t = min_time; t < max_time; t += time_step, tt++) {
		vector<Point> ps = { Point(t, make_array3d(0, 10., z_screen), make_array3d(0, 0, 1.), G),
		Point(t, make_array3d(0, -10., z_screen), make_array3d(0, 0, 1.), G) };
		char log_fname[20];
		sprintf_s(log_fname, "%s/%02i.log", task_name.c_str(), tt);
		ofstream out(log_fname);
		for (int n = 18; n < 21; n += 2) {
			for (auto p : ps) {
				clock_t start = clock();
				double ff = 0;
				double f = 0;
				const int count = 100;
				for (int l = 0; l < count; l++) {
					double val = p.f(n);
					f = double(l) / (l + 1) * f + val / (l + 1);
					ff = double(l) / (l + 1) * ff + val * val / (l + 1);
					//cout << "\r    \r" << l;
					cout << '.';
				}
				out << scientific;
				ff = sigma(f, ff, count);
				out << ":nn = " << n << "\tf = " << f << "\tdf = " << ff <<
					"\trel_df = " << ff / f << "\tt = " << (clock() - start) / (double)CLOCKS_PER_SEC / count << endl;
				cout << endl;
			}
		}
		cout << '\xd' << tt << ")DONE " << t << " \tin " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
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
			if (i % 1000 == 0) cout << "\r         " << '\xd' << i / 1000 << '/' << k / 1000;
		}
		double avg_time = (clock() - start) / (double)CLOCKS_PER_SEC / k;
		double Df = double(k) / (k - 1) *(Ef2 - Ef * Ef);
		cout << "\r         " << '\xd' << n << '\t'; log_out << n << '\t';
		print_to_streams({Ef, Ef / ans(TIME) - 1, Df , Df / Ef, Df * avg_time});
	}
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

void Task2::init() {
	G = new SphereMedia(.1, .1, 1., 10., make_array3d(0, 0, 0), Indic_Direct);
	GlobalMedia::instance(up2w_pos_dir, 1)->add_submedia(G);
	G->add_submedia(new SphereMedia(.4, .4, 1.1, 5, make_array3d(0, 0, 0), Indic_isotrophy));
	G->add_submedia(new SphereMedia(.1, .1, 10., 1., make_array3d(-7, 0, 0), Indic_isotrophy));
	G->add_submedia(new SphereMedia(.1, .1, 10., 1, make_array3d(5, -5, 0), Indic_isotrophy));

	res = 500, color_mul = 200;

	n = 5, k = 100;
	size = 20, time_step = 3, min_time = 10, max_time = 40, z_screen = 6,
		size2 = size / 2, step = size / res;
	task_name = "sec";
	go();
}
*/
arrayd ww = {1, 0, 0};
double border(sp<BorderPoint> x) {
	return x->t > EPS ?
		//		exp(-sqr(x->t/20. - 100.)) *
		3 * exp(-(norm(x->dir + ww) + norm(x->pos / norm(x->pos) - ww))) :
		0;
}

void Task3::init() {
	auto ind = indic_henyey<2>;
	G = new SphereMedia(.001, .001, 3., 100., make_array3d(0, 0, 0), ind);
	GlobalMedia::instance(border)->add_submedia(G);
	G->add_submedia(new SphereMedia(1, 1, 3., 11, make_array3d(0, 15, 0), indic_isotrophy));
	G->add_submedia(new SphereMedia(3, 6, 3., 11, make_array3d(0, -15, 0), indic_isotrophy));

	res = 100, color_mul = 170;
	n = 7, k = 10;
	size = 50., time_step = 50, min_time = 2360, max_time = 3000, z_screen = 50,
		size2 = size / 2, step = size / res;
	task_name = "trd5";
	point_go();
}

void TwoSphere::init() {
	auto ind = indic_henyey<4, 3>;
	G = new SphereMedia(.0001, .0001, 3., 100., make_array3d(0, 0, 0), ind);
	GlobalMedia::instance(border)->add_submedia(G);
	G->add_submedia(new SphereMedia(.5, .5, 3., 13, make_array3d(0, 0, 0), indic_isotrophy));
	G->add_submedia(new SphereMedia(.3, .3, 30., 4, make_array3d(15, 0, -15), indic_isotrophy));
	G->add_submedia(new SphereMedia(.3, .3, 30., 4, make_array3d(-15, 0, 15), indic_isotrophy));

	res = 300, color_mul = 200;
	n = 7, k = 20;
	size = 50., time_step = 20, min_time = 430, max_time = 3000, z_screen = 50,
		size2 = size / 2, step = size / res;
	task_name = "two_sphere";
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
	n = 7, k = 5;
	size = 50., time_step = 5, min_time = 2400, max_time = 3000, z_screen = 50,
		size2 = size / 2, step = size / res;
	task_name = "Refraction";
	go();
}
