#!/bin/sh

DEMO_DIR="$(dirname $0)/.."
DATA_DIR="$HOME/demo_data"

lm="$DATA_DIR/aurora_3gram.bin"
la_lm="$DATA_DIR/aurora_2gram.bin"
lex="$DATA_DIR/wsj_5k_cmu_0.7a_tri.lex"
hmm="$DATA_DIR/wsj0dsp_ml_gain2500_occ200_26.1.2011_28"
DECODER_CMD="\"$DEMO_DIR/decoder\" --words --binlm \"$lm\" --lookahead \"$la_lm\" --lexicon \"$lex\" --ph \"$hmm.ph\" --dur \"$hmm.dur\" --lm-scale 40 --token-limit 25000 --beam 220"

"$DEMO_DIR/recognizer" -b $hmm -d "$DECODER_CMD" -C "$hmm.gcl" --eval-ming 0.15
