#include "Kinematics.hh"

#include "Environment.hh"

//----------------------------------------------------------------------------------------------------

void Kinematics::ThetasToTPhi(const Environment &env)
{
	th = sqrt(th_x*th_x + th_y*th_y);
	t_x = env.p*env.p * th_x * th_x;
	t_y = env.p*env.p * th_y * th_y;
	t = t_x + t_y;
	phi = atan2(th_y, th_x);
}

//----------------------------------------------------------------------------------------------------

void Kinematics::TPhiToThetas(const Environment &env)
{
	th = sqrt(t) / env.p;
	th_x_L = th_x_R = th_x = th * cos(phi);
	th_y_L = th_y_R = th_y = th * sin(phi);

	t_x = t * cos(phi) * cos(phi);
	t_y = t * sin(phi) * sin(phi);
}
