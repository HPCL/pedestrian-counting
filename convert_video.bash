#!/bin/bash

for f in *.h264
do
  filename=$(basename "$f")
  filename="${filename%.*}"
  x264 $f -o $filename.mp4
  #rm $f
done
