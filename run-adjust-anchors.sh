#!/bin/bash

input=$1
scale=$2

if [ ! -n "$input" ] ; then
	echo "Usage: $0 input.anc <scale>"
	exit
fi
if [ ! -n "$scale" ] ; then
	echo "Usage: $0 input.anc <scale>"
	exit
fi

output=${input/.anc/-$scale.anc}

awk "{printf(\"%s %s %s %s %s %s\n\", \$1, \$2, \$3, \$4, \$5, \$6 * $scale)}" < $input > $output
