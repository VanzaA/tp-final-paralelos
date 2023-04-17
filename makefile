build-sequential:
	gcc -Wall src/sequential.c -o build/sequential

build-sequential-debug:
	gcc -Wall -D DEBUG src/sequential.c -o build/sequential

build-pthreads:
	gcc -Wall -pthread src/pthreads.c -o build/pthreads

build-pthreads-debug:
	gcc -Wall -D DEBUG -pthread src/pthreads.c -o build/pthreads

build-openmp:
	gcc -Wall -fopenmp src/openmp.c -o build/openmp

build-openmp-debug:
	gcc -Wall -D DEBUG -fopenmp src/openmp.c -o build/openmp

clean:
	rm -rf ./build

folder:
	mkdir build

build_all:
	make build-sequential
	make build-pthreads
	make build-openmp
