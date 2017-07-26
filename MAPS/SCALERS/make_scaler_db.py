#!/usr/bin/python

# Deal with args

xscalerMapName = 'scaler.map'

cratemap = {
    "4":{"spec":"H", "firstslot":6, "nslots":7, "roc":5, "offset":0},
    "5":{"spec":"P", "firstslot":6, "nslots":8, "roc":8, "offset":640}
    }
chandict  = {}
nperslot  = 32
clockrate = 1000000

class Channel:
    def __init__(self, spec, slot, chan, comment=''):
        self.spec = spec
        self.slot = slot
        self.chan = chan
        self.comment = comment
    def __str__(self):
        string = self.spec+"."+str(self.slot)+"."+str(self.chan)+"."+self.comment
        return string

with open(xscalerMapName, 'r') as fi:
    for line in fi:
        # Skip empty lines
        if line.isspace():
            continue

        line = line.strip()

        # Check if comment line
        if line.startswith('#'):
            continue
        if line.startswith('DATE') or line.startswith('xscaler-'):
            continue

        splitline = line.split(None,6)
        helicity = splitline[1]
        spec = cratemap[splitline[2]]["spec"]
        name = spec+splitline[0]
        slot = int(splitline[3])+cratemap[splitline[2]]["firstslot"]
        start = int(splitline[4])
        nchan = splitline[5] # This better be 1
        comment = splitline[6]
        uniquename = name
        count = 2
        while chandict.has_key(uniquename):
            uniquename = name+"_"+str(count)
            count += 1
        chandict[uniquename] = Channel(cratemap[splitline[2]]["spec"],slot,start,comment)

    for spec in cratemap:
        firstslot = cratemap[spec]["firstslot"]
        nslots = cratemap[spec]["nslots"]
        roc = cratemap[spec]["roc"]
        lastslot = firstslot + nslots - 1
        offset = cratemap[spec]["offset"]
        specprefix = cratemap[spec]["spec"]
        hcanaMapName = 'db_'+specprefix+'Scalevt.dat'
        with open(hcanaMapName, 'w') as fo:
            for slot in range(firstslot,firstslot+nslots):
                if slot == lastslot:
                    rateinfo=' {0} {1}'.format(nperslot-1,clockrate)
                else:
                    rateinfo=''
                print >>fo, 'map 3801 {0} {1} {2:04x}{1:02x}{3:02x} ffffffff {4}'.\
                format(roc, slot, offset+(slot-firstslot)*nperslot,nperslot,lastslot)\
                +rateinfo

            for name in chandict:
                channel = chandict[name]
                slot = channel.slot-firstslot
                printname = "." + name[1:] + ".scaler" # Drop prefix
                detPrefix = name[2:5]
                printHodoName = "." + detPrefix + "." + name[5:10] + "."
                if specprefix == channel.spec and slot<nslots:
                    chan = channel.chan
                    slot = channel.slot-firstslot
                    comment = channel.comment
                    if detPrefix == "hod":
                        if printHodoName.find("+") != -1:
                            printHodoName = printHodoName.replace("+", "")
                            printHodoName = printHodoName + "posScaler"
                        if printHodoName.find("-") != -1:
                            printHodoName = printHodoName.replace("-", "")
                            printHodoName = printHodoName + "negScaler"
                        print >>fo, 'variable', slot, chan, 1, printHodoName, comment
                        print >>fo, 'variable', slot, chan, 2, printHodoName+'Rate', comment
                    else:    
                        print >>fo, 'variable', slot, chan, 1, printname, comment
                        print >>fo, 'variable', slot, chan, 2, printname+'Rate', comment
