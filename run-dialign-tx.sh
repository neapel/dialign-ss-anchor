#!/bin/bash

input=$1
suffix=$2

if [ ! -n "$input" ] ; then
	echo "Usage: $0 input.tfa <suffix>"
	exit
fi

root=dialign/DIALIGN-TX_1.0.2
data=$root/conf/
bin="/usr/bin/time -f %U\t%C -a -o run-dialign-tx.times.log $root/bin/dialign-tx"

echo "dialign-tx: $input"

if [ ! -n "$suffix" ] ; then
	# without anchors
	out=$input.ref.dialign-tx
	$bin $data $input $out.fasta >/dev/null
	squizz -c MSF $out.fasta > $out.msf 2>/dev/null
fi

# with anchors
for kind in abs q3 sov ; do
	name=$input.$kind$suffix
	out=$name.dialign-tx
	$bin -A $name.anc $data $input $out.fasta >/dev/null
	squizz -c MSF $out.fasta > $out.msf 2>/dev/null
done

echo "done: $input"
