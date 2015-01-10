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
        make -f Makefile_sdcc clean
	make -f Makefile_xc8 clean
        ;;

    sdcc_mcu)
        echo "==[MAKE SDCC MCU]==="
        make -f Makefile_sdcc BUILD_TYPE=MCU all
        ;;

    sdcc_sim)
        echo "==[MAKE SDCC SIMULATOR]==="
        make -f Makefile_sdcc BUILD_TYPE=SIM all
        ;;

    xc8_mcu)
        echo "==[MAKE XC8 MCU]==="
        make -f Makefile_xc8 BUILD_TYPE=MCU all
        ;;

    xc8_dbg)
        echo "==[MAKE XC8 DEBUG]==="
        make -f Makefile_xc8 BUILD_TYPE=DBG all
        ;;

    *)
        echo "Usage $0 {clean|sdcc_mcu|sdcc_sim|xc8_mcu|xc8_dbg}"
        exit 1
        ;;
esac
