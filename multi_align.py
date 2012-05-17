#!/usr/bin/env python

from os import path, environ
import subprocess

from Bio import SeqIO, AlignIO
import psipred

CACHE_DIR = 'sec'
DIALIGN_ROOT = 'dialign/dialign_package'
DIALIGN = path.join(DIALIGN_ROOT, 'src/dialign2-2')
DIALIGN_DIR = path.join(DIALIGN_ROOT, 'dialign2_dir')

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
			runs = subprocess.check_output([
				'./align',
				'--blosum', 'dialign-blosum.dat',
				'--weights', 'weights.txt',
				seqs[i].secname,
				seqs[j].secname
			])
			for line in runs.splitlines():
				print >>output, (i + 1), (j + 1), line


def dialign(filename):
	env = {'DIALIGN2_DIR': DIALIGN_DIR}
	env.update(environ)
	subprocess.check_call([
		DIALIGN,
		'-anc', # anchors
		'-fa', # fasta output
		filename
	], env = env)


if __name__ == '__main__':
	import sys
	name = sys.argv[1]
	seqs = list(structure(SeqIO.parse(name, "fasta")))

	basename, _ = path.splitext(name)
	with open(basename + '.anc', 'w') as f:
		anchors(seqs, f)
	dialign(name)
	aln = next(AlignIO.parse(basename + '.fa', 'fasta'))
	AlignIO.write(aln, sys.stdout, 'clustal')
