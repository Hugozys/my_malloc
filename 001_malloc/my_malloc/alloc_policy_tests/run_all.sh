#!/bin/bash

echo "**********Result of BF*************" >> log_bf.txt
echo "" >> log_bf.txt
echo "" >> log_bf.txt

for i in `ls /home/yz395/ece650/001_malloc/my_malloc/alloc_policy_tests/*allocs`
do
    pro=`basename $i`
    echo "=================${pro}=============" >> log_bf.txt
    $i >> log_bf.txt
    echo "" >> log_bf.txt
    echo "" >> log_bf.txt
done

    
