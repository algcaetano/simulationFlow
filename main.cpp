//programa pra testar e elevar a velocidade - as var�veis j� tem que existir
#include<iostream>
#include"AuxFunctions.h"
#include"PointDouble.h"
#include "Lattice.h"
#include"LBMfunctions.h"

int main() {

	std::ifstream loadFiles("distributions.dat", std::ios::in | std::ios::binary);
	std::vector<std::vector<double>> f = readF(loadFiles);
	loadFiles.open("domainPoints.dat", std::ios::in | std::ios::binary);
	std::vector<PointDouble> domainPoints = readPoints(loadFiles);
	loadFiles.open("fractions.dat", std::ios::in | std::ios::binary);
	std::vector<double> fractions = readDoubleVec(loadFiles);
	loadFiles.open("propNoCol.dat", std::ios::in | std::ios::binary);
	std::vector<std::vector<int>> propNoCol = readProp(loadFiles);
	loadFiles.open("propCol.dat", std::ios::in | std::ios::binary);
	std::vector<std::vector<int>> propCol = readProp(loadFiles);
	loadFiles.open("taus.dat", std::ios::in | std::ios::binary);
	double tau1, tau2;
	readTaus(tau1, tau2, loadFiles);
	Lattice lattice(4);

	double xMin = domainPoints[0].X;
	double xMax = xMin;
	for (int i = 0; i < domainPoints.size(); i++) {
		xMin = (domainPoints[i].X < xMin) ? domainPoints[i].X : xMin;
		xMax = (domainPoints[i].X > xMax) ? domainPoints[i].X : xMax;
	}
	double L = xMax - xMin;
	double Kn = 0.0001;
	double Re = 1000;
	double Ma = 0.01;
	double T0 = 1;
	double cs = sqrt(2 * T0) / lattice.as;
	double u0 = Kn * Re * cs;
	std::vector<int> savePoints;
	double fractpart, intpart;
	fractpart = modf(L / 2, &intpart);
	double xMiddle = intpart + 0.5;
	for (int i = 0; i < domainPoints.size();i++) {
		if (compareDouble(domainPoints[i].X, xMiddle)) {
			savePoints.push_back(i);
		}
	}

	double t0, t1;
	std::vector<std::vector<double>> fTemp = f;
	std::vector<double> u(f.size());
	std::vector<double> uOld = u;
	double rho, ux, uy, T, txx, txy, tyy, uDotU, theta;
	std::vector<double> uxf2(lattice.ex.size()), uxfUyf(lattice.ex.size()), uyf2(lattice.ex.size()); //velocidades de flutua��o ao quadrado
	double error = 1;
	int saveControl = 1;
	int maxNumThreads = omp_get_max_threads();
	
	//simula��o---------------------------------------
	omp_set_num_threads(maxNumThreads);
	while (error>1e-13) {
		t0 = omp_get_wtime();
		for (int j = 0; j < 1000; j++) {
			colision(f, lattice, tau1, tau2);
			propagation(f, fTemp, propNoCol, propCol, fractions);
		}
		#pragma omp parallel for firstprivate(rho, ux, uy, T, uDotU, uxf2, uxfUyf, uyf2, lattice)
		for (int i = 0; i < u.size(); i++) {
			calculateMacVar(rho, ux, uy, T, uDotU, uxf2, uxfUyf, uyf2, f[i], lattice);
			u[i] = ux;
		}
		error = calculateError(u, uOld);
		if (saveControl == 1) {
			saveF(f, (int)f.size(), (int)lattice.ex.size(), "fs1.dat");
			saveControl = 2;
		}
		else {
			saveF(f, (int)f.size(), (int)lattice.ex.size(), "fs2.dat");
			saveControl = 1;
		}
		t1 = omp_get_wtime();
		std::cout << "1000 iterations in: " << (t1 - t0) << " s. "<< maxNumThreads <<"/"<<maxNumThreads << " threads used. " <<"Error in x-component of velocity = " << error << "." << std::endl;
	}

	std::ofstream myfile("velocities.txt");
	if (myfile.is_open()) {
		for (int i = 0; i < savePoints.size();i++) {
			myfile << domainPoints[savePoints[i]].Y << " " << u[savePoints[i]] / u0 << "\n";
		}
	}
	else {
		std::cerr << "Arquivo n�o pode ser aberto." << std::endl;
	}
	myfile.close();

	std::cout << "Simulation finished. -1 to exit." << std::endl;
	int testPause = 0;
	while (testPause != -1)
	{
		std::cin >> testPause;
	}
	return 0;
}