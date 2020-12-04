#pragma once
#include<Windows.h>
#include <vector>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")
#include <omp.h>
#include <iostream>
#include "PointDouble.h"

class DomainPoints {
public:
	DomainPoints();
	DomainPoints(std::vector<Point> pointList, Point min, Point max);
	~DomainPoints();
public:
	std::vector<PointDouble> domainPoints;
private:
	std::vector<PointDouble> getInteriorPoints(std::vector<Point> pointList, Point min, Point max);
};