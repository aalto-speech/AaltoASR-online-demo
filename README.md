In Debian based systems, you need to install the following packages:
    
    liblapack-dev
    libblas-dev
    libfftw3-dev
    libsndfile1-dev
    libsigc++-1.2-dev
    portaudio19-dev
    swig
    autoconf
    libtool
    libsdl-dev
    libexpat-dev
    libphysfs-dev
    libfreetype6-dev
    g++
    fort77

To build online-demo, checkout AaltoASR and build and install it normally.

E.g.

    git clone git@github.com:aalto-speech/AaltoASR.git
    cd AaltoASR
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=../dest ..
    make install
    cd ..

After this, give the installation prefix of AaltoASR as prefix:

    git clone git@github.com:aalto-speech/AaltoASR-online-demo.git
    cd AaltoASR-online-demo
    mkdir build && cd build
    cmake -DAaltoASR_PREFIX=../../AaltoASR/dest ..
    make

Creating DEB package for demo laptops
=====================================

The executables and data are in separate packages.

To create a data package do:

    git clone git@github.com:aalto-speech/AaltoASR-online-demo.git
    cd AaltoASR-online-demo/data
    ./make_new_package.sh

This always creates a new package and bumps the version number automatically. The current package can also be found in /work/asr/demos/aaltoasr-online-demo (automatically copied)

To make an executables package do:

    git clone git@github.com:aalto-speech/AaltoASR.git
    cd AaltoASR
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=../dest ..
    make install
    cd ../../
    git clone git@github.com:aalto-speech/AaltoASR-online-demo.git
    cd AaltoASR-online-demo
    #Bump version number in CMakeLists.txt if anything has changed
    mkdir build && cd build
    cmake -DAaltoASR_PREFIX=../../AaltoASR/dest ..
    make package

When installing the packages (with dpkg -i), install the data package first.

This will create a package that can be installed on any Ubuntu laptop. After that, the demo can be started with the command 'startdemo'

Note that sometimes the make package will fail because of "no ./configure in src/physfs". In that case, cd to build/vendor/paragui/src/paragui_ext/ and run autogen.sh. After that you can run make package again.

Audio Issues
============
If selecting a microphone in pulseaudio does not work, create the file ~/.asoundrc with the following contents:

    pcm.pulse { type pulse }
    ctl.pulse { type pulse }
    
