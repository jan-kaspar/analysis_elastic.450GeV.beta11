#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------

std::vector<std::string> input_files;

void InitInputFiles()
{
	input_files = {
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version3/run_324575.0_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version3/run_324575.1_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version3/run_324576.0_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version3/run_324576.1_re_reco_Totem1.root"
	};
}
