#!/afs/cern.ch/user/n/nschroed/anaconda3/bin/python

import os
import sys
from optparse import OptionParser
import pandas as pd
import numpy as np

""" Command Line Options """
parser = OptionParser()
parser.add_option("-i",
        "--inputFile", 
        help="Input file: takes a text file containing the path to two root files prefaced by their type (data or mc)")
parser.add_option("-o",
        "--outputFile",
        help="output string, do not add a suffix or prefix")
parser.add_option("--process", 
        help="Process: if you only want to run one of the processes (producer or plotter) you can use this option to limit which program runs")
parser.add_option("--transformR9", 
        action="store_true", dest="transformR9", default=False,
        help="[flag] this will turn on the option of transforming R9 in the producer")
parser.add_option("--makeTree",
        action="store_true", dest="makeTree", default=False,
        help="[flag] turns on the option to produce a root tree alongside the histograms")
parser.add_option("--mediumID",
        action="store_true", dest="mediumID", default=False,
        help="[flag] turns on the application of the medium electron working point")
parser.add_option("--tightID",
        action="store_true", dest="tightID", default=False,
        help="[flag] turns on the application of the tight electron working point")
parser.add_option("--dataTitle",
        help="Legend entry for the data, if left empty the program will attempt to fill it based on the information in the name of the file provided")
parser.add_option("--mcTitle",
        help="Legend entry for the mc, if left empty the program will attempt to fill it based on the information in the name of the file provided")
parser.add_option("-q",
        "--quantity",
        help="Quantity you wish to plot, default is median invariant mass. your options are \n'median' \n'mean' \n'stddev' \n'resolution'")
parser.add_option("--lumiLabel", 
        help="Luminosity Label: this will be used as the label at the upper right corner of the plot e.g. `#font[42]{### fb^{-1} ### TeV (YEAR)`")
parser.add_option("--plotEta", 
        action="store_true", dest="plotEta", default=False,
        help="[flag] turns off the `plot all` feature and plot the `eta` portion of the standard suite of plots")
parser.add_option("--plotEtaDoubRatio", 
        action="store_true", dest="plotEtaDoubRatio", default=False,
        help="[flag] turns off the `plot all` feature and plots the `eta double ratio` portion of the standard suite of plots (requires 4 input files)")
parser.add_option("--plotCat",
        action="store_true", dest="plotCat", default=False,
        help="[flag] turns off the `plot all` feature and plots the `category` portion of the standard suite of plots")
parser.add_option("--plotEt",
        action="store_true", dest="plotEt", default=False,
        help="[flag] turns off the `plot all` feature and plots the `et` port of the standard suite of plots")
parser.add_option("--plotR9",
        action="store_true", dest="plotR9", default=False,
        help="[flag] turns off the `plot all` feature and plots the `r9` portion of the standard suite of plots")
parser.add_option("--invMass",
        action="store_true", dest="invMass", default=False,
        help="[flag] turns on the option to plot the invariant mass distributions used in the validation plots")

(opts, args) = parser.parse_args()

prefix = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/rootFiles/scalesSmearsHists_"

opt_makeTree = ""
if opts.makeTree: opt_makeTree = "--makeTree "

opt_transR9 = ""
if opts.transformR9: opt_transR9 = "--transformR9 "

opt_mediumID = ""
if opts.mediumID: opt_mediumID = "--mediumID "

opt_tightID = ""
if opts.tightID: opt_tightID = "--tightID "

opt_eta = ""
if opts.plotEta: opt_eta = "--plotEta "

opt_etaDR = ""
if opts.plotEta: opt_etaDR = "--plotEtaDoubRatio "

opt_cat = ""
if opts.plotCat: opt_cat = "--plotCat "

opt_et = ""
if opts.plotEt: opt_et = "--plotEt "

opt_r9 = ""
if opts.plotR9: opt_r9 = "--plotR9 "

opt_invMass = ""
if opts.invMass: opt_invMass = "--invMassDist"

"""open in input file and identify the data and MC files"""
files = pd.read_csv(opts.inputFile, sep=" ")
files = np.matrix(files)


"""run the producer for the data and the mc separately"""
if opts.process == "all" or opts.process == "producer":
    os.system("./bin/producer_scaleSmearHists -f "+opts.inputFile+" -o "+opts.outputFile+" "+opt_makeTree+opt_transR9+opt_mediumID+opt_tightID)

"""run the plotter on the output file of the data and mc
*** the producer should write the complete file path
*** of the data and mc output root files to an 'output.dat'
*** file. """

if opts.process == "all" or opts.process == "plotter":

    if opts.process is "all": plotFile = pd.read_csv("producer_scaleSmearHists_"+outputFile+".dat")
    else: plotFile = pd.read_csv(opts.inputFile)

    plotFile = np.matrix(plotFile)
    for row in plotFile:
        row = np.ravel(row)
        os.system(str("./bin/plot_StandardSet -i "+row[0]+" -o "+opts.outputFile+" -q "+opts.quantity+" -d "+opts.dataTitle+" -m "+opts.mcTitle+" --lumiLabel='"+opts.lumiLabel+"' "+opt_eta+opt_etaDR+opt_cat+opt_et+opt_r9+opt_invMass))
