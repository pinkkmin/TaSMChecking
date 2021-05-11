 rm -rf test
clang test.c -L/root/TaSMChecking/project -Lrt -ltasmc_rt -o test

echo '*****************************************************************'
./test
echo '*****************************************************************'