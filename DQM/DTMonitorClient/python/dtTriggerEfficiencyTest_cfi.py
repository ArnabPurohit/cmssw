import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

triggerEffTest = DQMEDHarvester("DTTriggerEfficiencyTest",
    # prescale factor (in luminosity blocks) to perform client analysis
    diagnosticPrescale = cms.untracked.int32(1),
    # run in online environment
    runOnline = cms.untracked.bool(False),
    # kind of trigger data processed by DTLocalTriggerTask
    hwSources = cms.untracked.vstring('TM','DDU'),
    # false if DTLocalTriggerTask used LTC digis
    localrun = cms.untracked.bool(True),
    # root folder for booking of histograms
    folderRoot = cms.untracked.string(''),
    #enable generation of chamber granularity plots
    detailedAnalysis = cms.untracked.bool(False)                                  
)

from Configuration.Eras.Modifier_run2_common_cff import run2_common
run2_common.toModify( triggerEffTest, hwSources = cms.untracked.vstring('TM'))

