#!/bin/bash

echo "**********Result of FF*************" >> log_ff.txt
echo "" >> log_ff.txt
echo "" >> log_ff.txt

for i in `ls /home/yz395/ece650/001_malloc/my_malloc/alloc_policy_tests/*allocs`
do
    pro=`basename $i`
    echo "=================${pro}=============" >> log_ff.txt
    $i >> log_ff.txt
    echo "" >> log_ff.txt
    echo "" >> log_ff.txt
done

    
