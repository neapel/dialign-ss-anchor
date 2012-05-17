#!/usr/bin/env python
"""
Run PSIPRED on PSI-BLAST output.
"""

import sys
from subprocess import *
from os import path, mkdir
from Bio import SeqIO

# The name of the BLAST+ data bank
DBNAME = 'psipred/db/uniref90filt'

PSIDIR = 'psipred'

# Where the PSIPRED V3 programs have been installed
PSIPRED = path.join(PSIDIR, 'bin/psipred')

# Where the PSIPRED V3 data files have been installed
DATADIR = path.join(PSIDIR, 'data')

# Where to cache PSSM, PSIPRED output
CACHEDIR = 'cache'


def run_blast(seq, checkpoint_file):
	''' Run a BLAST search for input sequence, write PSSM. '''
	blast = Popen([
		'psiblast',
		'-db', DBNAME,
		'-query', '-', # stdin
		'-inclusion_ethresh', '0.001',
		'-out_pssm', checkpoint_file,
		'-num_iterations', '3',
		'-num_alignments', '0', # show no alignments
		'-num_threads', '4', # parallel threads to use
		'-out', '/dev/null', # quiet
	], stdin=PIPE)

	SeqIO.write(seq, blast.stdin, 'fasta')
	blast.stdin.close()
	blast.wait()


def run_psipred(in_file, out_file):
	''' Run PSIPRED on a PSSM. '''
	psipred = Popen([
		PSIPRED,
		# first pass
		'--first', path.join(DATADIR, 'weights.dat'),
		'--first', path.join(DATADIR, 'weights.dat2'),
		'--first', path.join(DATADIR, 'weights.dat3'),
		# second pass
		'--second', path.join(DATADIR, 'weights_p2.dat'),
		#'--iter', '1',
		#'--dca', '1.0',
		#'--dcb', '1.0',
	], stdin=in_file, stdout=out_file)
	psipred.wait()


def cached(seq):
	if not path.exists(CACHEDIR):
		mkdir(CACHEDIR)
	pred_name = path.join(CACHEDIR, seq.id + '.ss')
	if not path.exists(pred_name):
		pssm_name = path.join(CACHEDIR, seq.id + '.pssm')
		if not path.exists(pssm_name):
			run_blast(seq, pssm_name)
		run_psipred(open(pssm_name, 'r'), open(pred_name, 'w'))
	return pred_name

if __name__ == '__main__':
	for seq in SeqIO.parse(sys.stdin, 'fasta'):
		print seq.id, '=>', cached(seq)
