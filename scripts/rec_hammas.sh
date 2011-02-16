#!/bin/sh

DEMO_DIR="$(dirname $0)/.."
DATA_DIR="$HOME/demo_data"

lm="$DATA_DIR/hammas.bin"
la_lm="$DATA_DIR/hammas_2gram.bin"
lex="$DATA_DIR/hammas.lex"
hmm="$DATA_DIR/test_mfcc_noisy_trained"
DECODER_CMD="$DEMO_DIR/decoder --words --binlm $lm --lookahead $la_lm --lexicon $lex --ph $hmm.ph --dur $hmm.dur --lm-scale 40 --token-limit 20000 --beam 200"

"$DEMO_DIR/recognizer" -b $hmm -d "$DECODER_CMD" -C "$hmm.gcl" --eval-ming 0.10
