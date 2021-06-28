EXECS = a.out
COMPILER = mpicc -std=c99

all: ${EXECS}

a.out: finalfloyd.c
	${COMPILER} finalfloyd.c

.phony: clean report

clean:
	rm ${EXECS}
cleano:
	rm -f *.out
cleansho:
	rm -f *.sh.o*
cleanshe:
	rm -f *.sh.e*
report:
	mpirun -n 4 ./a.out 4.in
	echo "....................................."
	mpirun -n 1 ./a.out 2048.in
	echo "....................................."
	mpirun -n 2 ./a.out 2048.in
	echo "....................................."
	mpirun -n 4 ./a.out 2048.in
	echo "....................................."
	mpirun -n 8 ./a.out 2048.in
	echo "....................................."
	mpirun -n 16 ./a.out 2048.in
	echo "....................................."
	mpirun -n 4 ./a.out 512.in
	echo "....................................."
	mpirun -n 4 ./a.out 1024.in
	echo "....................................."
	mpirun -n 4 ./a.out 2048.in
	echo "....................................."
	mpirun -n 4 ./a.out 4096.in
	echo "....................................."
	
test:
	qsub -l nodes=1:ppn=1 -v n=1,p=1,exec=a.out,inputfile=2048.in test.sh
	echo "......................................................"
	qsub -l nodes=1:ppn=2 -v n=1,p=2,exec=a.out,inputfile=2048.in test.sh
	qsub -l nodes=2:ppn=1 -v n=2,p=1,exec=a.out,inputfile=2048.in test.sh
	echo "......................................................"
	qsub -l nodes=1:ppn=4 -v n=1,p=4,exec=a.out,inputfile=2048.in test.sh
	qsub -l nodes=2:ppn=2 -v n=2,p=2,exec=a.out,inputfile=2048.in test.sh
	qsub -l nodes=4:ppn=1 -v n=4,p=1,exec=a.out,inputfile=2048.in test.sh
	echo "......................................................"
	qsub -l nodes=1:ppn=8 -v n=1,p=8,exec=a.out,inputfile=2048.in test.sh
	qsub -l nodes=2:ppn=4 -v n=2,p=4,exec=a.out,inputfile=2048.in test.sh
	qsub -l nodes=4:ppn=2 -v n=4,p=2,exec=a.out,inputfile=2048.in test.sh
	qsub -l nodes=8:ppn=1 -v n=8,p=1,exec=a.out,inputfile=2048.in test.sh
	echo "......................................................"
	qsub -l nodes=2:ppn=8 -v n=2,p=8,exec=a.out,inputfile=2048.in test.sh
	qsub -l nodes=4:ppn=4 -v n=4,p=4,exec=a.out,inputfile=2048.in test.sh
	qsub -l nodes=8:ppn=2 -v n=8,p=2,exec=a.out,inputfile=2048.in test.sh
	echo "......................................................"
	qsub -l nodes=1:ppn=4 -v n=1,p=4,exec=a.out,inputfile=512.in test.sh
	qsub -l nodes=2:ppn=2 -v n=2,p=2,exec=a.out,inputfile=512.in test.sh
	qsub -l nodes=4:ppn=1 -v n=4,p=1,exec=a.out,inputfile=512.in test.sh
	echo "......................................................"
	qsub -l nodes=1:ppn=4 -v n=1,p=4,exec=a.out,inputfile=1024.in test.sh
	qsub -l nodes=2:ppn=2 -v n=2,p=2,exec=a.out,inputfile=1024.in test.sh
	qsub -l nodes=4:ppn=1 -v n=4,p=1,exec=a.out,inputfile=1024.in test.sh
	echo "......................................................"
	qsub -l nodes=1:ppn=4 -v n=1,p=4,exec=a.out,inputfile=4096.in test.sh
	qsub -l nodes=2:ppn=2 -v n=2,p=2,exec=a.out,inputfile=4096.in test.sh
	qsub -l nodes=4:ppn=1 -v n=4,p=1,exec=a.out,inputfile=4096.in test.sh
