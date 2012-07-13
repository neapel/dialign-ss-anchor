#!/usr/bin/env python

from os import path, environ
import subprocess
from tempfile import NamedTemporaryFile

from Bio import SeqIO, AlignIO
import psipred
import dialign

def structure(seqs):
	''' Run PSIPRED to guess secondary structure '''
	for record in seqs:
		record.secname = psipred.cached(record)
		yield record


def anchors(seqs, output):
	"""
	Write anchors into the given file.
	"""
	for i in range(len(seqs)):
		for j in range(i):
			print 'anchors:', seqs[i].id, '&', seqs[j].id
			try:
				runs = subprocess.check_output([
					'./align',
					'--blosum', 'dialign-blosum.dat',
					'--weights', 'weights.txt',
					seqs[i].secname,
					seqs[j].secname
				], stderr=subprocess.STDOUT)
				for line in runs.splitlines():
					print >>output, (i + 1), (j + 1), line
			except subprocess.CalledProcessError, e:
				print 'error:', ' '.join(e.cmd)
				#raise e


def run(name, **kwargs):
	anc_f = NamedTemporaryFile()
	seqs = list(structure(SeqIO.parse(name, 'fasta')))
	anchors(seqs, anc_f)
	return dialign.run(name, anchors=anc_f.name, **kwargs)


if __name__ == '__main__':
	import sys
	out = run(sys.argv[1], fasta=True)['fasta']
	aln = next(AlignIO.parse(out, 'fasta'))
	AlignIO.write(aln, sys.stdout, 'clustal')
