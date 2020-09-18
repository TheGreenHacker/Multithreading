default:
		gcc -Wall -Wextra -g -pthread -lprofiler SortedList.c lab2_list.c -o lab2_list

tests: default
		./testlist.sh

graphs: tests
		./lab2b_list.gp

profile: default
		LD_PRELOAD=/usr/lib64/libprofiler.so 
		CPUPROFILE=raw.gperf ./lab2_list --iterations=1000 --threads=12 --sync=s
		pprof --text ./lab2_list raw.gperf > profile.out
		pprof --list=listTasks ./lab2_list raw.gperf >> profile.out
		rm -f raw.gperf 

clean:
		rm -f lab2b-604754714.tar.gz lab2_list

dist: graphs profile
		tar -zcvf lab2b-604754714.tar.gz SortedList.h SortedList.c lab2_list.c \
		lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png lab2b_list.gp \
		profile.out testlist.sh lab2b_list.csv Makefile README
		
