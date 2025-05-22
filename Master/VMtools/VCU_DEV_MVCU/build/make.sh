# Copyright CASCO SIGNAL - 2018
# XD - 01/01/2018
# This file call make to build the full binary to be flashed. 
# This file exist only to be called from the run.bat script for DOS 
# compatibility as this is no easy to call make directly.

# First clean before rebuilding all, will be change when dependency (.h) will 
# be managed at makefile level.

make clean
make BOARDTYPE=MVCU APPNAME=COM  all

