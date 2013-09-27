#!/bin/bash

input=$1

if [ ! -n "$input" ] ; then
	echo "Usage: $0 input.tfa....msf"
	exit
fi

base=$(sed 's/\.tfa.*//' <<< $input)
seqname=$(sed 's%^.*RV../%%' <<< $base)
kind=$(sed -r 's/^.*\.(.*)\.dialign-tx\.msf$/\1/' <<< $input)


bin=balibase3/bali_score_src/bali_score


all=$($bin $base.msf $input 2>/dev/null)
allsp=$(awk '{print $3}' <<< $all)
alltc=$(awk '{print $4}' <<< $all)
printf "$seqname\t$kind\tall\t$allsp\t$alltc\n"

core=$($bin $base.xml $input 2>/dev/null)
coresp=$(awk '{print $3}' <<< $core)
coretc=$(awk '{print $4}' <<< $core)
printf "$seqname\t$kind\tcore\t$coresp\t$coretc\n"
