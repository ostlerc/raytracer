#!/usr/bin/sh

ffmpeg -y -f image2 -i %d_image.ppm -sameq video.mpg
