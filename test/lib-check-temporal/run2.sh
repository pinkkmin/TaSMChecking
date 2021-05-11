 rm -rf test_2
clang test_2.c -L/root/TaSMChecking/project -Lrt -ltasmc_rt -o test_2

echo '*****************************************************************'
./test_2
echo '*****************************************************************'