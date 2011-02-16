#!/bin/sh

DEMO_DIR=$(dirname $0)/..
"$DEMO_DIR/demogui" -w 1300 -h 830 --beam 200 --lmscale 50 --script "$DEMO_DIR/scripts/rec_eng_5k.sh"
