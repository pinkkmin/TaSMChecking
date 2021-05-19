
FILE = susan.c
all: susan_sf susan_or

susan_sf: ${FILE} Makefile
		clang -fsoftboundcets ${FILE}  -o susan_sf -L/root/paper/SoftBoundCETS-3.9/runtime -lm -lrt -lsoftboundcets_rt
susan_or: ${FILE} Makefile
	clang  ${FILE}  -o susan_or -lm 
clean:
	rm -rf susan_sf susan_or output*
