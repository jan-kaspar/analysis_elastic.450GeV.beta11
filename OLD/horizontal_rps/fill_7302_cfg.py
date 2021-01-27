import FWCore.ParameterSet.Config as cms

process = cms.Process('CTPPSTest')

# number of events
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(-1)
)

# minimum of logs
process.MessageLogger = cms.Service("MessageLogger",
  statistics = cms.untracked.vstring(),
  destinations = cms.untracked.vstring("cout"),
  cout = cms.untracked.PSet(
    threshold = cms.untracked.string("WARNING")
  )
)

# data source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
	"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version4/run_324579.0_re_reco_Totem1.root",
	"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version4/run_324579.1_re_reco_Totem1.root",
	"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version4/run_324579.2_re_reco_Totem1.root"
  )
)

# track distribution plotter
process.ctppsTrackDistributionPlotter = cms.EDAnalyzer("CTPPSTrackDistributionPlotter",
  tagTracks = cms.InputTag("ctppsLocalTrackLiteProducer"),

  rpId_45_F = cms.uint32(23),
  rpId_45_N = cms.uint32(3),
  rpId_56_N = cms.uint32(103),
  rpId_56_F = cms.uint32(123),

  outputFile = cms.string("tracks_7302.root")
)

# processing path
process.p = cms.Path(
  process.ctppsTrackDistributionPlotter
)
