#!/bin/bash

#-----------------------------------------------------------------------
#---- Configuration de l'environnement
#-----------------------------------------------------------------------


#-----------------------------------------------------------------------
#--- Lib externes
#-----------------------------------------------------------------------

#---- opencv
export OPENCV_LIB=/home/sether/Externes_c/opencv-4.0.1/install/lib64
export OPENCV_INC=/home/sether/Externes_c/opencv-4.0.1/install/include/opencv4



#---- LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${OPENCV_LIB}:${LD_LIBRARY_PATH} 


