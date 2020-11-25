#pragma once

#pragma once
#include<math.h>

class PointDouble {
public:
	PointDouble() { this->X = 0.0; this->Y = 0.0; };
	PointDouble(double X, double Y) { this->X = X; this->Y = Y; };
	~PointDouble() {};
public:
	double X;
	double Y;
public:
	bool compareDouble(const double& d1, const double& d2) const
	{
		if (sqrt(pow((d2 - d1), 2)) < 1e-13) {
			return true;
		}
		else {
			return false;
		}
	}
	PointDouble operator+(const PointDouble& point) const
	{
		return PointDouble(X + point.X,
			Y + point.Y);
	}
	PointDouble operator-(const PointDouble& point) const
	{
		return PointDouble(X - point.X,
			Y - point.Y);
	}
	PointDouble operator/(const double& d) const
	{
		return PointDouble(X / d,
			Y / d);
	}
	PointDouble operator*(const double& d) const
	{
		return PointDouble(X * d,
			Y * d);
	}
	bool operator==(const PointDouble& point) const
	{
		return (compareDouble(X, point.X) && compareDouble(Y, point.Y));
	}
	bool operator!=(const PointDouble& point) const
	{
		return !((compareDouble(X, point.X) && compareDouble(Y, point.Y)));
	}
	double distance(const PointDouble& point)
	{
		return sqrt(pow(X - point.X, 2) + pow(Y - point.Y, 2));
	}
};