#!/bin/bash

# read architecture
ARCH=$(uname -m)

if [ "$ARCH" == x86_64 ]; then
	echo starting 64-bit test
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../3rdparty/amd64/linux/realroot/lib:../3rdparty/amd64/linux/lidia-2.3.0/lib:../3rdparty/amd64/linux/libqi-0.9.33/lib
else
	echo starting 32-bit test
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../3rdparty/i386/linux/realroot/lib:../3rdparty/i386/linux/lidia-2.3.0/lib:../3rdparty/i386/linux/libqi-0.9.33/lib
fi

# run application
./test $@

