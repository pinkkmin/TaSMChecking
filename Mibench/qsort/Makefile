FILE1 = qsort_small.c
FILE2 = qsort_large.c

all: qsort_small qsort_large

qsort_small: qsort_small.c Makefile
	clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  qsort_small.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o qsort_small

	# gcc -static qsort_small.c -O3 -o qsort_small -lm
qsort_large: qsort_large.c Makefile
	# gcc -static qsort_large.c -O3 -o qsort_large -lm
	clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  qsort_large.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o qsort_large
clean:
	rm -rf qsort_small qsort_large output*
