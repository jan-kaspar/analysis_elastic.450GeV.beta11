#ifndef _FiducialCut_hh_
#define _FiducialCut_hh_

namespace edm
{
	class ParameterSet;
}

#include <vector>
#include <set>
#include <tuple>
#include <cstdio>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct FiducialCut
{
	struct Point
	{
		private:
			double x, y;

			// slopes of th_y cut wrt. vtx_y [rad/mm], separately for positive and negative values of vtx_y
			double slope_pos, slope_neg;

		public:
			Point(double _x, double _y, double _sp, double _sn) : x(_x), y(_y), slope_pos(_sp), slope_neg(_sn) {}

			void Write() const { printf("(%.3E, %.3E, %.3E, %.3E)", x, y, slope_pos, slope_neg); }

			tuple<double /*x*/, double /*y*/> Resolve(double vtx_y) const;

			void ApplyShift(double dx, double dy) { x += dx; y += dy; }

			void ApplyCDTransform(double C, double D)
			{
				double x_orig = x, y_orig = y;
				x += C * y_orig;
				y += D * x_orig;
			}
	};

	vector<Point> points;

	static constexpr double mi_inf = -10.;
	static constexpr double pl_inf = +10.;

	static constexpr bool debug = false;

	FiducialCut() {}

	FiducialCut(const vector<Point> & _points) : points(_points)
	{
	}

	void Init(const std::vector<edm::ParameterSet> &input);

	void Print() const;

	void Shift(double x, double y);

	void ApplyCDTransform(double C, double D);

	bool Satisfied(double th_x, double th_y, double vtx_y) const;

	tuple<double /*th_y_min*/, double /*th_y_max*/> GetThYRange(double th_x, double vtx_y) const;

	vector<pair<double, double>> GetIntersectionPhis(double th, double vtx_y) const;
};

#endif
