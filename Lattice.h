#pragma once

#include <vector>
#include <math.h>

class Lattice {
public:
	//constructors
	Lattice();
	Lattice(int order);
	~Lattice();
public:
	//members
	std::vector<int> ex; //dire��es x das velocidades
	std::vector<int> ey; //dire��es y das velocidades
	std::vector<int> ex2; //valores ao quadrado dos valores;
	std::vector<int> ey2; //valores ao quadrado dos valores
	std::vector<double> eDotE; //produto interno das dire��es
	std::vector<double> ws; //pesos das velocidades
	double as; //fator de escala
	double as2; //fator de escala ao quadrado
	int numDir; //n�mero de dire��es discretas
private:
	//functions members
	void setLattice(int order, std::vector<int>& ex, std::vector<int>& ey, std::vector<double>& ws, double& as);
};