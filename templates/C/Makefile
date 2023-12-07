all: solution

test: solution
	solution test.txt

run: solution
	solution input.txt

solution: solution.c
	gcc solution.c -o solution -lm

clean:
	rm -rf *.o solution