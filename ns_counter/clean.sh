#!/bin/bash

ut test && for p in `ls`; do
    ([ -d $p ] && cd $p && rm -f *.vcd)
done
