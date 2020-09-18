#!/bin/bash

#NAME: Jack Li
#EMAIL: jackli2014@gmail.com
#ID: 604754714

rm -rf lab2_add.csv
touch lab2_add.csv

for niterations in 10, 20, 40, 80, 100, 1000, 10000, 100000
do
	for nthreads in 1, 2, 4, 8, 12
	do
		./lab2_add --threads=$nthreads --iterations=$niterations >> lab2_add.csv
		./lab2_add --threads=$nthreads --iterations=$niterations --yield >> lab2_add.csv
	done
done



for niterations in 10, 20, 40, 80, 100, 1000, 10000, 100000
do
	for nthreads in 1, 2, 4, 8, 12
	do
		./lab2_add --yield --sync=c --iterations=$niterations --threads=$nthreads >> lab2_add.csv
		./lab2_add --yield --sync=m --iterations=$niterations --threads=$nthreads >> lab2_add.csv
		./lab2_add --yield --sync=s --iterations=$niterations --threads=$nthreads >> lab2_add.csv
		./lab2_add --sync=c --iterations=$niterations --threads=$nthreads >> lab2_add.csv
		./lab2_add --sync=m --iterations=$niterations --threads=$nthreads >> lab2_add.csv
        ./lab2_add --sync=s --iterations=$niterations --threads=$nthreads >> lab2_add.csv
	done
done

for niterations in 10, 20, 40, 80, 100, 1000
do
	for nthreads in 1, 2, 4, 8, 12
	do
		./lab2_add --yield --sync=s --iterations=$niterations --threads=$nthreads >> lab2_add.csv
        ./lab2_add --sync=s --iterations=$niterations --threads=$nthreads >> lab2_add.csv
	done
done