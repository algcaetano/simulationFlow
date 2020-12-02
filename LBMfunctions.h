#pragma once
#include<vector>
#include"Lattice.h"
#include<omp.h>

inline double power(const double& base, int exp) {
	double result = 1;
	for (int i = 0; i < exp; i++) {
		result *= base;
	}
	return result;
}

inline void calculateMacVar(double& rho, double& ux, double& uy, double& T, double& theta, double& uDotU, std::vector<double>& uxf2, 
	std::vector<double>& uxfUyf, std::vector<double>& uyf2, const std::vector<double>& f, const Lattice& dir) {
	rho = ux = uy = T = 0.0;
	double tempFi;
	for (int i = 0; i < dir.numDir; i++) {
		tempFi = f[i];
		rho += tempFi;
		ux += tempFi * dir.ex[i];
		uy += tempFi * dir.ey[i];
		T += tempFi * dir.eDotE[i];
	}
	ux /= rho;
	uy /= rho;
	uDotU = dir.as2 * (power(ux, 2) + power(uy, 2));
	T = ((T / rho) - uDotU) / 2;
	theta = T - 1;
	for (int i = 0; i < dir.numDir; i++) {
		uxf2[i] = dir.as2 * power((dir.ex[i] - ux), 2);
		uxfUyf[i] = dir.as2 * (dir.ex[i] - ux) * (dir.ey[i] - uy);
		uyf2[i] = dir.as2 * power((dir.ey[i] - uy), 2);
	}
}

inline double calculateFiEq(int& ex, int& ey, double& eDotE, double& as2, double& w, double& rho, double& ux, 
	double& uy, double& theta, double& uDotU) {
	double result = 1.0;
	double F = as2 * (ux * ex + uy * ey);
	double R = eDotE * theta - uDotU;
	double F2 = power(F, 2);
	result += F;
	result += (F2 + R - 2 * theta) / 2;
	result += (F2*F + 3 * F * R - 12 * F * theta) / 6;
	result += (power(F2, 2) + 6 * F2 * R - 3 * (12 * F2 * theta - power(R, 2)) - 24 * R * theta + 24 * power(theta, 2)) / 24;
	result *= (w * rho);
	return result;
}

inline void calculateFEq(double& rho, double& ux, double& uy, double& theta, double& uDotU, std::vector<double>& fEq, Lattice& dir) {
	for (int i = 0; i < dir.numDir; i++) {
		fEq[i] = calculateFiEq(dir.ex[i], dir.ey[i], dir.eDotE[i] , dir.as2, dir.ws[i], rho, ux, uy, theta, uDotU);
	}
}

inline void calculateFProp(std::vector<double>& f, std::vector<double>& fEq, int& fiSize, std::vector<double>& aNEq, double& tau1, double& tauFactor) {
	for (int i = 0; i < fiSize; i++) {
		f[i] += (fEq[i] - f[i]) / tau1 - tauFactor * aNEq[i];
	}
}

inline void calculateTxy(std::vector<double>& f, std::vector<double>& fEq, int& numDir, double& txx, double& txy, double& tyy,
	std::vector<double>& uxf2, std::vector<double>& uxfUyf, std::vector<double>& uyf2, double& as2, double& rho, double& ux, double& uy, double& T) {
	txx = txy = tyy = 0.0;
	double fNEq;
	for (int i = 0; i < numDir; i++) {
		fNEq = (f[i] - fEq[i]);
		txx += fNEq * uxf2[i];
		txy += fNEq * uxfUyf[i];
		tyy += fNEq * uyf2[i];
	}
	double factor = rho * T;
	txx /= factor;
	txy /= factor;
	tyy /= factor;
}

inline double calculateAiNeq(double& fiEq, double& uxf2i, double& uxfUyfi, double& uyf2i, double& as2, double& txx, double& txy, double& tyy, double& ux, double& uy) {
	return fiEq * (txx * (uxf2i - 1) + 2 * txy * uxfUyfi + tyy * (uyf2i - 1)) / 2;
}

inline void calculateANEq(std::vector<double>& fEq, std::vector<double>& aNeq, Lattice& dir, double& txx, double& txy, double& tyy,
	double& ux, double& uy, std::vector<double>& uxf2, std::vector<double>& uxfUyf, std::vector<double>& uyf2) {
	for (int i = 0; i < dir.numDir; i++) {
		aNeq[i] = calculateAiNeq(fEq[i], uxf2[i], uxfUyf[i], uyf2[i], dir.as2, txx, txy, tyy, ux, uy);
	}
}

