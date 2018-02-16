#!/usr/bin/python

import argparse

def get_args():
    '''This function parses and returns arguments passed in'''
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-r','--runs',type=str,help='Run list',required=True,nargs="+")
    parser.add_argument(
        '-f','--infile',type=str,help='Scaler list file',required=True)
    parser.add_argument(
        '-o','--outfile',type=str,help='Output file',required=True)
    args = parser.parse_args()
    infilename = args.infile
    outfilename = args.outfile
    runlist = args.runs[0].split(",")

    return runlist, infilename, outfilename

runlist, infilename, outfilename = get_args()

print "\nScaler File Name: [ %s ]\n" % infilename
print "\nOutput File Name: [ %s ]\n" % outfilename

final_runlist = []
for r in runlist:
    minirunlist = r.split("-")
    nruns = len(minirunlist)
    if nruns == 1:
       #print "Run: [%s]" % r
       scaler_filename = "replay_coin_scalers_" + r + ".report"
       final_runlist.append(scaler_filename)
    else:
       startrun=int(minirunlist[0])
       endrun=int(minirunlist[1])
       while startrun <= endrun:
           #print "MiniRun: [%s]" % startrun
           scaler_filename = "replay_coin_scalers_" + str(startrun) + ".report"
           final_runlist.append(scaler_filename)
           startrun=startrun+1

#print final_runlist

basic_name_array = ["Run Number"]
scaler_name_array = []
with open(infilename) as fp:
    scaler_name_array = fp.read().splitlines()
fp.close()

of = open(outfilename,'w')
scaler_name_array_run_number = basic_name_array+scaler_name_array
mx = len(max(scaler_name_array_run_number))
print (" ".join(["{:<{mx}}".format(ele,mx=mx) for ele in scaler_name_array_run_number]))
of.write(" ".join(["{:<{mx}}".format(ele,mx=mx) for ele in scaler_name_array_run_number]))
of.write("\n")

for runfile in final_runlist:
    with open(runfile) as fp:
        runfile_linelist = fp.read().splitlines()
	runnumbersplit = runfile.split("_")
	runnumbersplit2 = runnumbersplit[3].split(".")
        run_number_array=[]
        run_number_array.append(runnumbersplit2[0])
	scaler_value_array=[]
        for line in runfile_linelist:
            parsed_line = line.split(":")
            for scaler in scaler_name_array:
                	if parsed_line[0].rstrip(" ") == scaler:
			    full_value = parsed_line[1].lstrip()
		            numerical_value = full_value.split(" ")
                            scaler_value_array.append(numerical_value[0])
        scaler_value_array_run_number=run_number_array+scaler_value_array
        print (" ".join(["{:<{mx}}".format(ele,mx=mx) for ele in scaler_value_array_run_number]))
        of.write(" ".join(["{:<{mx}}".format(ele,mx=mx) for ele in scaler_value_array_run_number]))
        of.write("\n")
        fp.close()

of.close()
