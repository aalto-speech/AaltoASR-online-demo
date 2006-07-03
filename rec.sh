#!/bin/sh

cd /home/thirsima/Work/online-demo-libs

dir=/home/thirsima/Work/online-demo-libs
lm=$dir/morph40000_6gram_gt_1e-8.bin
la_lm=$dir/morph40000_2gram_gt.bin
lex=$dir/morph40000.lex

hmm=$dir/mfcc_cms_speecon_tri_14.6.2006_9
# hmm=$dir/mfcc_speecon_mag_tri_25.5.2006_10

./recognizer -b $hmm -d "./decoder --binlm $lm --lookahead $la_lm --lexicon $lex --ph $hmm.ph --dur $hmm.dur --lm-scale 35 --token-limit 30000 --beam 150"
#/home/thirsima/Work/online-demo/recognizer -b $hmm -d "/home/thirsima/Work/online-demo/decoder --binlm $lm --lookahead $la_lm --lexicon $lex --ph $hmm.ph --dur $hmm.dur --lm-scale 35 --token-limit 30000 --beam 150"



## vanha...
#!/bin/sh

#ssh itl-cl11
##cd /home/thirsima/Work/online-demo-libs

##dir=/home/thirsima/Work/online-demo-libs
##hmm=$dir/mfcc_cms_speecon_tri_14.6.2006_9
# hmm=$dir/mfcc_speecon_mag_tri_25.5.2006_10

##./recognizer -b $hmm -d "./decoder  --binlm /share/oldwork/thirsima/morph160000_4gram_1e-8.bin --lookahead /share/oldwork/thirsima/morph160000_2gram.bin --lexicon /share/oldwork/thirsima/morph160000.lex --ph $hmm.ph --dur $hmm.dur --lm-scale 35 --token-limit 30000 --beam 100"
