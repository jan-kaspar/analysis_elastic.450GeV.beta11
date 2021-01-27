#ifndef _common_algorithms_h_
#define _common_algorithms_h_

class HitData;
class Environment;
class Analysis;
class Kinematics;

#include <string>

//----------------------------------------------------------------------------------------------------

extern Kinematics DoReconstruction(const HitData &h, const Environment &env);

extern void BuildBinning(const Analysis &anal, const std::string &type, double* &binEdges, unsigned int &bins);

extern HitData ProtonTransport(const Kinematics & k, const Environment &env);

//----------------------------------------------------------------------------------------------------

extern double *th_x_binning_edges_1d, *th_y_binning_edges_1d;
extern double *th_x_binning_edges_2d, *th_y_binning_edges_2d;
extern double *th_x_binning_edges_2d_coarse, *th_y_binning_edges_2d_coarse;

extern int th_x_binning_n_1d, th_y_binning_n_1d;
extern int th_x_binning_n_2d, th_y_binning_n_2d;
extern int th_x_binning_n_2d_coarse, th_y_binning_n_2d_coarse;

extern void BuildThBinning();

#endif
