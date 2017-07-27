#!/usr/bin/python

# Python script to merge multiple def-files into single def-file
import os
import sys

if (len(sys.argv) != 3 or sys.argv[1] == '-h'):
    print('Usage:')
    print('  merge_def_files.py merge_list.txt outfile.def')
    sys.exit()

defFileList   = open(sys.argv[1], 'r')
defFile       = defFileList.readlines()
defFilePath   = []
mergedDefFile = sys.argv[2]

for defFileName in defFile:
    defFilePath.append((defFileName).strip())

with open(mergedDefFile, 'w') as outFile:
    for defFileName in defFilePath:
        outFile.write('\n\n')
        with open(defFileName) as inFile:
            for line in inFile:
                outFile.write(line)
