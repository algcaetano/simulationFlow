#pragma once

#include<math.h>
#include<vector>
#include<fstream>
#include<string>
#include"PointDouble.h"

bool compareDouble(const double& d1, const double& d2) {
	return (sqrt(pow((d2 - d1), 2)) < 1e-13) ? true : false;
}

void saveF(std::vector<std::vector<double>>& f, const int& fSize, const int& numDirections, std::string name) {
	std::ofstream outFile(name, std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		std::cerr << "cannot open file" << std::endl;
	}
	else {
		outFile.write((char*)&fSize, sizeof(int));
		outFile.write((char*)&numDirections, sizeof(int));
		for (std::vector<std::vector<double>>::iterator it1 = f.begin(); it1 != f.end(); it1++) {
			for (std::vector<double>::iterator it2 = (*it1).begin(); it2 != (*it1).end(); it2++) {
				outFile.write((char*)&(*it2), sizeof(double));
			}
		}
	}
	outFile.close();
}

void saveProp(std::vector<std::vector<int>>& prop, const int& propSize, const std::string& varName) {
	std::ofstream outFile(varName, std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		std::cerr << "cannot open file" << std::endl;
	}
	else {
		outFile.write((char*)&propSize, sizeof(int));
		for (std::vector<std::vector<int>>::iterator it1 = prop.begin(); it1 != prop.end(); it1++) {
			for (std::vector<int>::iterator it2 = (*it1).begin(); it2 != (*it1).end(); it2++) {
				outFile.write((char*)&(*it2), sizeof(int));
			}
		}
	}
	outFile.close();
}

void saveFractions(std::vector<double>& fractionsValues, const int& fractionsValuesSize, const std::string& varName) {
	std::ofstream outFile(varName, std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		std::cerr << "cannot open file" << std::endl;
	}
	else {
		outFile.write((char*)&fractionsValuesSize, sizeof(int));
		for (std::vector<double>::iterator it1 = fractionsValues.begin(); it1 != fractionsValues.end(); it1++) {
			outFile.write((char*)&(*it1), sizeof(double));
		}
	}
	outFile.close();
}

void saveDomainPoints(std::vector<PointDouble>& domainPoints, const int& size, const std::string& varName) {
	std::ofstream outFile(varName, std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		std::cerr << "cannot open file" << std::endl;
	}
	else {
		outFile.write((char*)&size, sizeof(int));
		for (std::vector<PointDouble>::iterator it = domainPoints.begin(); it != domainPoints.end(); it++) {
			outFile.write((char*)&((*it).X), sizeof(double));
			outFile.write((char*)&((*it).Y), sizeof(double));
		}
	}
}

void saveTaus(double& tau1, double& tau2, const std::string& varName) {
	std::ofstream outFile(varName, std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		std::cerr << "cannot open file" << std::endl;
	}
	else {
		outFile.write((char*)&tau1, sizeof(double));
		outFile.write((char*)&tau2, sizeof(double));
	}
}

void readTaus(double& tau1, double& tau2, std::ifstream& inputFile) {
	inputFile.read((char*)&tau1, sizeof(double));
	inputFile.read((char*)&tau2, sizeof(double));
	inputFile.close();
}

std::vector<std::vector<double>> readF(std::ifstream& inputFile) {
	int sizeF, numDirections;
	inputFile.read((char*)&sizeF, sizeof(int));
	inputFile.read((char*)&numDirections, sizeof(int));
	std::vector<std::vector<double>> f(sizeF, std::vector<double>(numDirections));
	for (int i = 0; i < sizeF; i++) {
		for (int j = 0; j < numDirections; j++) {
			inputFile.read((char*)&f[i][j], sizeof(double));
		}
	}
	inputFile.close();
	return f;
}

std::vector<std::vector<int>> readProp(std::ifstream& inputFile) {
	int sizeProp;
	inputFile.read((char*)&sizeProp, sizeof(int));
	std::vector<std::vector<int>> f(sizeProp, std::vector<int>(4));
	for (int i = 0; i < sizeProp; i++) {
		for (int j = 0; j < 4; j++) {
			inputFile.read((char*)&f[i][j], sizeof(int));
		}
	}
	inputFile.close();
	return f;
}

std::vector<PointDouble> readPoints(std::ifstream& inputFile) {
	int size;
	inputFile.read((char*)&size, sizeof(int));
	std::vector<PointDouble> f(size);
	for (int i = 0; i < size; i++) {
		inputFile.read((char*)&f[i].X, sizeof(double));
		inputFile.read((char*)&f[i].Y, sizeof(double));
	}
	inputFile.close();
	return f;
}

std::vector<double> readDoubleVec(std::ifstream& inputFile) {
	int size;
	inputFile.read((char*)&size, sizeof(int));
	std::vector<double> f(size);
	for (int i = 0; i < size; i++) {
		inputFile.read((char*)&f[i], sizeof(double));
	}
	inputFile.close();
	return f;
}