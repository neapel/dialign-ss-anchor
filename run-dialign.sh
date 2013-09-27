#!/bin/bash

root=dialign/dialign_package
data=$root/dialign2_dir
bin=$root/src/dialign2-2

input=test/1aho_u

# read $input.{fasta,anc}
# write $input.{cap,ali,ms}

DIALIGN2_DIR=$data $bin -anc -msf $input.fasta

rm $input.cap
rm $input.ali
