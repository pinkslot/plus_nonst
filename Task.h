#pragma once
#include "Media.h"

class Task
{
protected:
	Media * G;
	int n, m, k;
	double size, time_step, min_time, max_time, z_screen,
		size2, step;
	string task_name;

	int res, color_mul;
public:
	void go();
	void point_go();
	Task();
};


class Task1 : public Task{
public:
	Task1() {}
	void init();
};

class Task2 : public Task {
public:
	Task2() {}
	void init();
};

class Task3 : public Task {
public:
	Task3() {}
	void init();
};


void Task() {

}*/
