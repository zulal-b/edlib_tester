all:
	c++ -c ../edlib/src/edlib.cpp -o edlib.o -I edlib/include
	cc -c edlib_run.c -Wall -O3 -o edlib_run.o -I ../edlib/include
	c++ edlib_run.o edlib.o -Wall -O3 -o edlib_run

mt:
	c++ -c ../edlib/src/edlib.cpp -O3 -o edlib.o -I edlib/include
	cc -c edlib_run_mt.c -O3 -fopenmp -Wall -o edlib_run_mt.o -I ../edlib/include
	c++ edlib_run_mt.o edlib.o -O3 -fopenmp -Wall -o edlib_run_mt

clean:
	$(RM) *~ *.o edlib_run edlib_run_mt