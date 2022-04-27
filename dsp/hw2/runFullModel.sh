sh 00_clean_all.sh
sh 01_run_HCopy.sh > phase1text
sh 02_run_HCompV.sh > phase2text
sh 03_training.sh > phase3text
sh 04_testing.sh > phase4text
less result/accuracy
