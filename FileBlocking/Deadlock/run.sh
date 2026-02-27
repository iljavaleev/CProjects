#!/bin/bash

gcc deadlock.c -o lock
./lock tfile1 tfile2&
PID=$!
./lock tfile2 tfile1&

echo "wait 5 seconds deadlock"
sleep 5
echo "kill first process"
kill $PID

rm lock