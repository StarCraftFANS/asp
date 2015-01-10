#!/bin/bash
# /************************************************************************
#  *                                                                      *
#  * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
#  *                                                                      *
#  * This program is free software; you can redistribute it and/or modify *
#  * it under the terms of the GNU General Public License as published by *
#  * the Free Software Foundation; either version 2 of the License, or    *
#  * (at your option) any later version.                                  *
#  *                                                                      *
#  ************************************************************************/

### Setup environment
source ../setup_environment.sh

BUILD_DIR="../obj"
RUN_DIR="./gpsim_run_dir"

APP="pic_asp_sdcc"

### Check that simulation is possible
if [ ! -e ${BUILD_DIR}/${APP}.cod ]
then
    echo "*** ERROR : No build files found"
    exit 1
fi

### Clean any previous simulation
\rm -rf ${RUN_DIR}

### Prepare simulation
mkdir -pv ${RUN_DIR}/obj
cp ${BUILD_DIR}/${APP}{.cod,.cof,.dasm,.hex,.lst,.map} ${RUN_DIR}
cp ${BUILD_DIR}/*.asm ${RUN_DIR}/obj/

### Run simulation
cd ${RUN_DIR}
gpsim -s ${APP}.cod -c ../gpsim_asp.stc
