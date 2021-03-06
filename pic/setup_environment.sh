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

# Microchip XC8 toolchain
export XC8_HOME=/opt/microchip/xc8/v1.33
export PATH=$XC8_HOME/bin:$PATH

# SDCC toolchain
export SDCC_HOME=/opt/sdcc
export PATH=$SDCC_HOME/bin:$PATH

# GPUTILS
export GPUTILS_HOME=/opt/gputils
export PATH=$GPUTILS_HOME/bin:$PATH

# GPSIM
export GPSIM_HOME=/opt/gpsim
export PATH=$GPSIM_HOME/bin:$PATH
