all:
	c++ -c ../edlib/src/edlib.cpp -g -o edlib.o -I edlib/include
	cc -c edlib_run.c -g -o edlib_run.o -I ../edlib/include
	c++ edlib_run.o edlib.o -g -o edlib_run

mt:
	c++ -c ../edlib/src/edlib.cpp -g -o edlib.o -I edlib/include
	cc -c edlib_run_mt.c -g -fopenmp -Wall -O3 -o edlib_run_mt.o -I ../edlib/include
	c++ edlib_run_mt.o edlib.o -g -fopenmp -Wall -O3 -o edlib_run_mt

clean:
	$(RM) *~ *.o edlib_run edlib_run_mt