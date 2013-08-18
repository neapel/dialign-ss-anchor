#!/bin/bash

input=$1

if [ ! -n "$input" ] ; then
	echo "Usage: $0 input.tfa"
	exit
fi

root=dialign/DIALIGN-TX_1.0.2
data=$root/conf/
bin="/usr/bin/time -f %U\t%C -a -o run-dialign-tx.times.log $root/bin/dialign-tx"

echo "dialign-tx: $input"

# without anchors
out=$input.dialign-tx
$bin $data $input $out.fasta >/dev/null
squizz -c MSF $out.fasta > $out.msf 2>/dev/null

# with anchors
for kind in blosum blosum-psipred-q3 blosum-psipred-sov ; do
	out=$input.$kind.dialign-tx
	$bin -A $input.$kind.anc $data $input $out.fasta >/dev/null
	squizz -c MSF $out.fasta > $out.msf 2>/dev/null
done

echo "done: $input"
