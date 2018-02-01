#! /bin/bash

echo " *******************Result of Lock *****************" >> lock

echo"">>lock.txt
echo"">>lock.txt


for i in thread_test_malloc_free thread_test_measurement thread_test_malloc_free_change_thread thread_test
do
    echo "========================${i}===========================" >> lock.txt
    (time ./$i) >> lock.txt 2>&1
    echo "" >> lock.txt
    echo "" >> lock.txt
done
