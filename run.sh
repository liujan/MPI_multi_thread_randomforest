rm -r random
make;
rm -r *.o;
rm -r result.csv;
mpirun -n 3  ./random;