inline void clearTempFi(std::vector<double>& fTempi, int numDir) {
	for (int i = 0; i < numDir; i++) {
		fTempi[i] = 0.0;
	}
}

void calculateNewF(double& rho, double& ux, double&  uy, double& T, double&  txx, double&  txy, double&  tyy, double&  uDotU, 
	double& theta, std::vector<double>& fi, std::vector<double>& fEq, std::vector<double>& aNEq, std::vector<double>& uxf2, std::vector<double>& uxfUyf,
	std::vector<double>& uyf2, double& tau1, double& tauFactor, Lattice& dir) {
	calculateMacVar(rho, ux, uy, T, theta, uDotU, uxf2, uxfUyf, uyf2, fi, dir);
	calculateFEq(rho, ux, uy, theta, uDotU, fEq, dir);
	calculateTxy(fi, fEq, dir.numDir, txx, txy, tyy, uxf2, uxfUyf, uyf2, dir.as2, rho, ux, uy, T);
	calculateANEq(fEq, aNEq, dir, txx, txy, tyy, ux, uy, uxf2, uxfUyf, uyf2);
	calculateFProp(fi, fEq, dir.numDir, aNEq, tau1, tauFactor);
}

inline void colision(std::vector<std::vector<double>>& f, int& fSize, Lattice dir, double tau1, double tauFactor) {
	#pragma omp parallel
	{
		double rho, ux, uy, T, txx, txy, tyy, uDotU, theta;
		std::vector<double> fEq(dir.numDir); //funções de distribuição de equilíbrio
		std::vector<double> aNEq(dir.numDir); //parte de não equilíbrio viscoso das funções de distribuição
		std::vector<double> uxf2(dir.numDir), uxfUyf(dir.numDir), uyf2(dir.numDir); //velocidades de flutuação ao quadrado
		#pragma omp for firstprivate(dir, tau1, tauFactor) schedule (static)
		for (int i = 0; i < fSize; i++) {
			calculateMacVar(rho, ux, uy, T, theta, uDotU, uxf2, uxfUyf, uyf2, f[i], dir);
			calculateFEq(rho, ux, uy, theta, uDotU, fEq, dir);
			calculateTxy(f[i], fEq, dir.numDir, txx, txy, tyy, uxf2, uxfUyf, uyf2, dir.as2, rho, ux, uy, T);
			calculateANEq(fEq, aNEq, dir, txx, txy, tyy, ux, uy, uxf2, uxfUyf, uyf2);
			calculateFProp(f[i], fEq, dir.numDir, aNEq, tau1, tauFactor);
			//calculateNewF(rho, ux, uy, T, txx, txy, tyy, uDotU, theta, f[i], fEq, aNEq, uxf2, uxfUyf, uyf2, tau1, tauFactor, dir);
		}
	}
}

inline void propagation(std::vector<std::vector<double>>& f, std::vector<std::vector<double>>& fTemp, int fSize, int numDir,
	std::vector<std::vector<int>>& propNoCol, std::vector<std::vector<int>>& propCol, std::vector<double>& fractions) {
	#pragma omp parallel for
	for (int i = 0; i < fSize; i++) {
		clearTempFi(fTemp[i], numDir);
	}
	#pragma omp parallel for
	for (int i = 0; i < propNoCol.size(); i++) {
		fTemp[propNoCol[i][2]][propNoCol[i][3]] = f[propNoCol[i][0]][propNoCol[i][1]];
	}
	for (int i = 0; i < propCol.size();i++) {
		fTemp[propCol[i][2]][propCol[i][3]] += fractions[i] * f[propCol[i][0]][propCol[i][1]];
	}
	f.swap(fTemp);
}

double calculateError(std::vector<double>& newValues, std::vector<double>& oldValues) {
	double sum = 0.0;
	#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < newValues.size();i++) {
		sum += power((newValues[i] - oldValues[i]), 2);
	}
	oldValues.swap(newValues);
	return sqrt(sum);
}

double calculateRmsqrError(std::vector<double>& newValues, std::vector<double>& oldValues) {
	double sum = 0.0;
	int n = newValues.size();
	#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < newValues.size();i++) {
		sum += power((newValues[i] - oldValues[i]), 2);
	}
	oldValues.swap(newValues);
	return sqrt(sum/n);
}