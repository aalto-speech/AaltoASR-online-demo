#!/bin/sh

#export LD_LIBRARY_PATH=/share/puhe/x86_64/lib

#cd /home/thirsima/Work/online-demo-libs

dir=/home/thirsima/Work/online-demo-libs
lm=$dir/morph40000_6gram_gt_1e-8.bin
la_lm=$dir/morph40000_2gram_gt.bin
lex=$dir/morph40000.lex

hmm=$dir/mfcc_cms_adapt_speecon_tri_14.6.2006_9

./recognizer -b $hmm -d "./decoder --binlm $lm --lookahead $la_lm --lexicon $lex --ph $hmm.ph --dur $hmm.dur --lm-scale 35 --token-limit 30000 --beam 200"
