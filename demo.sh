#!/bin/sh

DEMO_DIR=$(dirname $0)
"$DEMO_DIR/jaakko" -w 1200 -h 900 --beam 200 --lmscale 50 --script "$DEMO_DIR/rec.sh"
