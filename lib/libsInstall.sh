#!/bin/bash

# Installing OpenGL
apt-get install mesa-common-dev

# Installing glut
apt-get install freeglut3-dev




# Installing ImageMagick
wget ftp://ftp.imagemagick.org/pub/ImageMagick/ImageMagick.tar.gz -O ImageMagick.tar.gz

chmod 744 ImageMagick.tar.gz

tar zxvf ImageMagick.tar.gz

# at the end of the following line there is a "tab" character that ables me to
# access the right ImageMagick folder (I hope they do not modify the way they
# specify the format of the folder in which the ImageMagick is compressed).
cd ImageMagick-	

./configure --prefix="usr/local"
make
make install




# Installing Voronoi3D
tar zxvf voronoi3D.tar.gz
cd voronoi3D
make
make clean
rm *.c *.h *.cc Makefile




# Returning to the initial directory
cd ..

