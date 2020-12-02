//programa pra testar e elevar a velocidade - as varáveis já tem que existir
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
	//double tau1, tau2;
	//readTaus(tau1, tau2, loadFiles);
	Lattice lattice(4);

	double xMin = domainPoints[0].X;
	double xMax = xMin;
	for (int i = 0; i < domainPoints.size(); i++) {
		xMin = (domainPoints[i].X < xMin) ? domainPoints[i].X : xMin;
		xMax = (domainPoints[i].X > xMax) ? domainPoints[i].X : xMax;
	}
	double L = xMax - xMin + 1;
	double Kn = 0.0001;
	double Re = 1000;
	double Ma = 0.01;
	double T0 = 1;
	double cs = sqrt(2 * T0) / lattice.as;
	double u0 = Kn * Re * cs;
	double nu = u0 * L / Re;
	double tau2 = (pow(lattice.as, 2) * nu) / T0 + 1.0 / 2.0;
	double Pr = 0.7;
	double tau1 = (tau2 - 1.0 / 2.0) / Pr + 1.0 / 2.0;
	std::cout << "Number of points in x direction: " << L << "\n"
		<< "Reynolds: " << Re << "\n"
		<< "Knudsen: " << Kn << "\n"
		<< "sound speed: " << cs << "\n"
		<< "u0: " << u0 << "\n"
		<< "relaxation time 1: " << tau1 << "\n"
		<< "relaxation time 2: " << tau2 << "\n";

	std::vector<int> savePoints;
	double fractpart, intpart;
	fractpart = modf(L / 2, &intpart);
	double xMiddle = intpart + 0.5;
	for (int i = 0; i < domainPoints.size();i++) {
		if (compareDouble(xMiddle, domainPoints[i].X)) {
			savePoints.push_back(i);
		}
	}

	double t0, t1;
	std::vector<std::vector<double>> fTemp = f;
	std::vector<double> u(f.size());
	std::vector<double> Temp(f.size());
	std::vector<double> uOld = u;
	double rho, ux, uy, T, theta, uDotU;
	std::vector<double> uxf2(lattice.ex.size()), uxfUyf(lattice.ex.size()), uyf2(lattice.ex.size()); //velocidades de flutuação ao quadrado
	double error = 1;
	int saveControl = 1;
	int maxNumThreads = omp_get_max_threads();
	int numThreads = (maxNumThreads>1)?(maxNumThreads -1):1;
	int fSize = f.size();
	double tauFactor = (1 / tau2) - (1 / tau1);
	//simulação---------------------------------------
	omp_set_num_threads(numThreads);
	while (error>1e-13) {
		t0 = omp_get_wtime();
		for (int j = 0; j < 1000; j++) {
			colision(f, fSize, lattice, tau1, tauFactor);
			propagation(f, fTemp, fSize, lattice.numDir, propNoCol, propCol, fractions);
		}
		#pragma omp parallel for firstprivate(rho, ux, uy, T, uDotU, uxf2, uxfUyf, uyf2, lattice)
		for (int i = 0; i < u.size(); i++) {
			calculateMacVar(rho, ux, uy, T, theta, uDotU, uxf2, uxfUyf, uyf2, f[i], lattice);
			u[i] = ux;
			Temp[i] = T;
		}
		error = calculateRmsqrError(u, uOld);
		if (saveControl == 1) {
			saveF(f, (int)f.size(), (int)lattice.ex.size(), "fs1.dat");
			saveControl = 2;
		}
		else {
			saveF(f, (int)f.size(), (int)lattice.ex.size(), "fs2.dat");
			saveControl = 1;
		}
		t1 = omp_get_wtime();
		std::cout << "1000 iterations in: " << (t1 - t0) << " s. "<< numThreads <<"/"<<maxNumThreads << " threads used. " <<"RMS Error in x-component of velocity = " << error << "." << std::endl;
	}

	std::ofstream myfile("velocities.txt");
	if (myfile.is_open()) {
		for (int i = 0; i < savePoints.size();i++) {
			myfile << domainPoints[savePoints[i]].X << " " << domainPoints[savePoints[i]].Y << " " << u[savePoints[i]] / u0 << " " << Temp[i] << "\n";
		}
	}
	else {
		std::cerr << "Arquivo não pode ser aberto." << std::endl;
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