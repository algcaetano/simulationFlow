#pragma once

//Aqui vir�o todas as fun��es relacionadas �s condi��es de contorno

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

template <class T>
int findID(std::vector<T>& input, T& ref) {
	int id = 0;
	for (std::vector<T>::iterator it = input.begin(); it != input.end(); it++) {
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

void applyPeriodicX(std::vector<std::vector<int>>& prop, std::vector<PointDouble>& domainPoints, double& periodicLine1, double& periodicLine2, Lattice dir) {
	double xRef;
	double difLimits = max(periodicLine1, periodicLine2) - min(periodicLine1, periodicLine2);
	PointDouble pRef;
	for (int i = 0; i < prop.size(); i++) {
		if (isDomain(domainPoints[prop[i][0]].X, periodicLine1, periodicLine2)) {//ponto inicial faz parte do dom�nio
			if (!isDomain(domainPoints[prop[i][2]].X, periodicLine1, periodicLine2)) {//ponto final n�o faz parte do dom�nio
				//o ponto final est� � direita ou � esquerda do dom�nio?
				xRef = (domainPoints[prop[i][2]].X > max(periodicLine1, periodicLine2)) ? (domainPoints[prop[i][2]].X-difLimits) : (domainPoints[prop[i][2]].X + difLimits);
				pRef.X = xRef;// x final muda
				pRef.Y = domainPoints[prop[i][2]].Y; // y final continua igual
				int id = findID(domainPoints, pRef); //achar qual o id desse ponto
				prop[i][2] = id; //mudar o ponto final
			}
		}
		else {
			prop[i][2] = prop[i][0]; //esse ponto est� fora do dom�nio e n�o pode propagar => fica no mesmo lugar
		}
	}
}