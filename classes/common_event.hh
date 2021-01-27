#ifndef _common_event_hh_
#define _common_event_hh_

#include "common_alignment.hh"

using namespace std;

//----------------------------------------------------------------------------------------------------

struct UnitHitData
{
	// validity flag
	unsigned int v;

	// hit position in mm
	double x, y;

	UnitHitData() : v(0), x(0.), y(0.) {}

	void operator += (const UnitHitData &add)
	{
		x += add.x;
		y += add.y;
	}
};

//----------------------------------------------------------------------------------------------------

struct HitData
{
	UnitHitData L_1_F, L_2_F;
	UnitHitData R_1_F, R_2_F;

	void operator += (const HitData &add)
	{
		L_1_F += add.L_1_F;
		L_2_F += add.L_2_F;

		R_1_F += add.R_1_F;
		R_2_F += add.R_2_F;
	}

	HitData ApplyAlignment(const AlignmentData &al) const;
};

//----------------------------------------------------------------------------------------------------

struct EventRed
{
	unsigned int lumi_section;
	unsigned int timestamp;
	unsigned int run_num, bunch_num, event_num;

	// vertical RPs
	HitData h;
};

#endif
