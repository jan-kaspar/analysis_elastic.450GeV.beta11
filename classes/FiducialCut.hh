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
			bool evolve;

		public:
			Point(double _x, double _y, bool _e) : x(_x), y(_y), evolve(_e) {}

			void Write() const { printf("(%.3E, %.3E, %i)", x, y, evolve); }

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
