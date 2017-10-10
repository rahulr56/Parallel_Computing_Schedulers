all: s_gant d_gant

gant.o: 
	g++ -O3 -std=c++11 -g -pthread   -c -o gantt.o gantt_static_sched.cpp 

s_gant: gant.o
	g++ -lpthread gantt.o ../libfunctions.a -o s_gant

run_static: s_gant 
	./gant 1 0 10 100000 1 16 thread

dyngant.o: 
	g++ -O3 -std=c++11 -g -pthread   -c -o dyngantt.o gantt_dynamic_sched.cpp 

d_gant: dyngant.o
	g++ -lpthread dyngantt.o ../libfunctions.a -o d_gant

run_dynamic: d_gant
	./dgant 1 0 10 1000000 100000 16 thread 10000

clean:
	- rm -rf dyngantt.o gantt.o s_gant d_gant
