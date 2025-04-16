#!/bin/bash

./setup.sh
ut test && for p in `ls`; do
    ([ -d $p ] && cd $p && ../test.sh && ../run.sh)
done
