#!/usr/bin/perl

use File::Slurp;


my $file = read_file($ARGV[0]);

$file =~ s/\Qchar RunFileName[100];\E\W*\Qsprintf(RunFileName, RunFileNamePattern, RunNumber);\E\W*\QTHaRun* run = new THaRun(RunFileName);\E/THaRun* run = new THaRun( pathList, Form(RunFileNamePattern, RunNumber) );/s;

$file =~ s/\Qconst char* RunFileNamePattern = "\E.*?\/(.*?\.dat)";/const char* RunFileNamePattern = "$1";\n  vector<TString> pathList;\n    pathList.push_back(".");\n    pathList.push_back(".\/raw");\n    pathList.push_back(".\/cache");\n/s;

#$file =~ s/\Qconst char* RunFileNamePattern = "raw\/\E(.*?%05d.dat)\Q";\E/


write_file("$ARGV[0]", $file);

