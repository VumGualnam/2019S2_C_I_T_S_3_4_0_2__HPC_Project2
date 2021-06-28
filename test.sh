#PBS -l nodes=2:ppn=1 
echo ------------------------------------------------------
echo Filename=${inputfile} nodes=${n} ppn=${p}
echo ------------------------------------------------------
source /etc/bash.bashrc
mpirun ${exec} ${inputfile}

