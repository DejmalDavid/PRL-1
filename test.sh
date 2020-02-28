#!/bin/bash

#pocet cisel bud zadam nebo 10 :)
if [ $# -ne 1 ];then 
    echo "Wrong argument's! Usage: test.sh pocet_hodnot"
    exit 1
else
    if ! [[ $1 =~ ^[0-9]+$ ]]
    then
        echo "Parametr pocet_hodnot must be positive integer!"
        exit 2
    fi
    
    if  [ $1 -lt 2 ]
    then
        echo "Cannot sort less than 2 numbers!"
        exit 3
    fi
    
    #TODO maximalni hodnotu urcit
    if  [ $1 -gt 32768 ]
    then
        echo "This is too much numbers, sorry..."
        exit 4
    fi
    
    numbers=$1
    echo $numbers
fi



#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o ots ots.cpp


#vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=$numbers of=numbers > /dev/null  2>&1

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $numbers ots

#uklid
#rm -f ots numbers
