To build ld online-demo, checkout AaltoASR and build and install it normally.

E.g.

    git checkout https://github.com/aalto-speech/AaltoASR.git
    cd AaltoASR
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=../dest ..
    make install

After this, give the installation prefix of AaltoASR as prefix:

    cd AaltoASR-online-demo
    mkdir build && cd build
    cmake -DAaltoASR_PREFIX=../../AaltoASR/dest ..
    make

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
