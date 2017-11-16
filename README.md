# opensea-common
Cross platform library containing common set of functions used by other libraries. 

Overview 
--------
The opensea-common library has common set of defines and functions
that are not specific to any interface, compiler or OS. 

Other opensea-* libraries depends on this library for common functionality. 

Source
------
https://github.com/Seagate/opensea-common.git

Building
--------
All Makefile and Visual Studio project & solution files are part of Make folder.

The following will build the debug version of the library by default.

cd Make/gcc
make 

To build under Microsoft Windows, open the correspoinding 
Visual Studio Solution files for VS 2013 or 2015

Documentation
-------------
Header files & functions have doxygen documentation. 

Platforms
---------
Under Linux this libraries can be built on the following platforms using 
a cross platform compiler: 

        aarch64
        alpha 
        arm 
        armhf 
        hppa 
        m68k 
        mips 
        mips64 
        mips64el
        mipsel 
        powerpc 
        powerpc64 
        powerpc64le
        s390x 
        sh4 
        x86 
        x86_64 
        
This project can be build under Windows Visual Studio 2013 & 2015 solution
files for x86 and x64 targets.  
