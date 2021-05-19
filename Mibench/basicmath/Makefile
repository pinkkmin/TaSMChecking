FILE1 = basicmath_small.c   rad2deg.c  cubic.c   isqrt.c
FILE2 = basicmath_large.c   rad2deg.c  cubic.c   isqrt.c

all: basicmath_sf basicmath_or

basicmath_sf: ${FILE1} Makefile
		clang -fsoftboundcets ${FILE1} -o basicmath_sf -L/root/paper/SoftBoundCETS-3.9/runtime -lm -lrt -lsoftboundcets_rt
basicmath_or: ${FILE1} Makefile
	clang ${FILE1} -o basicmath_or -lm 
clean:
	rm -rf basicmath_sf basicmath_or output*
