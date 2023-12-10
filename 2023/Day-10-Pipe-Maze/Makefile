all: solution

test1: solution
	./solution test1.txt

test2: solution
	./solution test2.txt

test: solution
	./solution test1.txt
	./solution test2.txt

tests: solution
	./solution test1.txt
	./solution test2.txt

run: solution
	./solution input.txt

solution: solution.c
	gcc solution.c -o solution -lm

clean:
	rm -rf *.o solution