#include "FiducialCut.hh"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <cmath>

//----------------------------------------------------------------------------------------------------

void FiducialCut::Init(const std::vector<edm::ParameterSet> &input)
{
	points.clear();
	for (const auto &p : input)
		points.emplace_back(Point{p.getParameter<double>("x"), p.getParameter<double>("y")});
}

//----------------------------------------------------------------------------------------------------

void FiducialCut::Print() const
{
	for (const auto &p : points)
		printf("(%.3E, %.3E)-", p.x, p.y);
	printf("\n");
}

//----------------------------------------------------------------------------------------------------

void FiducialCut::Shift(double x, double y)
{
	for (auto &p : points)
	{
		p.x += x;
		p.y += y;
	}	
}

//----------------------------------------------------------------------------------------------------

void FiducialCut::ApplyCDTransform(double C, double D)
{
	for (auto &p : points)
	{
		const auto &p_orig = p;
		p.x += C * p_orig.y;
		p.y += D * p_orig.x;
	}
}

//----------------------------------------------------------------------------------------------------

bool FiducialCut::Satisfied(double th_x, double th_y) const
{
	unsigned int n_le = 0, n_gr = 0;

	for (unsigned int i = 0; i < points.size(); i++)
	{
		const unsigned int j = (i + 1) % points.size();

		const bool hasIntersection = (points[i].x < points[j].x) ? (points[i].x <= th_x && th_x < points[j].x) : (points[j].x <= th_x && th_x < points[i].x);
		if (!hasIntersection)
			continue;

		const double a = (points[j].y - points[i].y) / (points[j].x - points[i].x);
		const double th_y_int = points[j].y + a * (th_x - points[j].x);

		if (th_y_int < th_y)
			n_le++;
		if (th_y_int > th_y)
			n_gr++;
	}

	if (debug)
		printf("n_le = %u, n_gr = %u\n", n_le, n_gr);

	return ((n_gr % 2) == 1);
}

//----------------------------------------------------------------------------------------------------

void FiducialCut::GetThYRange(double th_x, double &th_y_min, double &th_y_max) const
{
	th_y_min = pl_inf;
	th_y_max = mi_inf;

	for (unsigned int i = 0; i < points.size(); i++)
	{
		const unsigned int j = (i + 1) % points.size();

		const bool hasIntersection = (points[i].x < points[j].x) ? (points[i].x <= th_x && th_x <= points[j].x) : (points[j].x <= th_x && th_x <= points[i].x);
		if (!hasIntersection)
			continue;

		const double a = (points[j].y - points[i].y) / (points[j].x - points[i].x);
		const double th_y_int = points[j].y + a * (th_x - points[j].x);

		th_y_min = min(th_y_int, th_y_min);
		th_y_max = max(th_y_int, th_y_max);
	}
}

//----------------------------------------------------------------------------------------------------

vector<FiducialCut::Point> FiducialCut::GetIntersectionPhis(double th) const
{
	set<double> phis;

	if (debug)
		printf("GetIntersectionPhis(%.1f)\n", th*1e6);

	for (unsigned int i = 0; i < points.size(); i++)
	{
		const unsigned int j = (i + 1) % points.size();

		const double a = (points[j].y - points[i].y) / (points[j].x - points[i].x);
		const double b = points[j].y - a * points[j].x;

		const double A = 1. + a*a;
		const double B = 2. * a * b;
		const double C = b*b - th*th;

		double D = B*B - 4.*A*C;

		if (D < 0.)
			continue;

		if (D == 0.)
			D = 1E-20;

		const double x1 = (-B + sqrt(D)) / 2. / A;
		const double x2 = (-B - sqrt(D)) / 2. / A;

		for (const double &x : {x1, x2})
		{
			if (points[i].x <= points[j].x && (x < points[i].x || x >= points[j].x))
				continue;

			if (points[j].x <= points[i].x && (x < points[j].x || x >= points[i].x))
				continue;

			const double y = a*x + b;
			const double phi = atan2(y, x);

			phis.insert(phi);

			if (debug)
				printf("    add phi: i=%i (x=%.1f), j=%i (x=%.1f), D=%.1E, x=%.1f | phi=%.4f\n", i, points[i].x*1e6, j, points[j].x*1e6, D, x*1e6, phi);
		}
	}

	vector<Point> segments; // x = start phi, y = end phi

	if ((phis.size() % 2) != 0)
	{
		printf("ERROR in FiducialCut::GetIntersectionPhis > even number of phis.\n");
		return segments;
	}

	for (set<double>::iterator it = phis.begin(); it != phis.end(); ++it)
	{
		Point p;
		p.x = *it;
		++it;
		p.y = *it;
		segments.push_back(p);
	}

	return segments;
}
