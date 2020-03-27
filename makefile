all:
	c++ -c ../edlib/src/edlib.cpp -g -o edlib.o -I edlib/include
	cc -c edlib_run.c -g -o edlib_run.o -I ../edlib/include
	c++ edlib_run.o edlib.o -g -o edlib_run

clean:
	$(RM) *~ *.o edlib_run