#!/bin/bash

# проверить или создать
DIR="$HOME/.local/bin"


if [ ! -d "$DIR" ]; then
    mkdir "$DIR"
fi

files=$(ls *.c)
for f in $files; do
     exec_name=mq_"${f%.*}"
     gcc $f -o $exec_name && cp $exec_name $DIR && rm $exec_name
done


