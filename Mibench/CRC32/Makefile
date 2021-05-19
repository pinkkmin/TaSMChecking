FILE = crc_32.c
all: crc_sf crc_or

crc_sf: ${FILE} Makefile
		clang -fsoftboundcets ${FILE}  -o crc_sf -L/root/paper/SoftBoundCETS-3.9/runtime -lm -lrt -lsoftboundcets_rt
crc_or: ${FILE} Makefile
	clang  ${FILE}  -o crc_or -lm 
clean:
	rm -rf crc_sf crc_or output*
