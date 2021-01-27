#ifndef _common_init_hh_
#define _common_init_hh_

#include "Config.hh"
#include "Environment.hh"
#include "Analysis.hh"

extern Config cfg;
extern Environment env;
extern Analysis anal;

extern const int rcIncompatibleDiagonal;

//----------------------------------------------------------------------------------------------------

extern int Init(const string &cfg_file, const string &diagonal_input);

extern void PrintConfiguration();

#endif
