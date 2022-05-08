#!/bin/sh

# run regression tests on interpreter

PROG=../tstest
ok="ok"
endmsg=$ok

#
# run regression tests
#
for i in *.ts
do
    j=`basename $i .ts`
    echo $i ...
    $PROG $i > $j.txt
    if diff -ub $j.expect $j.txt
    then
        rm -f $j.txt
    else
       echo FAIL: $j
       endmsg="TEST FAILURES"
    fi
done
echo $endmsg
