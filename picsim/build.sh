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

### Build
case "$1" in
    clean)
        echo "==[MAKE CLEAN]==="
        make clean
        ;;

    all)
        echo "==[MAKE ALL]==="
        make all
        ;;

    *)
        echo "Usage $0 {clean|all}"
        exit 1
        ;;
esac
