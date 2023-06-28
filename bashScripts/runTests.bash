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

#all below start with strat_allZero/prioritizeLeader/bothEnds
#plurality: pluralityScores/prioritizeLeader/bothEnds
nice nice ./voteResearch 2 1 1 1 3 1 1 1 3
nice nice ./voteResearch 2 1 1 1 3 1 1 2 3
#approval: approvalScores/prioritizeLeader/bothEnds
nice nice ./voteResearch 1 1 1 1 2 1 1 1 3
nice nice ./voteResearch 1 1 1 1 2 1 1 2 3
#Hare: harePairwise/prioritizeLeader/buildFromTop
nice nice ./voteResearch 5 1 1 1 7 1 2 1 3
nice nice ./voteResearch 5 1 1 1 7 1 2 2 3
#Borda: bordaScores/prioritizeLeader/bothEnds
nice nice ./voteResearch 4 1 1 1 6 1 1 1 3
nice nice ./voteResearch 4 1 1 1 6 1 1 2 3
#Copeland: copelandScores/prioritizeLeader/bothEnds?
nice nice ./voteResearch 6 1 1 1 9 1 1 1 3
nice nice ./voteResearch 6 1 1 1 9 1 1 2 3
#Simpson: simpsonScores/prioritizeLeader/bothEnds?
nice nice ./voteResearch 7 1 1 1 10 1 1 1 3
nice nice ./voteResearch 7 1 1 1 10 1 1 2 3
#Coombs: coombsPairwise/prioritizeLeader/buildFromBottom?
nice nice ./voteResearch 9 1 1 1 7 1 3 1 3
nice nice ./voteResearch 9 1 1 1 7 1 3 2 3