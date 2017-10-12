all:
	gcc -c backtrace_wrapper.c crash_handler.c -I.
	@ar x libbacktrace.a
	@ar r libcrashhandler.a *.o

clean:
	rm ./*.o

