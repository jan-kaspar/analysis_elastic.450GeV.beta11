#ifndef _config_hh_
#define _config_hh_

namespace edm
{
	class ParameterSet;
}

#include <vector>
#include <string>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

enum DiagonalType { dUnknown, d45b_56t, d45t_56b, dCombined, ad45b_56b, ad45t_56t };

//----------------------------------------------------------------------------------------------------

struct Config
{
	DiagonalType diagonal = dUnknown;
	string diagonal_str;
	string python_object;

	// +1 for 45b_56t, -1 for 45t_56b
	double th_y_sign = 0.;

	// list of input EDM files
	vector<string> input_files;

	// list of distilled file directories
	vector<string> distilled_files;

	// global offset between UNIX time and timestamps used in the analysis
	double timestamp0;

	// range of timestamps (UNIX time - timestamp0)
	double timestamp_min, timestamp_max;

	// directory with associated Timber data
	string timber_dir;

	void Load(const edm::ParameterSet &ps);

	void Print() const;
};

#endif
