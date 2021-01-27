#include "classes/FiducialCut.hh"

#include <cstdio>
#include <cmath>

//----------------------------------------------------------------------------------------------------

void test_Satisfied(const FiducialCut &fc, double th_x, double th_y, bool exp)
{
	const bool r = fc.Satisfied(th_x, th_y);

	printf("* th_x = %.1f urad, th_y = %.1f urad: satisfied = %u --> %s\n", th_x*1E6, th_y*1E6, r, (r == exp) ? "OK" : "FAIL");
}

//----------------------------------------------------------------------------------------------------

void test_GetThYRange(const FiducialCut &fc, double th_x, double exp_th_y_min, bool exp_th_y_max)
{
	double th_y_min, th_y_max;
	fc.GetThYRange(th_x, th_y_min, th_y_max);

	const bool ok = (fabs(th_y_min - exp_th_y_min) < 1E-10 && fabs(th_y_max - exp_th_y_max));

	printf("* th_x = %.1f urad: th_y min = %.1f, max = %.1f --> %s\n", th_x*1E6, th_y_min*1E6, th_y_max*1E6, (ok) ? "OK" : "FAIL");
}

//----------------------------------------------------------------------------------------------------

void test_GetIntersectionPhis(const FiducialCut &fc, double th, unsigned int exp_n_segments)
{
	const auto &segments = fc.GetIntersectionPhis(th);

	const bool ok = (segments.size() == exp_n_segments);

	printf("* th = %.1f urad: %lu segments --> %s\n", th*1E6, segments.size(), (ok) ? "OK" : "FAIL");

	/*
	for (unsigned int si = 0; si < segments.size(); ++si)
	{
		const double s_phi = segments[si].x;
		const double s_x = th * cos(s_phi);
		const double s_y = th * sin(s_phi);

		const double e_phi = segments[si].y;
		const double e_x = th * cos(e_phi);
		const double e_y = th * sin(e_phi);

		printf("    segment %u: start = (%.1f, %.1f), end = (%.1f, %.1f)\n", si, s_x*1E6, s_y*1E6, e_x*1E6, e_y*1E6);
	}
	*/
}

//----------------------------------------------------------------------------------------------------

int main()
{
	FiducialCut fc1({{-350E-6, 32E-6}, {0E-6, 32E-6}, {250E-6, 40E-6}, {390E-6, 80E-6}, {270E-6, 127E-6}, {-280E-6, 132E-6}, {-390E-6, 60E-6}});

	FiducialCut fc2({{-350E-6, 33E-6}, {0E-6, 33E-6}, {250E-6, 42E-6}, {385E-6, 80E-6}, {270E-6, 124E-6}, {-280E-6, 129E-6}, {-385E-6, 60E-6}});

	printf("\n>> test Satisfied, not at borders\n");

	test_Satisfied(fc1, 50E-6, 50E-6, true);
	test_Satisfied(fc1, -300E-6, 60E-6, true);

	test_Satisfied(fc1, 50E-6, 0E-6, false);
	test_Satisfied(fc1, -500E-6, 50E-6, false);
	test_Satisfied(fc1, -50E-6, 200E-6, false);

	printf("\n>> test Satisfied, at borders and at nodes\n");

	test_Satisfied(fc1, 0E-6, 0E-6, false);

	test_Satisfied(fc1, 0E-6, 32E-6, true);
	test_Satisfied(fc1, 250E-6, 40E-6, true);

	test_Satisfied(fc1, -100E-6, 32E-6, true);

	printf("\n>> test GetThYRange\n");

	test_GetThYRange(fc1, -500E-6, 10., -10.);
	test_GetThYRange(fc1, -390E-6, 60E-6, 60E-6);
	test_GetThYRange(fc1, -350E-6, 32E-6, 86.2E-6);
	test_GetThYRange(fc1, -200E-6, 32E-6, 131.3E-6);
	test_GetThYRange(fc1, 0E-6, 32E-6, 129.5E-6);
	test_GetThYRange(fc1, +500E-6, 10., -10.);

	printf("\n>> test GetIntersectionPhis\n");

	test_GetIntersectionPhis(fc2, 50E-6, 1);
	test_GetIntersectionPhis(fc2, 100E-6, 1);
	test_GetIntersectionPhis(fc2, 150E-6, 2);
	test_GetIntersectionPhis(fc2, 200E-6, 2);

	test_GetIntersectionPhis(fc2, sqrt(250E-6*250E-6 + 42E-6*42E-6), 2);
	test_GetIntersectionPhis(fc2, sqrt(270E-6*270E-6 + 124E-6*124E-6), 2);
	test_GetIntersectionPhis(fc2, sqrt(385E-6*385E-6 + 80E-6*80E-6), 0);
	test_GetIntersectionPhis(fc2, sqrt(385E-6*385E-6 + 80E-6*80E-6) - 1E-10, 1);
}
