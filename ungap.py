#!/usr/bin/env python
"""
FastA: Remove '.' and '-' gaps from sequence, return as upper case.
"""

import sys
from Bio import AlignIO, SeqIO
from Bio.SeqRecord import SeqRecord

def ungap(data):
	return [
		SeqRecord(rec.seq.ungap('-').ungap('.').upper(), id=rec.id, description=rec.description)
		for rec in data
	]

if __name__ == '__main__':
	data = AlignIO.read(sys.stdin, 'fasta')
	SeqIO.write(ungap(data), sys.stdout, 'fasta')
