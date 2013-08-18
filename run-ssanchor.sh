#!/bin/bash

input=$1

if [ ! -n "$input" ] ; then
	echo "Usage: $0 input.tfa"
	exit
fi

echo "ssanchor: $input"

bin='/usr/bin/time -f %U\t%C -a -o run-ssanchor.times.log ssanchor/align'

$bin $input.sub*.psipred > $input.blosum.anc
$bin --q3 $input.sub*.psipred > $input.blosum-psipred-q3.anc
$bin --sov $input.sub*.psipred > $input.blosum-psipred-sov.anc

echo "done: $input"
