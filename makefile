all:compile run
run:prun

compile:mpitest.cpp
	mpic++ -O2 -o mpitest mpitest.cpp
prun:mpitest
	mpirun -np 4 mpitest
	
srun:mpitest
	./mpitest
