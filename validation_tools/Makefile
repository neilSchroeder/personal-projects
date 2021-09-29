CFLAGS = -pthread -m64 -Wno-deprecated -lboost_program_options -lRooFit -lRooFitCore 
CFLAGS += $(shell root-config --cflags --libs)
HERE = $(shell pwd)

all: Check1 Check8

Check1: src/plot_StandardSet.cc 
	g++ -g -o bin/plot_StandardSet -g src/plot_StandardSet.cc ${HERE}/src/utilities.cc ${HERE}/src/statistic.cc ${HERE}/src/standardSet.cc ${CFLAGS}
Check2: src/plot_massSummary.cc
	g++ -o bin/plot_massSummary src/plot_massSummary.cc ${HERE}/src/utilities.cc ${HERE}/src/statistic.cc ${CFLAGS}
Check3: src/plot_massScan.cc
	g++ -o bin/plot_massScan src/plot_massScan.cc ${HERE}/src/utilities.cc ${HERE}/src/statistic.cc ${CFLAGS}
Check4: src/dynamicBin.cc
	g++ -o bin/dynamicBin src/dynamicBin.cc ${CFLAGS}
Check5: src/producer_scaleSmearHists_fgg.cc
	g++ -g -o bin/producer_scaleSmearHists_fgg src/producer_scaleSmearHists_fgg.cc src/EnergyScaleCorrection.cc ${CFLAGS}
Check6: src/producer_hggHists.cc
	g++ -g -o bin/producer_hggHists src/producer_hggHists.cc ${HERE}/src/utilities.cc ${HERE}/src/statistic.cc ${CFLAGS}
Check7: scripts/plot_trainPlots.cc
	g++ -o bin/plot_trainPlots scripts/plot_trainPlots.cc ${HERE}/src/utilities.cc ${HERE}/src/statistic.cc ${CFLAGS}
Check8: src/producer_scaleSmearHists.cc
	g++ -g -o bin/producer_scaleSmearHists src/producer_scaleSmearHists.cc src/EnergyScaleCorrection_class.cc ${CFLAGS}
Check9: src/write_filesWithCorrVals.cc
	g++ -g -o bin/write_filesWithCorrVals src/write_filesWithCorrVals.cc src/EnergyScaleCorrection_class.cc ${CFLAGS}
