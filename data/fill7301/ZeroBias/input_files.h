#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------

std::vector<std::string> input_files;

void InitInputFiles()
{
	input_files = {
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.0_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.10_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.11_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.15_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.16_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.17_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.19_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.20_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.21_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.22_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.24_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.25_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.26_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.27_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.28_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.29_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.2_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.3_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.5_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.6_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.7_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.8_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324575.9_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.0_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.10_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.14_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.15_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.16_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.18_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.19_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.20_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.23_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.24_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.25_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.28_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.2_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.5_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.7_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version2/run_324576.8_re_reco_ZeroBias.root"
	};
}
