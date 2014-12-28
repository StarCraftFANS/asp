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

### Setup build environment
source ./setup_environment.sh

### Build
case "$1" in
    clean)
        echo "==[MAKE CLEAN]==="
        make clean
        ;;

    mcu)
        echo "==[MAKE MCU]==="
        make BUILD_TYPE=MCU all
        ;;

    sim)
        echo "==[MAKE SIM]==="
        make BUILD_TYPE=SIM all
        ;;

    *)
        echo "Usage $0 {clean|mcu|sim}"
        exit 1
        ;;
esac
