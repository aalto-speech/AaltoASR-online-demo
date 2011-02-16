#!/bin/sh

DEMO_DIR="$(dirname $0)/.."
DATA_DIR="$HOME/demo_data"

lm="$DATA_DIR/wsj_trigram_closed_20k.bin"
la_lm="$DATA_DIR/wsj_bigram_closed_20k.bin"
lex="$DATA_DIR/wsj_20k_cmu_0.7a_tri.lex"
hmm="$DATA_DIR/wsj_284_ml_gain4000_occ400_17.11.2010_25"
DECODER_CMD="\"$DEMO_DIR/decoder\" --words --binlm \"$lm\" --lookahead \"$la_lm\" --lexicon \"$lex\" --ph \"$hmm.ph\" --dur \"$hmm.dur\" --lm-scale 50 --token-limit 25000 --beam 200"

"$DEMO_DIR/recognizer" -b $hmm -d "$DECODER_CMD" -C "$hmm.gcl" --eval-ming 0.08
