#!/bin/bash

ut clean
for p in `ls`; do
    ([ -d $p ] && cd $p && rm -f *.vcd)
done
