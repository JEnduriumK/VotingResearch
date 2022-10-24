#!/bin/bash

#Format is 1x election type, 2x strategy triplets, 1x prog/ind choice, 1x simple/verbose
#
#Election Type:
#1 - approval
#2 - plurality
#3 - antiplurality
#4 - borda
#5 - hare
#6 - copeland
#7 - simpson
#8 - schultze
#9 - coombs
#
#Strategy triplet:
#1 - strat_allZero                   1 - prioritizeLeader        1 - bothEnds
#2 - strat_approval                  2 - prioritizeRunnerUp      2 - front
#3 - strat_plurality                 3 - neighbors               3 - back
#4 - strat_antiplurality
#5 - strat_distanceToWin
#6 - strat_bordaEval
#7 - strat_pairwise
#8 - strat_hareEliminationSpeed
#9 - strat_copeland
#10 - strat_simpson
#11 - strat_victoryPairwise
#12 - strat_winOrLosePairwise
#
#Prog/ind choice:
#1 - progressive (YYYYXXXXX)
#2 - individual (XXXYXXXXX)
#
#Simple/verbose:
#1 - simple
#2 - verbose
#3 - verbose + voters
#
#Sample would be:
#./voteResearch 4 8 2 3 2 1 1 2 1
#which is [4] [8 2 3] [2 1 1] [2] [1]
#Resulting in a Borda election comparing
#strat_hareEliminationSpeed prioritizeRunnerUp back
#to
#strat_approval prioritizeLeader bothEnds
#making changes in an individual (XXXYXXXXX) manner
#with simple output.

#Hare: harePairwise/prioritizeLeader/buildFromTop
#nice nice ./voteResearch 5 7 1 2 7 1 2 1 3
#nice nice ./voteResearch 5 7 1 2 7 1 2 2 3

#https://stackoverflow.com/questions/169511/how-do-i-iterate-over-a-range-of-numbers-defined-by-variables-in-bash

#checks approval/runnerUp/bothEnds against adjacent strategies.

stage1max=2
stage2max=3
stage3max=3
stage1init=2
stage1=2
stage2init=2
stage2=2
stage3init=1

#nice nice ./voteResearch1000 1 1 1 1 2 1 1 1 1
#run another sincere vs winOrLosePairwise prioritizeLeader for good measure

for ((stage3=1;stage3<=stage3max;stage3++)); do
	if [[ $stage1init == $stage1 && $stage2init == $stage2 && $stage3init == $stage3 ]]; then
		continue
	fi
	date
	echo "Running $stage1 $stage2 $stage3"
	nice nice ./voteResearch1000 5 "$stage1init" "$stage2init" "$stage3init" "$stage1" "$stage2" "$stage3" 1 1
done
