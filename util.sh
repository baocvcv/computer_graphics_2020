#!/bin/sh

retrive_img ()
{
    img=$1
    scp mengxy@166.111.17.31:/home/mengxy/bh/final.RayTracing/code/output/$1 final.RayTracing/
}

send_code ()
{
    tar cvzf raytracing.tgz final.RayTracing
    scp raytracing.tgz mengxy@166.111.17.31:/home/mengxy/bh/
    rm -rf raytracing.tgz
}

## Main scripts

if  [ "$1" = "get" ]; then
    retrive_img $2
elif [ "$1" = "send" ]; then
    send_code
fi
