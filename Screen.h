#include "Utils.h"
#include "Media.h"

class Screen {
private:
	int res;
	double size;
	arrayd voffset, hoffset, normal, center, voffset_reshoffset;

	Media *G;
	int counter;

public:
	Point *cur;
	Screen(int res, double size, arrayd _normal, arrayd center, Media *G): 
		res(res), normal(normalize(_normal)), center(center), size(size),
		G(G), hoffset(3), voffset(3), voffset_reshoffset(3)
	{
		hoffset = size / res * normalize({ normal[2], 0, -normal[0] });
		voffset = arrayd{
			hoffset[1] * normal[2] - hoffset[2] * normal[1],
			hoffset[2] * normal[0] - hoffset[0] * normal[2],
			hoffset[0] * normal[1] - hoffset[1] * normal[0]
		};
		voffset_reshoffset = voffset - (res - 1.) * hoffset;
		cur = 0, counter = 0;
	}
	~Screen() { cur; }

	Point *next(double t) {
		if (counter == res * res) {
			delete cur;
			counter = 0;
			return 0;
		}
		if (!counter) {
			cur = new Point(t, center - res / 2. * (voffset + hoffset), normal, G);
		}
		else {
			cur->pos += counter % res ? hoffset : voffset_reshoffset;
		}
		if (counter % res == 0)
			cout << '.';
		counter++;
		return cur;
	}
};