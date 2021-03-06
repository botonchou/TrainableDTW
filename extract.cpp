#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <map>

#include <archive_io.h>
#include <cmdparser.h>

using namespace std;

int main(int argc, char* argv[]) {

  CmdParser cmdParser(argc, argv);
  cmdParser
    .addGroup("Generic options:")
    .add("-a", "filename of alignments")
    .add("-p", "phone table")
    .add("-m", "model")
    .add("--feat-ark", "feature archive where mfcc extracted from")
    .add("--mfcc-output-folder", "destination folder for saving mfcc files");

  cmdParser
    .addGroup("Examples: ./extract -a data/train.ali.txt -p data/phones.txt"
	" -m data/final.mdl --feat-ark=/share/LectureDSP_script/feat/train.39.ark"
	" --mfcc-output-folder=data/mfcc/");

  if(!cmdParser.isOptionLegal())
    cmdParser.showUsageAndExit();

  string alignmentFile = cmdParser.find("-a");
  string phoneTableFile = cmdParser.find("-p");
  string modelFile = cmdParser.find("-m");
  string featArk = cmdParser.find("--feat-ark");
  string outputFolder = cmdParser.find("--mfcc-output-folder");

  debug(alignmentFile);
  debug(phoneTableFile);
  debug(modelFile);

  vector<string> phones = getPhoneMapping(phoneTableFile);

  printf("Loading alignments and model...\n");
  map<string, vector<Phone> > phoneLabels;
  size_t nInstance = load(alignmentFile, modelFile, phoneLabels);
  printf(GREEN"[Done]"COLOREND"\n");

  map<size_t, vector<FeatureSeq> > phoneInstances;

  if (featArk.empty()) {
    printf("No feature archive provided.\n");
    return 0;
  }

  printf("Loading feature archive...\n");
  size_t n = loadFeatureArchive(featArk, phoneLabels, phoneInstances);
  check_equal(n, nInstance);
  printf(GREEN"[Done]"COLOREND"\n");

  printf("Saving feature sequences into files...\n");
  size_t nMfccFiles = saveFeatureAsMFCC(phoneInstances, phones, outputFolder);
  check_equal(nMfccFiles, nInstance);
  printf(GREEN"[Done]"COLOREND"\n");

  return 0;
}


