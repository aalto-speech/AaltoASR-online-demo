#!/bin/sh

host=$1

if [ "$xhost" = "x" ]; then
    host=itl-cl1
    echo "connecting to $host"
fi

./jaakko -w 1200 -h 900 --beam 200 --lmscale 50 --host $host --script /home/thirsima/Work/online-demo-libs/rec.sh
