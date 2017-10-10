CFLAGS=-O3 -std=c11 -f -pthread
CXXFLAGS=-O3 -std=c++11 -g -pthread
LDFLAGS=-lpthread
ARCHIVES=libfunctions.a
LD=g++

all: static_sched dynamic_sched sequential

t1: rahul.o
	$(LD) $(LDFLAGS) rahul.o $(ARCHIVES) -o test

static_sched: static_sched.o approx
	$(LD) $(LDFLAGS) static_sched.o $(ARCHIVES) -o static_sched

dynamic_sched: dynamic_sched.o approx
	$(LD) $(LDFLAGS) dynamic_sched.o $(ARCHIVES) -o dynamic_sched

libfunctions.a: functions.o
	ar rcs libfunctions.a functions.o

test: static_sched dynamic_sched approx
	./test.sh

sequential: sequential.o
	$(LD) $(LDFLAGS) sequential.o  libintegrate.a $(ARCHIVES) -o sequential

static_sched_plots.pdf: static_sched all
	./bench_static.sh

dynamic_sched_plots.pdf: dynamic_sched all
	./bench_dynamic.sh

bench: static_sched_plots.pdf dynamic_sched_plots.pdf

assignment-pthreads.tgz: static_sched.cpp dynamic_sched.cpp approx.cpp Makefile bench_static.sh bench_dynamic.sh test.sh assignment-pthreads.pdf libfunctions.a libintegrate.a sequential.cpp bench_sequential.sh
	tar zcvf assignment-pthreads.tgz static_sched.cpp dynamic_sched.cpp approx.cpp Makefile bench_static.sh bench_dynamic.sh test.sh assignment-pthreads.pdf libfunctions.a libintegrate.a sequential.cpp bench_sequential.sh

clean:
	- rm -rf *.o dynamic_sched static_sched 

clean_all:
	- rm -rf *.o dynamic_sched static_sched approx test bench assignment.tgz all t1 core*
