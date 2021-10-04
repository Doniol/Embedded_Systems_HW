GOOGLE_TEST=/u/c/e2102000/Embedded\ Systems\ Project/googletest/googletest

test: ring_buffer.o test_file.c libgtest.a
	g++ -isystem ${GOOGLE_TEST}/include -pthread ring_buffer.o test_file.c libgtest.a Gtest_main.c -o test

ring_buffer.o: ring_buffer.c ring_buffer.h
	g++ -c ring_buffer.c  -isystem ${GOOGLE_TEST}/include -I${GOOGLE_TEST} -pthread -o ring_buffer.o

libgtest.a:
	g++ -c $(GOOGLE_TEST)/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o

clean:
	rm -f *.o
