#!/bin/bash

echo "**********Result of BF*************" >> bf.txt
echo "" >> bf.txt
echo "" >> bf.txt

for i in `ls /home/yz395/ece650/001_malloc/my_malloc/alloc_policy_tests/*allocs`
do
    pro=`basename $i`
    echo "=================${pro}=============" >> bf.txt
    $i >> bf.txt
    echo "" >> bf.txt
    echo "" >> bf.txt
done

    
