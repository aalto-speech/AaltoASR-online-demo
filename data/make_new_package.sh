#!/bin/bash

OD_DIR=/work/asr/demos/aaltoasr-online-demo
rm -Rf build
mkdir build && cd build
CUR_VERSION=$(cat $OD_DIR/AaltoASR-online-demo-data-version)
NEXT_VERSION=$(echo $CUR_VERSION | awk -F. '{$NF+=1; OFS="."; print $0}') 
cmake -DCPACK_PACKAGE_VERSION=$NEXT_VERSION -DModels_DIR=$OD_DIR/models/ ..
echo $NEXT_VERSION > $OD_DIR/AaltoASR-online-demo-data-version
make package
cp AaltoASR-online-demo-data-${NEXT_VERSION}-Linux.deb $OD_DIR
chmod g+rw ${OD_DIR}/AaltoASR-online-demo-data-${NEXT_VERSION}-Linux.deb
echo "New package can be found here: $OD_DIR/AaltoASR-online-demo-data-${NEXT_VERSION}-Linux.deb"

