#!/usr/bin/python

# Generate hcana helicity scaler definition files from the xscaler scaler map file

xscalerMapName = 'scaler_hel.map'

cratemap = {
    "4":{"spec":"H", "firstslot":19, "nslots":1, "roc":5, "offset":0, "clockChan":8, "clockSlot":19},
    "5":{"spec":"P", "firstslot":4, "nslots":1, "roc":8, "offset":640, "clockChan":8, "clockSlot":4}
    }
chandict   = {}
nperslot   = 32
clockrate  = 1000000

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

        splitline = line.split(None,7)
        helicity = splitline[1]
        spec = cratemap[splitline[2]]["spec"]
        name = spec+splitline[0]
        #slot = int(splitline[3])+cratemap[splitline[2]]["firstslot"]
        slot = int(splitline[3])
        start = int(splitline[4])
        nchan = splitline[5] # This better be 1
        page = splitline[6] # We ignore this here
        comment = splitline[7]
        if comment == "Empty":  # Overwrite name used by xscaler
            name = spec+"Empty"
        uniquename = name
        count = 2
        if uniquename in chandict:
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
        hcanaMapName = 'db_hel'+specprefix+'Scalevt.dat'
        clockChan = cratemap[spec]["clockChan"]
        clockSlot = cratemap[spec]["clockSlot"]
        with open(hcanaMapName, 'w') as fo:
            for slot in range(firstslot,firstslot+nslots):
                if slot == clockSlot:
                    rateinfo=' {0} {1}'.format(clockChan,clockrate)
                else:
                    rateinfo=''
                print ('map 3801 {0} {1} {2:04x}{1:02x}{3:02x} ffffffff {4}'.\
                format(roc, slot, offset+(slot-firstslot)*nperslot,nperslot,clockSlot)\
                       +rateinfo, file=fo)

            for name in chandict:
                channel = chandict[name]
                slot = channel.slot-firstslot
                printname = "." + name[1:] + ".scaler" # Drop prefix
                detPrefix = name[2:5]
                if specprefix == channel.spec and slot<nslots:
                    chan = channel.chan
                    # slot = channel.slot-firstslot
                    slot = channel.slot
                    comment = channel.comment
                    if detPrefix == "hod":
                        printHodoName = "." + detPrefix + "." + name[5:10] + "."
                        if printHodoName.find("+") != -1:
                            printname = printHodoName.replace("+", "")+"posScaler"
                        if printHodoName.find("-") != -1:
                            printname = printHodoName.replace("-", "")+"negScaler"
                    print('variable', slot, chan, 1, printname, comment, file=fo)
                    print('variable', slot, chan, 2, printname+'Rate', comment, file=fo)
                    if name[1:4] == "BCM" or name[1:6] == "Unser":
                        print('variable', slot, chan, 3, printname+'Current', comment, file=fo)
                        print('variable', slot, chan, 4, printname+'Charge', comment, file=fo)
                    elif name[1:5] == "1MHz":
                        print('variable', slot, chan, 5, printname+'Time', comment, file=fo)
