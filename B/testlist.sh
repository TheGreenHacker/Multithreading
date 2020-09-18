#!/bin/bash

# NAME: Jack Li
# EMAIL: jackli2014@gmail.com
# ID: 604754714

rm -rf lab2b_list.csv
touch lab2b_list.csv

# single list 
for nthreads in 1, 2, 4, 8, 12, 16, 24
do
    ./lab2_list --sync=m --iterations=1000 --threads=$nthreads >> lab2b_list.csv
    ./lab2_list --sync=s --iterations=1000 --threads=$nthreads >> lab2b_list.csv
done

# no synchronization with 4 lists
for niterations in 1, 2, 4, 8, 16
do
	for nthreads in 1, 4, 8, 12, 16
	do
		./lab2_list --threads=$nthreads --iterations=$niterations --lists=4 --yield=id >> lab2b_list.csv 
	done
done

# synchronization with 4 lists
for niterations in 10, 20, 40, 80
do
	for nthreads in 1, 4, 8, 12, 16
	do
		./lab2_list --threads=$nthreads --sync=m --iterations=$niterations --lists=4 --yield=id >> lab2b_list.csv 
		./lab2_list --threads=$nthreads --sync=s --iterations=$niterations --lists=4 --yield=id >> lab2b_list.csv
	done
done

# synchronization with variable number of lists
for nthreads in 1, 2, 4, 8, 12
do
	for nlists in 4, 8, 16
	do
		./lab2_list --threads=$nthreads --sync=m --iterations=1000 --lists=$nlists >> lab2b_list.csv 
		./lab2_list --threads=$nthreads --sync=s --iterations=1000 --lists=$nlists >> lab2b_list.csv
	done
done
