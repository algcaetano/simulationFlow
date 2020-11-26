#include "Lattice.h"

Lattice::Lattice()
{
	this->ex = { 0 };
	this->ey = { 0 };
	this->as = 0;
	this->ws = { 0 };
}

Lattice::Lattice(int order)
{
	//lattice with given order
	setLattice(order, this->ex, this->ey, this->ws, this->as);
}

Lattice::~Lattice()
{
	//destructor
}

void Lattice::setLattice(int order, std::vector<int>& ex, std::vector<int>& ey, std::vector<double>& ws, double& as)
{
	double R = pow((67.0 + 36.0 * sqrt(30.0)), (1.0 / 3.0));
	double w0 = (56266.0 * pow(R, 2.0) - (pow(7.0, (2.0 / 3.0))) * (19991.0 - 338.0 * sqrt(30.0)) * R + (pow(7.0, (4.0 / 3.0))) * (14323.0 + 6238.0 * sqrt(30.0))) / (264600.0 * pow(R, 2.0));
	double w1_4 = (31206.0 * pow(R, 2.0) - (pow(7.0, (2.0 / 3.0))) * (3201.0 + 466.0 * sqrt(30.0)) * R - (pow(7.0, (4.0 / 3.0))) * (2427.0 - 706.0 * sqrt(30.0))) / (264600.0 * pow(R, 2));
	double w5_8 = (29232.0 * pow(R, 2) + (pow(7, (2.0 / 3.0))) * (3888 + 265 * sqrt(30.0)) * R + (pow(7, (4.0 / 3.0))) * (216 - 1027 * sqrt(30.0))) / (529200.0 * pow(R, 2));
	double w9_12 = (42.0 * pow(R, 2) + (pow(7.0, (2.0 / 3.0))) * (33 + 2 * sqrt(30)) * R - (pow(7.0, (1.0 / 3.0))) * (3 + 62 * sqrt(30))) / (3600 * pow(R, 2));
	double w13_20 = (1638 * pow(R, 2) + (pow(7, (2.0 / 3.0))) * (1647 + 4 * sqrt(30)) * R - (pow(7, (4.0 / 3.0))) * (891 + 496 * sqrt(30))) / (264600 * pow(R, 2));
	double w21_24 = (-126 * pow(R, 2) + (pow(7, (2.0 / 3.0))) * (1161 + 194 * sqrt(30)) * R + (pow(7, (4.0 / 3.0))) * (1107 - 242 * sqrt(30))) / (1058400.0 * pow(R, 2));
	double w25_28 = (14 * pow(R, 2) + (pow(7, (2.0 / 3.0))) * (131 + 10 * sqrt(30)) * R + (pow(7, (4.0 / 3.0))) * (17 - 34 * sqrt(30))) / (264600 * pow(R, 2));
	double w29_36 = (-168 * pow(R, 2) + (pow(7, (2.0 / 3.0))) * (228 + 71 * sqrt(30)) * R + (pow(7, (4.0 / 3.0))) * (516 - 29 * sqrt(30))) / (1058400 * pow(R, 2));
	switch (order)
	{
	default:
		ex = { 0, 1, 0, -1, 0, 1, -1, -1, 1, 2, 0, -2, 0, 2, 1, -1, -2, -2, -1, 1, 2, 2, -2, -2, 2, 3, 0, -3, 0, 3, 1, -1, -3, -3, -1, 1, 3 };
		ey = { 0, 0, 1, 0, -1, 1, 1, -1, -1, 0, 2, 0, -2, 1, 2, 2, 1, -1, -2, -2, -1, 2, 2, -2, -2, 0, 3, 0, -3, 1, 3, 3, 1, -1, -3, -3, -1 };
		ws = { w0, w1_4, w1_4, w1_4, w1_4, w5_8, w5_8, w5_8, w5_8, w9_12, w9_12, w9_12, w9_12,
			w13_20, w13_20, w13_20, w13_20, w13_20, w13_20, w13_20, w13_20, w21_24, w21_24, w21_24, w21_24,
			w25_28, w25_28, w25_28, w25_28, w29_36, w29_36, w29_36, w29_36, w29_36, w29_36, w29_36, w29_36 };
		as = (1.0 / 6.0) * sqrt(49.0 - 17.0 * (pow(7, (2.0 / 3.0))) / R + (pow(7, (1.0 / 3.0))) * R);
		as2 = as * as;
		ex2 = ex;
		ey2 = ey;
		std::vector<double> eTemp(ex.size());
		for (int i = 0; i < ex2.size(); i++) {
			ex2[i] *= ex2[i];
			ey2[i] *= ey2[i];
			eTemp[i] = as2 * (ex2[i] + ey2[i]);
		}
		eDotE = eTemp;
		numDir = ex.size();
		break;
	}
}