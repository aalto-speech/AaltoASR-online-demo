#!/bin/sh

DEMO_DIR=$(dirname $0)/..
"$DEMO_DIR/demogui" -w 1300 -h 830 --beam 180 --lmscale 40 --script "$DEMO_DIR/scripts/rec.sh"
#"$DEMO_DIR/demogui" -w 1200 -h 710 --beam 180 --lmscale 40 --script "$DEMO_DIR/scripts/rec.sh"
