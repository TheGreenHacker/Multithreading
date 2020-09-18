#!/bin/bash

#NAME: Jack Li
#EMAIL: jackli2014@gmail.com
#ID: 604754714

rm -rf lab2_list.csv
touch lab2_list.csv

for niterations in 1, 10, 100, 1000
do
	for nthreads in 1, 2, 4, 8, 12
	do
		./lab2_list --threads=$nthreads --iterations=$niterations >> lab2_list.csv 
	done
done

for niterations in 1, 2, 4, 8, 16, 32
do
	for nthreads in 2, 4, 8, 12
	do
		./lab2_list --threads=$nthreads --iterations=$niterations --yield=i >> lab2_list.csv 
		./lab2_list --threads=$nthreads --iterations=$niterations --yield=d >> lab2_list.csv 
		./lab2_list --threads=$nthreads --iterations=$niterations --yield=il >> lab2_list.csv 
		./lab2_list --threads=$nthreads --iterations=$niterations --yield=dl >> lab2_list.csv 
		./lab2_list --iterations=$niterations --threads=$nthreads --sync=s --yield=i >> lab2_list.csv
		./lab2_list --iterations=$niterations --threads=$nthreads --sync=s --yield=d >> lab2_list.csv
        ./lab2_list --iterations=$niterations --threads=$nthreads --sync=s --yield=il >> lab2_list.csv
        ./lab2_list --iterations=$niterations --threads=$nthreads --sync=s --yield=dl >> lab2_list.csv
        ./lab2_list --iterations=$niterations --threads=$nthreads --sync=m --yield=i >> lab2_list.csv
        ./lab2_list --iterations=$niterations --threads=$nthreads --sync=m --yield=d >> lab2_list.csv
        ./lab2_list --iterations=$niterations --threads=$nthreads --sync=m --yield=il >> lab2_list.csv
        ./lab2_list --iterations=$niterations --threads=$nthreads --sync=m --yield=dl >> lab2_list.csv
	done
done

for nthreads in 1, 2, 4, 8, 12, 16, 24
do
    ./lab2_list --sync=m --iterations=1000 --threads=$nthreads >> lab2_list.csv
    ./lab2_list --sync=s --iterations=1000 --threads=$nthreads >> lab2_list.csv
done
