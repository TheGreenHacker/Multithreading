#! /usr/local/cs/bin/gnuplot
#
# NAME: Jack Li
# EMAIL: jackli2014@gmail.com
# ID: 604754714
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2b_1.png ... throughput vs. number of threads for mutex and spin-lock synchronized list operations.
#	lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list   operations.
#   lab2b_3.png ... successful iterations vs. threads for each synchronization method.
#   lab2b_4.png ... throughput vs. number of threads for mutex synchronized partitioned lists.
#   lab2b_5.png ... throughput vs. number of threads for spin-lock-synchronized partitioned lists.
#

# general plot parameters
set terminal png
set datafile separator ","

# lab2b_1.png
set title "List-1: Throughput of synchronization mechanisms"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Operations/Second"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'list w/mutex, 1000 iterations' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'list w/spin-lock, 1000 iterations' with linespoints lc rgb 'green'


# lab2b_2.png
set title "List-2: Average per-operation times using Mutex-protections"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "average time/operation (ns)"
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'average time per operation' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'average wait-for-lock time' with linespoints lc rgb 'green'


# lab2b_3.png
set title "List-3: Successful Synchronization of Partitioned Lists"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
     "< grep list-id-none lab2b_list.csv" using ($2):($3) \
	title 'Unprotected'with points lc rgb 'green', \
    "< grep list-id-s lab2b_list.csv" using ($2):($3) \
	title 'Spin-Lock' with points lc rgb 'red', \
    "< grep list-id-m lab2b_list.csv" using ($2):($3) \
	title 'Mutex' with points lc rgb 'blue'
	


# lab2b_4.png
set title "List-4: Throughput of Mutex Protected Partitioned Lists"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Operations/Second"
set logscale y 10
set output 'lab2b_4.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=4' with linespoints lc rgb 'green', \
	 "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=8' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=16' with linespoints lc rgb 'orange'


# lab2b_5.png
set title "List-5: Throughput of Spin-Lock Protected Partitioned Lists"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "operations/second"
set logscale y 10
set output 'lab2b_5.png'
# set key left top

plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=4' with linespoints lc rgb 'green', \
	 "< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=8' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=16' with linespoints lc rgb 'orange'