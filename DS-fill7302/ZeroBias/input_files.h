#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------

std::vector<std::string> input_files;

void InitInputFiles()
{
	input_files = {
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.0_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.10_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.11_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.12_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.13_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.14_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.15_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.16_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.17_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.18_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.19_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.1_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.20_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.21_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.22_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.23_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.24_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.25_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.26_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.27_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.28_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.29_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.2_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.30_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.31_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.32_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.33_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.34_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.35_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.36_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.37_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.38_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.3_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.4_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.5_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.6_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.7_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.8_re_reco_ZeroBias.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/ZeroBias/version1/run_324579.9_re_reco_ZeroBias.root"
	};
}
