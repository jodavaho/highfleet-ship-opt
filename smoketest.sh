#!/bin/bash
build/$(gcc -dumpmachine)/hfopt fill --speed_min=800 --range_min=2000 ak100=4 fss=4 radar
pushd build/lib.linux-x86_64-3.8/
echo "import hf.opt as o; o.example()" | python3
popd
