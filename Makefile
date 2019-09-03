CFLAGS = -pthread -m64 -Wno-deprecated -lboost_program_options
CFLAGS += $(shell root-config --cflags --libs)
HERE = $(shell pwd)

all: Check3

Check1: src/plot_StandardSet.cc 
	g++ -o bin/plot_StandardSet src/plot_StandardSet.cc ${HERE}/src/utilities.cc ${HERE}/src/statistic.cc ${HERE}/src/standardSet.cc ${CFLAGS}
Check2: src/plot_massSummary.cc
	g++ -o bin/plot_massSummary src/plot_massSummary.cc ${HERE}/src/utilities.cc ${HERE}/src/statistic.cc ${CFLAGS}
Check3: src/plot_massScan.cc
	g++ -o bin/plot_massScan src/plot_massScan.cc ${HERE}/src/utilities.cc ${HERE}/src/statistic.cc ${CFLAGS}
Check4: src/dynamicBin.cc
	g++ -o bin/dynamicBin src/dynamicBin.cc ${CFLAGS}
Check5: src/producer_scaleSmearHists.cc
	g++ -o bin/producer_scaleSmearHists src/producer_scaleSmearHists.cc /afs/cern.ch/work/n/nschroed/ECALELF/CMSSW_9_4_9_cand1/src/RecoEgamma/EgammaTools/src/EnergyScaleCorrection.cc ${CFLAGS}
