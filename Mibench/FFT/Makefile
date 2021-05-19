
FILE = main.c fftmisc.c  fourierf.c
all: fft_or fft_sf
fft_sf: ${FILE} Makefile
	clang -fsoftboundcets ${FILE}   -o fft_sf -L/root/paper/SoftBoundCETS-3.9/runtime -lm -lrt -lsoftboundcets_rt

fft_or: ${FILE} Makefile
	clang  ${FILE}   -o fft_or  -lm 

clean:
	rm -rf  fft_* output*
