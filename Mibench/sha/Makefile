# By default, the code is compiled for a "big endian" machine.
# To compile on a "little endian" machine set the LITTLE_ENDIAN flag.
# To make smaller object code, but run a little slower, don't use UNROLL_LOOPS.
# To use NIST's modified SHA of 7/11/94, define USE_MODIFIED_SHA
FILE = sha.c   sha_driver.c
CFLAGS = -O3 -Wall
all: sha_ts sha_or

# sha_sf: ${FILE} Makefile
		# clang -fsoftboundcets ${FILE}  -o sha_sf -L/root/paper/SoftBoundCETS-3.9/runtime -lm -lrt -lsoftboundcets_rt
sha_ts: ${FILE} Makefile
	clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  ${FILE}  -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o sha_ts
sha_or: ${FILE} Makefile
	clang  ${FILE}  -o sha_or -lm 
clean:
	rm -rf sha_or sha_ts sha_ts output*

