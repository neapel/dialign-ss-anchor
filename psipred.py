#!/usr/bin/env python
"""
Run PSIPRED on PSI-BLAST output.
"""

import subprocess
from tempfile import mkdtemp, TemporaryFile
from os import path, remove, rmdir
from Bio import SeqIO

# The name of the BLAST+ data bank
DBNAME = 'psipred/db/uniref90filt'

PSIDIR = 'psipred'

# Where the PSIPRED V3 programs have been installed
EXECDIR = path.join(PSIDIR, 'src')

# Where the PSIPRED V3 data files have been installed
DATADIR = path.join(PSIDIR, 'data')


def psipred_stream(in_stream, out_file):
	tmpdir = mkdtemp()
	checkpoint_file = path.join(tmpdir, 'checkpoint')
	matrix_file = path.join(tmpdir, 'matrix')
	pass1_file = path.join(tmpdir, 'pass1')

	# run a BLAST search with input:
	subprocess.check_call([
		'psiblast',
		'-db', DBNAME,
		'-query', '-', # stdin
		'-inclusion_ethresh', '0.001',
		'-out_pssm', checkpoint_file,
		'-num_iterations', '3',
		'-num_alignments', '0', # show no alignments
		'-num_threads', '4' # parallel threads to use
	], stdin=in_stream)

	# Convert checkpoint to matrix:
	with open(matrix_file, 'w') as f:
		subprocess.check_call([
			path.join(EXECDIR, 'chkparse'),
			checkpoint_file # input file
		], stdout=f)
	remove(checkpoint_file)

	# First pass
	with open(pass1_file, 'w') as f:
		subprocess.check_call([
			path.join(EXECDIR, 'psipred'),
			matrix_file, # input file
			path.join(DATADIR, 'weights.dat'), # weights
			path.join(DATADIR, 'weights.dat2'),
			path.join(DATADIR, 'weights.dat3'),
		], stdout=f)
	remove(matrix_file)
	
	# Second pass
	subprocess.check_call([
		path.join(EXECDIR, 'psipass2'),
		path.join(DATADIR, 'weights_p2.dat'), # weights
		'1', # itercount
		'1.0', # DCA
		'1.0', # DCB
		out_file, # output
		pass1_file # input
	])
	remove(pass1_file)
	rmdir(tmpdir)


def psipred(seq, out_file):
	with TemporaryFile() as f:
		SeqIO.write(seq, f, 'fasta')
		f.seek(0)
		psipred_stream(f, out_file)


if __name__ == '__main__':
	import sys

	infile = sys.argv[1]
	outfile = infile + '.psipred'
	seq = next(SeqIO.parse(infile, 'fasta'))
	psipred(seq, outfile)
