# -*- coding: utf-8 -*-

from __future__ import division, print_function

import sys


class Detector:
    def __init__(self):
        self.ID = -1
        self.slots = []


class Slot:
    def __init__(self):
        self.num = -1
        # These go in front of SLOT number.
        self.KWs1 = {
            'ROC': -1,
        }
        # These go after the SLOT number.
        self.KWs2 = {
            'REFCHAN': -1,
            'REFINDEX': -1
        }
        # These are ignored.
        self.SkipKWs = set([
            'MASK',
            'NSUBADD',
            'BSUB',
        ])
        self.entries = []


listName = sys.argv[1]
mergedName = sys.argv[2]


# Get map files to merge.
with open(listName, 'r') as fi:
    fileNames = [
        l.strip() for l in fi
        if not l.isspace() and not l.startswith('#')
    ]


# Header information.
KWs = []
IDs = []
signals = []

# Detector information.
detectors = []


for fileName in fileNames:
    detector = Detector()
    slot = Slot()

    # Track detector IDs in current file.
    IDsCurrent = []

    with open(fileName, 'r') as fi:
        for line in fi:
            # Skip empty lines.
            if line.isspace():
                continue

            # Check if comment line.
            if line.startswith('!'):
                # Check if header line.
                # eg: ! HCAL_ID=4   ADC
                i = line.find('_ID=')
                if i == -1:
                    continue
                else:
                    KW = line[1:i].strip()
                    ID = line[i+4:].split()[0].strip()
                    signal = ','.join(line[i+4:].split()[1:])

                    if KW in KWs:
                        print('Detector keyword `{}` already present!'.format(KW))
                        sys.exit(1)
                    if ID in IDs:
                        print('Detector ID `{}` already present!'.format(ID))
                        sys.exit(1)

                    KWs.append(KW)
                    IDs.append(ID)
                    IDsCurrent.append(ID)
                    signals.append(signal)
                    continue

            line = line.strip()

            # Get comments.
            i = line.find('!')
            if i > -1:
                comment = line[i+1:].strip()
                line = ''.join(line[:i].split())
            else:
                line = ''.join(line.split())

            # Check if there is keyword.
            i = line.find('=')
            if i > -1:
                command = line[:i].upper()
                value = line[i+1:]

                if command in slot.KWs1.keys():
                    slot.KWs1[command] = value
                elif command in slot.KWs2.keys():
                    slot.KWs2[command] = value
                elif command in slot.SkipKWs:
                    continue

                elif command == 'DETECTOR':
                    if detector.ID > 0:
                        detectors.append(detector)
                        detector = Detector()
                    if value not in IDsCurrent:
                        print('Detector ID `{}` not found!'.format(value))
                        sys.exit()
                    detector.ID = value

                elif command == 'SLOT':
                    if slot.num > 0:
                        detector.slots.append(slot)
                        slot = Slot()
                    slot.num = value

                else:
                    print('Unknown command `{}` in line:'.format(command))
                    print(line)
                continue

            values = line.split(',')
            values = ','.join(['{:>4}'.format(val) for val in values])
            slot.entries.append('{}    ! {}'.format(values, comment))

    detectors.append(detector)


# Check if each detector has at least one slot and ROC defined.
for ID, KW in sorted(zip(IDs, KWs)):
    found = False
    for detector in detectors:
        if detector.ID == ID:
            found = True

            if len(detector.slots) == 0:
                print('Detector with ID {} has no slots!'.format(ID))
                sys.exit(1)
            if detector.slots[0].KWs1['ROC'] == -1:
                print('No crate defined for detector with ID {}!'.format(ID))
                sys.exit(1)

    if not found:
        print('No entry for detector with ID {}!'.format(ID))


with open(mergedName, 'w') as fo:
    # Write header.
    for ID, KW, signal in sorted(zip(IDs, KWs, signals)):
        IDString = '{}_ID={}'.format(KW, ID)
        fo.write('! {:15}  ::  {}\n'.format(IDString, signal))

    # Write detectors sorted by their ID.
    for ID, KW in sorted(zip(IDs, KWs)):
        for detector in detectors:
            if detector.ID == ID:

                fo.write('\n\n')
                fo.write('DETECTOR={0.ID}  ! {1}\n'.format(detector, KW))

                for slot in detector.slots:
                    fo.write('\n')

                    for key, value in sorted(slot.KWs1.items()):
                        if value > 0:
                            fo.write('{}={}\n'.format(key, value))

                    fo.write('SLOT={0.num}\n'.format(slot))

                    for key, value in sorted(slot.KWs2.items()):
                        if value > 0:
                            fo.write('{}={}\n'.format(key, value))

                    for entry in slot.entries:
                        fo.write('{}\n'.format(entry))

                break
