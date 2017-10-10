#!/bin/sh

REPEAT=20000000
AMOUNT=2000000000
LIBS="serial fork pthread pth"
for lib in $LIBS; do
	CMD="./banque --lib $lib --repeat $REPEAT --amount $AMOUNT"
	echo $CMD
	$CMD
done
