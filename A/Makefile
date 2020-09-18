CC = gcc
CFLAGS = -Wall -Wextra -g -pthread

build:
		$(CC) $(CFLAGS) lab2_add.c -o lab2_add
		$(CC) $(CFLAGS) SortedList.c lab2_list.c -o lab2_list

tests: build
		-./testadd.sh
		-./testlist.sh

graphs: tests
		./lab2_add.gp
		./lab2_list.gp

clean:
		rm -f lab2a-604754714.tar.gz lab2_add lab2_list

dist: graphs
		tar -zcvf lab2a-604754714.tar.gz lab2_add.c SortedList.h SortedList.c lab2_list.c \
		lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png \
		lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png testadd.sh testlist.sh \
		lab2_add.gp lab2_list.gp lab2_add.csv lab2_list.csv Makefile README
