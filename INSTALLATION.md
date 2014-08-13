## Creating an Eclipse project

If you would like to edit/debug the source code in Eclipse, you can follow the instructions given on http://www.vtk.org/Wiki/Eclipse_CDT4_Generator

In short

    git clone https://github.com/aalto-speech/AaltoASR-online-demo.git
    cd AaltoASR-online-demo
    cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DAaltoASR_PREFIX=../AaltoASR/dest .

And import the resulting project in eclipse.

