#!/usr/bin/env python
"""
Convert a PileUp-MSF multiple alignment to FastA.

like `squizz -c fasta`
"""

from collections import OrderedDict
import sys
from Bio.Seq import Seq
from Bio.Align import MultipleSeqAlignment
from Bio.SeqRecord import SeqRecord
from Bio.Alphabet import generic_protein as alphabet

def read_msf(lines):
	data = None
	for line in sys.stdin:
		line = line.strip()
		if len(line) == 0:
			continue
		if data != None:
			fields = iter(line.split())
			name = next(fields)
			string = ''.join(fields).replace('.', '-')
			data[name] = data.get(name, '') + string
		elif line == '//':
			data = OrderedDict()

	return MultipleSeqAlignment([
		SeqRecord(Seq(string, alphabet), id=name, description='')
		for name, string in data.items()
	], alphabet)


if __name__ == '__main__':
	from Bio import AlignIO
	AlignIO.write(read_msf(sys.stdin), sys.stdout, 'fasta')
