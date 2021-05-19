FILE = bitcnt_1.c bitcnt_2.c bitcnt_3.c bitcnt_4.c bitcnts.c bitfiles.c bitstrng.c bstr_i.c 

all: bitcount_or bitcount_sf
bitcount_sf: ${FILE} Makefile
	clang -fsoftboundcets ${FILE} -o bitcount_sf -L/root/paper/SoftBoundCETS-3.9/runtime -lm -lrt -lsoftboundcets_rt
bitcount_or: ${FILE} Makefile
	clang ${FILE} -o bitcount_or -lm 
clean:
	rm -rf bitcount_or bitcount_sf output*
