# -*- coding: utf-8 -*-

from __future__ import division, print_function

import copy
from pprint import pprint
import sys


slotKWs = [
    'REFCHAN',
    'REFINDEX',
]

skipKws = [
    'MASK',
    'NSUBADD',
    'BSUB',
]


class Detector:
    def __init__(self):
        self.ID = -1
        self.comment = ''
        self.rocs = [Roc()]

    def __str__(self):
        string = 'DETECTOR={0.ID}'.format(self)
        if self.comment != '':
            string += '  ! {0.comment}'.format(self)

        return string


class Roc:
    def __init__(self):
        self.ID = -1
        self.comment = ''
        self.slots = [Slot()]

    def __str__(self):
        string = 'ROC={0.ID}'.format(self)
        if self.comment != '':
            string += '  ! {0.comment}'.format(self)

        return string


class Slot:
    def __init__(self):
        self.ID = -1
        self.comment = ''
        self.KWs = {}
        self.channels = []

        for kw in slotKWs:
            self.KWs[kw] = -1

    def __str__(self):
        string = 'SLOT={0.ID}'.format(self)
        if self.comment != '':
            string += '  ! {0.comment}'.format(self)
        string += '\n'
        for k, v in self.KWs.items():
            if v > -1:
                string += '{}={}\n'.format(k, v)

        return string


class Channel:
    def __init__(self, ID, plane, bar, signal=-1, comment=''):
        self.ID = ID
        self.plane = plane
        self.bar = bar
        self.signal = signal
        self.comment = comment

    def __str__(self):
        string = '{0.ID:>4},{0.plane:>4},{0.bar:>4}'.format(self)
        if self.signal >= 0:
            string += ',{0.signal:>4}'.format(self)
        if self.comment:
            string += '  ! {0.comment}'.format(self)

        return string


if (len(sys.argv) != 3 or sys.argv[1] == '-h'):
    print('Call as:')
    print('  merge_maps.py merge_list.txt outfile.map')
    sys.exit()


listName = sys.argv[1]
mergedName = sys.argv[2]


# Get map files to merge.
with open(listName, 'r') as fi:
    fileNames = [
        l.strip() for l in fi
        if not l.isspace() and not l.startswith('#')
    ]


header = {
    "KWs": [],
    "IDs": [],
    "signals": [],
}

detectors = []


for fileName in fileNames:
    # Track detector IDs in current file.
    currentIDs = []
    detectors.append(Detector())

    with open(fileName, 'r') as fi:
        for line in fi:
            # Skip empty lines.
            if line.isspace():
                continue

            line = line.strip()

            # Check if comment line.
            if line.startswith('!'):
                # Check if header line.
                # eg: ! HCAL_ID=4   ADC
                i = line.find('_ID=')
                if i == -1:
                    continue

                KW =  line[1:i].strip()
                ID = line[i+4:].split()[0].strip()
                signal = ','.join(line[i+4:].replace('::', '').split()[1:])

                if KW in header['KWs']:
                    print('Detector keyword `{}` already present!'.format(KW))
                    sys.exit(1)
                if ID in header['IDs']:
                    print('Detector ID `{}` already present!'.format(ID))
                    sys.exit(1)

                header['KWs'].append(KW)
                header['IDs'].append(ID)
                header['signals'].append(signal)
                currentIDs.append(ID)
                continue

            # Get comments.
            i = line.find('!')
            if i > -1:
                comment = line[i+1:].strip()
                line = ''.join(line[:i].split())
            else:
                line = ''.join(line.split())
                comment = ''

            # Check if there is keyword.
            i = line.find('=')
            if i > -1:
                command = line[:i].upper()
                ID = line[i+1:]

                if command == 'DETECTOR':
                    if detectors[-1].ID > 0:
                        detectors.append(Detector())
                    if ID not in currentIDs:
                        print('Detector ID `{}` not found!'.format(ID))
                        sys.exit(1)
                    detectors[-1].ID = ID
                    detectors[-1].comment = comment

                elif command == 'ROC':
                    if detectors[-1].rocs[-1].ID > 0:
                        detectors[-1].rocs.append(Roc())
                    detectors[-1].rocs[-1].ID = ID
                    detectors[-1].rocs[-1].comment = comment

                elif command == 'SLOT':
                    if detectors[-1].rocs[-1].slots[-1].ID > 0:
                        detectors[-1].rocs[-1].slots.append(Slot())
                    detectors[-1].rocs[-1].slots[-1].ID = ID
                    detectors[-1].rocs[-1].slots[-1].comment = comment

                elif command in slotKWs:
                    detectors[-1].rocs[-1].slots[-1].KWs[command] = ID

                elif command in skipKWs:
                    pass

                else:
                    print('Unknown command `{}` in line:'.format(command))
                    print(line)

                continue

            # This must be channel line.
            values = line.split(',')
            detectors[-1].rocs[-1].slots[-1].channels.append(
                Channel(*values, comment=comment)
            )

# Check if each detector has defined at least one ROC and slot.
for ID, KW in sorted(zip(header['IDs'], header['KWs'])):
    found = False
    for detector in detectors:
        if detector.ID == ID:
            found = True

            if detector.rocs[0].ID < 0:
                print('No crate defined for detector {}!'.format(ID))
                sys.exit(1)

            for roc in detector.rocs:
                if roc.slots[0].ID < 0:
                    print(
                        'No slot defined for crate {} in detector {}!'.format(
                            roc.ID, detector.ID
                        )
                    )
                    sys.exit(1)

    if not found:
        print('No entry for detector {}!'.format(ID))


# Write merged map file.
with open(mergedName, 'w') as fo:
    # Write header.
    for ID, KW, signals in sorted(zip(
        header['IDs'], header['KWs'], header['signals']
    )):
        IDString = '{0}_ID={1}'.format(KW, ID)
        fo.write('! {0:15}  ::  {1}\n'.format(IDString, signals))

    # Write detector maps sorted by IDs.
    for ID, KW in sorted(zip(
        header['IDs'], header['KWs']
    )):
        for detector in detectors:
            if detector.ID == ID:
                fo.write('\n\n{0}\n'.format(detector))

                for roc in detector.rocs:
                    fo.write('\n{0}\n'.format(roc))

                    for slot in roc.slots:
                        fo.write('\n{0}'.format(slot))

                        for channel in slot.channels:
                            fo.write('{0}\n'.format(channel))


print('\nDone.')
