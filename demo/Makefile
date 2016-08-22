all:compile run

compile:mpitest.cpp
	mpic++ -O2 -o mpitest mpitest.cpp
run:mpitest
	mpirun -np 4 mpitest
	
clean:
	rm mpitest
