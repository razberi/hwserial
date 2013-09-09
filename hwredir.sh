#!/bin/sh

DIR="$( cd "$( dirname "$0" )" && pwd )"
#echo $DIR
#echo `pwd`
cd $DIR
#echo `pwd`

while true; do
    #nc -l -p 8600 -c 'bash; read -n 1; cat jsonData.json; sleep 5'
    nc -l -p 8600 -v -v -e ncreply.sh 2>&1
    err=$?
    echo "" # print new line
    if [ $err -ne 0 ]; then
        echo "Error code: $err"
        sleep 5 # sleep so in case of error it doesn't consume 100% cpu
    else
        sleep 1
    fi
done

