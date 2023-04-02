build-sequential:
	gcc -Wall src/sequential.c -o build/sequential

build-sequential-debug:
	gcc -Wall -D DEBUG src/sequential.c -o build/sequential

build-pthreads:
	gcc -Wall -std=gnu99 -pthread src/pthreads.c -o build/pthreads

build-pthreads-debug:
	gcc -Wall -D DEBUG -std=gnu99 -pthread src/pthreads.c -o build/pthreads

build-openmp:
	gcc -Wall -std=gnu99 -pthread src/openmp.c -o build/openmp

build-openmp-debug:
	gcc -Wall -D DEBUG -std=gnu99 -fopenmp src/openmp.c -o build/openmp

clean:
	rm -rf ./build

folder:
	mkdir build