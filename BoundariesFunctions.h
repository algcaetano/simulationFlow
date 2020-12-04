#pragma once

//Aqui virão todas as funções relacionadas às condições de contorno

#include <vector>
#include "PointDouble.h"
#include "Lattice.h"
#include "AuxFunctions.h"

double min(double& d1, double& d2) {
	return (d1 < d2) ? d1 : d2;
}

double max(double& d1, double& d2) {
	return (d2 > d1) ? d2 : d1;
}

int findID(std::vector<PointDouble>& input, PointDouble& ref) {
	int id = 0;
	for (std::vector<PointDouble>::iterator it = input.begin(); it != input.end(); it++) {
		if (*it == ref) {
			return id;
		}
		else {
			id++;
		}
	}
	return -1;
}

bool isDomain(double& refValue, double& periodicLine1, double& periodicLine2){
	return ((refValue < max(periodicLine1, periodicLine2)) && (refValue > min(periodicLine1, periodicLine2))) ? true : false;
}

void applyPeriodicX(std::vector<std::vector<int>>& prop, std::vector<PointDouble>& domainPoints, double& periodicLine1, double& periodicLine2) {
	double xRef;
	double difLimits = max(periodicLine1, periodicLine2) - min(periodicLine1, periodicLine2);
	PointDouble pRef, x0, xf, xPer;
	for (int i = 0; i < prop.size(); i++) {
		x0 = domainPoints[prop[i][0]];//ponto inicial da propagação
		xf = domainPoints[prop[i][2]];//ponto final da propagação
		if (isDomain(x0.X, periodicLine1, periodicLine2)) {//ponto inicial faz parte do domínio entre as linhas
			if (!isDomain(xf.X, periodicLine1, periodicLine2)) {//ponto final não faz parte do domínio entre as linhas
				//o ponto final está à direita ou à esquerda do domínio?
				xRef = (xf.X > max(periodicLine1, periodicLine2)) ? (xf.X-difLimits) : (xf.X + difLimits);
				pRef.X = xRef;// x final muda
				pRef.Y = xf.Y; // y final continua igual
				int id = findID(domainPoints, pRef); //achar qual o id desse ponto
				xPer = domainPoints[id];
				prop[i][2] = id; //mudar o ponto final
			}
		}
		else {
			prop[i][2] = prop[i][0]; //esse ponto está fora do domínio e não pode propagar => fica no mesmo lugar
		}
	}
}