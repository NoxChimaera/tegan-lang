#!/bin/bash
if [ "$#" -eq "0" ]; then
  # src="example/main.t"
  src="example/main.t"
  out="dist/out.3ac"
elif [ "$#" -eq "1" ]; then
  src=$1
  out="dist/out.3ac"
else
  src=$1
  out=$2
fi

echo $src
echo $out

echo "BUILD:"
dist/tlc $src $out
dist/tlc $src "dist/opt.3ac" -Oall
echo
echo
echo "RUN:"
lli $out
