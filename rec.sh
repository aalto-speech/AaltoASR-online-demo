#!/bin/sh

#ssh itl-cl11
cd /home/thirsima/Work/online-demo-libs

./recognizer -d "./decoder  --binlm /share/work/thirsima/morph160000_4gram_1e-8.bin --lookahead /share/work/thirsima/morph160000_2gram.bin --lexicon /share/work/thirsima/morph160000.lex --ph /home/thirsima/Work/online-demo-libs/mfcc_speecon_mag_tri_25.5.2006_10.ph --dur /home/thirsima/Work/online-demo-libs/mfcc_speecon_mag_tri_25.5.2006_10.dur --lm-scale 35 --token-limit 30000 --beam 150"

#./recognizer #/home/workspace/online-demo/recognizer #-d "./decoder --beam ,,,"