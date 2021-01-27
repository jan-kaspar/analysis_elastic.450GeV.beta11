#include "common_event.hh"

//----------------------------------------------------------------------------------------------------

HitData HitData::ApplyAlignment(const AlignmentData &al) const
{
	HitData r;

	r.L_2_F.x = L_2_F.x - al.a_L_2_F * L_2_F.y - al.b_L_2_F;
	r.L_2_F.y = L_2_F.y + al.a_L_2_F * L_2_F.x - al.c_L_2_F;

	r.L_1_F.x = L_1_F.x - al.a_L_1_F * L_1_F.y - al.b_L_1_F;
	r.L_1_F.y = L_1_F.y + al.a_L_1_F * L_1_F.x - al.c_L_1_F;

	r.R_1_F.x = R_1_F.x - al.a_R_1_F * R_1_F.y - al.b_R_1_F;
	r.R_1_F.y = R_1_F.y + al.a_R_1_F * R_1_F.x - al.c_R_1_F;

	r.R_2_F.x = R_2_F.x - al.a_R_2_F * R_2_F.y - al.b_R_2_F;
	r.R_2_F.y = R_2_F.y + al.a_R_2_F * R_2_F.x - al.c_R_2_F;

	return r;
}
