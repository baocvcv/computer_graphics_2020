HEADERS:=$(wildcard ./*.h*)

main: main.cpp $(HEADERS)
	g++ -O3 -fopenmp -std=c++14 $< -o $@

debug: main.cpp $(HEADERS)
	g++ -g -std=c++14 $< -o $@

.PHONY: run
run:
	./main output/scene1.bmp

.PHONY: clean
clean:
	rm -f main debug