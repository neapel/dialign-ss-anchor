#!/bin/bash

input=$1
ipssp=$2

if [ ! -n "$input" ] ; then
	echo "Usage: $0 input.tfa"
	exit
fi

echo "ssanchor: $input"

bin='/usr/bin/time -f %U\t%C -a -o run-ssanchor.times.log ssanchor/align'

if [ ! -n "$ipssp" ] ; then
	$bin $input.sub*.psipred > $input.abs.anc
	$bin --q3 $input.sub*.psipred > $input.q3.anc
	$bin --sov $input.sub*.psipred > $input.sov.anc
else
	$bin --ipssp --q3 $input.sub*.ipssp > $input.q3-ipssp.anc
	$bin --ipssp --sov $input.sub*.ipssp > $input.sov-ipssp.anc
fi

echo "done: $input"
