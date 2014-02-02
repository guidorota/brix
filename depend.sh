#!/bin/bash
object=`sed -e 's@\.c@@' <<< $2`
declare -a INPUT_FILES=($@)
unset INPUT_FILES[0]
$1 ${INPUT_FILES[@]} -MM -MG | sed -e "s@^.*:@$object.d $object.o:@"