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
    g++
    fort77

To build ld online-demo, checkout AaltoASR and build and install it normally.

E.g.

    git clone https://github.com/aalto-speech/AaltoASR.git
    cd AaltoASR
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=../dest ..
    make install
    cd ..

After this, give the installation prefix of AaltoASR as prefix:

    git clone https://github.com/aalto-speech/AaltoASR-online-demo.git
    cd AaltoASR-online-demo
    mkdir build && cd build
    cmake -DAaltoASR_PREFIX=../../AaltoASR/dest ..
    make

Creating DEB package for demo laptops
=====================================

A .deb package containing all necessaties for the online demo can be created.

    git clone https://github.com/aalto-speech/AaltoASR.git
    cd AaltoASR
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=../dest ..
    make install
    cd ../../
    git clone git@github.com:aalto-speech/AaltoASR-online-demo.git
    cd AaltoASR-online-demo
    mkdir build && cd build
    cmake -DAaltoASR_PREFIX=../../AaltoASR/dest -DModels_DIR=/m/work/t40511_research/online-demo/models ..
    make package

Make sure the path for Model_DIR is correct.

This will create a package that can be installed on any Ubuntu laptop. After that, the demo can be started with the command 'startdemo'

Note that sometimes the make package will fail because of "no ./configure in src/physfs". In that case, cd to build/vendor/paragui/src/paragui_ext/ and run autogen.sh. After that you can run make package again.

Audio Issues
============
If selecting a microphone in pulseaudio does not work, create the file ~/.asoundrc with the following contents:

    pcm.pulse { type pulse }
    ctl.pulse { type pulse }
    
