#include "FiducialCut.hh"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <cmath>

//----------------------------------------------------------------------------------------------------

tuple<double /*x*/, double /*y*/>  FiducialCut::Point::Resolve(double vtx_y) const
{
	if (vtx_y >= 0.)
		return { x, y + slope_pos * vtx_y};
	else
		return { x, y + slope_neg * vtx_y};
}

//----------------------------------------------------------------------------------------------------

void FiducialCut::Init(const std::vector<edm::ParameterSet> &input)
{
	points.clear();
	for (const auto &p : input)
		points.emplace_back(Point(p.getParameter<double>("x"), p.getParameter<double>("y"),
			p.getParameter<double>("slope_pos"), p.getParameter<double>("slope_neg")));
}

//----------------------------------------------------------------------------------------------------

void FiducialCut::Print() const
{
	for (const auto &p : points)
	{
		p.Write();
		printf("-");
	}
	printf("\n");
}

//----------------------------------------------------------------------------------------------------

void FiducialCut::Shift(double x, double y)
{
	for (auto &p : points)
		p.ApplyShift(x, y);
}

//----------------------------------------------------------------------------------------------------

void FiducialCut::ApplyCDTransform(double C, double D)
{
	for (auto &p : points)
		p.ApplyCDTransform(C, D);
}

//----------------------------------------------------------------------------------------------------

bool FiducialCut::Satisfied(double th_x, double th_y, double vtx_y) const
{
	unsigned int n_le = 0, n_gr = 0;

	for (unsigned int i = 0; i < points.size(); i++)
	{
		const unsigned int j = (i + 1) % points.size();

		const auto [pi_x, pi_y] = points[i].Resolve(vtx_y);
		const auto [pj_x, pj_y] = points[j].Resolve(vtx_y);

		const bool hasIntersection = (pi_x < pj_x) ? (pi_x <= th_x && th_x < pj_x) : (pj_x <= th_x && th_x < pi_x);
		if (!hasIntersection)
			continue;

		const double a = (pj_y - pi_y) / (pj_x - pi_x);
		const double th_y_int = pj_y + a * (th_x - pj_x);

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

tuple<double /*th_y_min*/, double /*th_y_max*/> FiducialCut::GetThYRange(double th_x, double vtx_y) const
{
	double th_y_min = pl_inf;
	double th_y_max = mi_inf;

	for (unsigned int i = 0; i < points.size(); i++)
	{
		const unsigned int j = (i + 1) % points.size();

		const auto [pi_x, pi_y] = points[i].Resolve(vtx_y);
		const auto [pj_x, pj_y] = points[j].Resolve(vtx_y);

		const bool hasIntersection = (pi_x < pj_x) ? (pi_x <= th_x && th_x <= pj_x) : (pj_x <= th_x && th_x <= pi_x);
		if (!hasIntersection)
			continue;

		const double a = (pj_y - pi_y) / (pj_x - pi_x);
		const double th_y_int = pj_y + a * (th_x - pj_x);

		th_y_min = min(th_y_int, th_y_min);
		th_y_max = max(th_y_int, th_y_max);
	}

	return {th_y_min, th_y_max};
}

//----------------------------------------------------------------------------------------------------

vector<pair<double, double>> FiducialCut::GetIntersectionPhis(double th, double vtx_y) const
{
	set<double> phis;

	if (debug)
		printf("GetIntersectionPhis(%.1f)\n", th*1e6);

	for (unsigned int i = 0; i < points.size(); i++)
	{
		const unsigned int j = (i + 1) % points.size();

		const auto [pi_x, pi_y] = points[i].Resolve(vtx_y);
		const auto [pj_x, pj_y] = points[j].Resolve(vtx_y);

		const double a = (pj_y - pi_y) / (pj_x - pi_x);
		const double b = pj_y - a * pj_x;

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
			if (pi_x <= pj_x && (x < pi_x || x >= pj_x))
				continue;

			if (pj_x <= pi_x && (x < pj_x || x >= pi_x))
				continue;

			const double y = a*x + b;
			const double phi = atan2(y, x);

			phis.insert(phi);

			if (debug)
				printf("    add phi: i=%i (x=%.1f), j=%i (x=%.1f), D=%.1E, x=%.1f | phi=%.4f\n", i, pi_x*1e6, j, pj_x*1e6, D, x*1e6, phi);
		}
	}

	vector<pair<double, double>> segments;

	if ((phis.size() % 2) != 0)
	{
		printf("ERROR in FiducialCut::GetIntersectionPhis > even number of phis.\n");
		return segments;
	}

	for (set<double>::iterator it = phis.begin(); it != phis.end(); ++it)
	{
		pair<double, double> p;
		p.first = *it;
		++it;
		p.second = *it;
		segments.push_back(p);
	}

	return segments;
}
