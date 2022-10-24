#include "vote.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

const int ELECTORATE_COUNT = 10000;
const int VOTER_COUNT = 30;
const int CANDIDATE_COUNT = 6;
const int NUMBER_OF_ROUNDS = 10;
const int MAX_CANDIDATES = 40;

const int DEBUG_MODE = 0;

const string RESULTS_DIRECTORY = "";
bool PRINT_TO_FILE = true;
bool ALERT_WHEN_DONE = true;
bool PRINT_DETAILS = true;
bool PRINT_VOTERS = false;//Won't work unless PRINT_DETAILS is true.

/* 
1 = debug weird hang during round1 of electorate1
 */

struct electorateExperiment {
	//Election theElectionObject;
	int initialWinner, initialWinnerRoundCount;
	bool initialElectionStable;
	vector<Voter> theseVoters;//This should never be reset.
	vector<int> winners,individualAdjustments,roundsToFinish;
	vector<bool> electionWasStable;
	//vector<bool> changes;
	electorateExperiment() : 
	winners(VOTER_COUNT,-1),
	individualAdjustments(VOTER_COUNT,0),
	roundsToFinish(VOTER_COUNT,-1),
	electionWasStable(VOTER_COUNT,false)
	//Use count_if() to count >0 and <0 individualAdjustments.
	//collectiveScore(VOTER_COUNT,0), //Just use the winner value to calculate any time you want it.
	//changes(VOTER_COUNT,false)
	{}
};

string printStratCombo()
{
	stringstream returnStream;
	return returnStream.str();
}

//https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp/
//For possible file input?
int main(int argc, char *argv[])
{
	bool PRINT_TO_FILE = true;
	bool ALERT_WHEN_DONE = false;
	bool PRINT_DETAILS = true;
	bool PRINT_VOTERS = false;//Won't work unless PRINT_DETAILS is true.
	stringstream outputFileName, debugFilename, researchOutput, researchResults, deepDiveOutput;
	researchOutput << __FILE__ << " was compiled on " << __DATE__ << " at " << __TIME__ << ".\n";
	electionType electionStyle;

	pollMethod firstPollMethod;
	pairingMethod firstPairMethod;
	ballotBuildMethod firstBuilderMethod;

	pollMethod secondPollMethod;
	pairingMethod secondPairMethod;
	ballotBuildMethod secondBuilderMethod;
	
	int tempElectionInput = (argc >= 2 ? atoi(argv[1]) : 0);
	int tempPollInputX = (argc >= 3 ? atoi(argv[2]) : 0);
	int tempPairInputX = (argc >= 4 ? atoi(argv[3]) : 0);
	int tempBuildInputX = (argc >= 5 ? atoi(argv[4]) : 0);
	int tempPollInputY = (argc >= 6 ? atoi(argv[5]) : 0);
	int tempPairInputY = (argc >= 7 ? atoi(argv[6]) : 0);
	int tempBuildInputY = (argc >= 8 ? atoi(argv[7]) : 0);
	int progressiveOrIndv = (argc >= 9 ? atoi(argv[8]) : 0);
	int verbositySetting = (argc >= 10 ? atoi(argv[9]) : 0);
		
	//bool usingInputFile = false;
	//ifstream inputFile;
	ofstream resultsFile;
	ofstream debugFile;
	
	cerr << "1 - approval\n"
	<< "2 - plurality\n"
	<< "3 - antiplurality\n"
	<< "4 - borda\n" 
	<< "5 - hare\n" 
	<< "6 - copeland\n" 
	<< "7 - simpson\n" 
	<< "8 - schultze\n" 
	<< "9 - coombs\n"
	<< "Please choose a voting system:";
	if(tempElectionInput)
	{
		cerr << tempElectionInput;
	}
	else
	{
		cin >> tempElectionInput;
	}
	
	cerr << "\n\nFor strategyX polling:\n"
	<< "1 - strat_allZero\n"
	<< "2 - strat_approval\n"
	<< "3 - strat_plurality\n"
	<< "4 - strat_antiplurality\n"
	<< "5 - strat_distanceToWin\n"
	<< "6 - strat_bordaEval\n"
	<< "7 - strat_pairwise\n"
	<< "8 - strat_hareEliminationSpeed\n"
	<< "9 - strat_copeland\n"
	<< "10 - strat_simpson\n"
	<< "11 - strat_victoryPairwise\n"
	<< "12 - strat_winOrLosePairwise\n"
	<< "Select the poll method:";
	if(tempPollInputX)
	{
		cerr << tempPollInputX;
	}
	else
	{
		cin >> tempPollInputX;
	}

	cerr << "\n\nFor strategyX pairing:\n"
	<< "1 - prioritizeLeader\n"
	<< "2 - prioritizeRunnerUp\n"
	<< "3 - neighbors\n"
	<< "Select the pairing method:";
	if(tempPairInputX)
	{
		cerr << tempPairInputX;
	}
	else
	{
		cin >> tempPairInputX;
	}
	
	cerr << "\n\nFor strategyX ballot building:\n"
	<< "1 - bothEnds\n"
	<< "2 - front\n"
	<< "3 - back\n"
	<< "Select the ballot building method:";
	if(tempBuildInputX)
	{
		cerr << tempBuildInputX;
	}
	else
	{
		cin >> tempBuildInputX;
	}
	
	cerr << "\n\nFor strategyY polling:\n"
	<< "1 - strat_allZero\n"
	<< "2 - strat_approval\n"
	<< "3 - strat_plurality\n"
	<< "4 - strat_antiplurality\n"
	<< "5 - strat_distanceToWin\n"
	<< "6 - strat_bordaEval\n"
	<< "7 - strat_pairwise\n"
	<< "8 - strat_hareEliminationSpeed\n"
	<< "9 - strat_copeland\n"
	<< "10 - strat_simpson\n"
	<< "11 - strat_victoryPairwise\n"
	<< "12 - strat_winOrLosePairwise\n"
	<< "Select the poll method:";
	if(tempPollInputY)
	{
		cerr << tempPollInputY;
	}
	else
	{
		cin >> tempPollInputY;
	}
	
	cerr << "\n\nFor strategyY pairing:\n"
	<< "1 - prioritizeLeader\n"
	<< "2 - prioritizeRunnerUp\n"
	<< "3 - neighbors\n"
	<< "Select the pairing method:";
	if(tempPairInputY)
	{
		cerr << tempPairInputY;
	}
	else
	{
		cin >> tempPairInputY;
	}
	
	cerr << "\n\nFor strategyY ballot building:\n"
	<< "1 - bothEnds\n"
	<< "2 - front\n"
	<< "3 - back\n"
	<< "Select the ballot building method:";
	if(tempBuildInputY)
	{
		cerr << tempBuildInputY << "\n";
	}
	else
	{
		cin >> tempBuildInputY;
	}
	
	cerr << "\n1 - progressive (YYYYXXXXX)\n"
	<< "2 - individual (XXXYXXXXX)\n"
	<< "Please choose how voters change:";
	if(progressiveOrIndv)
	{
		cerr << progressiveOrIndv << "\n";
	}
	else
	{
		cin >> progressiveOrIndv;
	}
	
	cerr << "\n1 - simple\n"
	<< "2 - verbose\n"
	<< "3 - verbose + voters\n"
	<< "Please choose a level of verbosity:";
	if(verbositySetting)
	{
		cerr << verbositySetting << "\n";
	}
	else
	{
		cin >> verbositySetting;
	}
	
	
	outputFileName << RESULTS_DIRECTORY;
	switch(tempElectionInput)
	{
		case 1: {
			electionStyle = approval;
			outputFileName << "approval-";
			break;
		}
		case 2: {
			electionStyle = plurality;
			outputFileName << "plurality-";
			break;
		}
		case 3: {
			electionStyle = antiplurality;
			outputFileName << "antiplurality-";
			break;
		}
		case 4: {
			electionStyle = borda;
			outputFileName << "borda-";
			break;
		}
		case 5: {
			electionStyle = hare;
			outputFileName << "hare-";
			break;
		}
		case 6: {
			electionStyle = copeland;
			outputFileName << "copeland-";
			break;
		}
		case 7: {
			electionStyle = simpson;
			outputFileName << "simpson-";
			break;
		}
		case 8: {
			electionStyle = schultze;
			outputFileName << "schultze-";
			break;
		}
		case 9: {
			electionStyle = coombs;
			outputFileName << "coombs-";
			break;
		}
		//Aggressively suppress compilation warnings about uninitialized variables
		//using default case.
		default: electionStyle = numElectionTypes;
			cerr << "ERROR unexpected value in " + string(__FILE__) + " at " + to_string(__LINE__) + "\n";
			exit(6);
			break;
	}

	switch(progressiveOrIndv)
	{
		case 1:{
			outputFileName << "P.";
			break;
		}
		case 2:{
			outputFileName << "I.";
			break;
		}
		default:
			cerr << "ERROR unexpected value in " + string(__FILE__) + " at " + to_string(__LINE__) + "\n";
			exit(6);
			break;
	}

	switch(tempPollInputX)
	{
		case 1:{
			firstPollMethod = strat_allZero;
			outputFileName << "allZero";
			break;}
		case 2:{
			firstPollMethod = strat_approval;
			outputFileName << "approval";
			break;}
		case 3:{
			firstPollMethod = strat_plurality;
			outputFileName << "plur";
			break;}
		case 4:{
			firstPollMethod = strat_antiplurality;
			outputFileName << "antiplur";
			break;}
/* 		case 5:{
			firstPollMethod = strat_distanceToWin;
			outputFileName << "distance2Win";
			break;} */
		case 6:{
			firstPollMethod = strat_bordaEval;
			outputFileName << "bordaEval";
			break;}
		case 7:{
			firstPollMethod = strat_pairwise;
			outputFileName << "pairwise";
			break;}
		case 8:{
			firstPollMethod = strat_hareEliminationSpeed;
			outputFileName << "hareScore";
			break;}
		case 9:{
			firstPollMethod = strat_copeland;
			outputFileName << "copelandScore";
			break;}
		case 10:{
			firstPollMethod = strat_simpson;
			outputFileName << "simpsonScore";
			break;}
		case 11:{
			firstPollMethod = strat_victoryPairwise;
			outputFileName << "victoryPairwise";
			break;}
		case 12:{
			firstPollMethod = strat_winOrLosePairwise;
			outputFileName << "winOrLosePairwise";
			break;}
		default:
			firstPollMethod = strat_sentinelValue;
			cerr << "ERROR unexpected value in " + string(__FILE__) + " at " + to_string(__LINE__) + "\n";
			exit(6);
			break;
	}
	
	
	switch(tempPairInputX)
	{
		case 1:{
			firstPairMethod = prioritizeLeader;
			outputFileName << "Leader";
			break;}
		case 2:{
			firstPairMethod = prioritizeRunnerUp ;
			outputFileName << "RunnerUp";
			break;}
		case 3:{
			firstPairMethod = neighbors ;
			outputFileName << "Neigh";
			break;}
		default: firstPairMethod = pair_sentinelValue;
			cerr << "ERROR unexpected value in " + string(__FILE__) + " at " + to_string(__LINE__) + "\n";
			exit(6);
			break;
	}
	
	switch(tempBuildInputX)
	{
		case 1:{
			firstBuilderMethod = bothEnds ;
			outputFileName << "BothEnds";
			break;}
		case 2:{
			firstBuilderMethod = front ;
			outputFileName << "Front";
			break;}
		case 3:{
			firstBuilderMethod = back ;
			outputFileName << "Back";
			break;}
		default: firstBuilderMethod = build_sentinelValue;
			cerr << "ERROR unexpected value in " + string(__FILE__) + " at " + to_string(__LINE__) + "\n";
			exit(6);
			break;
	}
	
	outputFileName << ".";
	
	switch(tempPollInputY)
	{
		case 1:{
			secondPollMethod = strat_allZero;
			outputFileName << "allZero";
			break;}
		case 2:{
			secondPollMethod = strat_approval;
			outputFileName << "approval";
			break;}
		case 3:{
			secondPollMethod = strat_plurality;
			outputFileName << "plur";
			break;}
		case 4:{
			secondPollMethod = strat_antiplurality;
			outputFileName << "antiplur";
			break;}
/* 		case 5:{
			secondPollMethod = strat_distanceToWin;
			outputFileName << "distance2Win";
			break;} */
		case 6:{
			secondPollMethod = strat_bordaEval;
			outputFileName << "bordaEval";
			break;}
		case 7:{
			secondPollMethod = strat_pairwise;
			outputFileName << "pairwise";
			break;}
		case 8:{
			secondPollMethod = strat_hareEliminationSpeed;
			outputFileName << "hareScore";
			break;}
		case 9:{
			secondPollMethod = strat_copeland;
			outputFileName << "copelandScore";
			break;}
		case 10:{
			secondPollMethod = strat_simpson;
			outputFileName << "simpsonScore";
			break;}
		case 11:{
			secondPollMethod = strat_victoryPairwise;
			outputFileName << "victoryPairwise";
			break;}
		case 12:{
			secondPollMethod = strat_winOrLosePairwise;
			outputFileName << "winOrLosePairwise";
			break;}
		default: secondPollMethod = strat_sentinelValue;
			cerr << "ERROR unexpected value in " + string(__FILE__) + " at " + to_string(__LINE__) + "\n";
			exit(6);
			break;
	}
	
	switch(tempPairInputY)
	{
		case 1:{
			secondPairMethod = prioritizeLeader;
			outputFileName << "Leader";
			break;}
		case 2:{
			secondPairMethod = prioritizeRunnerUp ;
			outputFileName << "RunnerUp";
			break;}
		case 3:{
			secondPairMethod = neighbors ;
			outputFileName << "Neigh";
			break;}
		default: secondPairMethod = pair_sentinelValue;
			cerr << "ERROR unexpected value in " + string(__FILE__) + " at " + to_string(__LINE__) + "\n";
			exit(6);
			break;
	}
	
	switch(tempBuildInputY)
	{
		case 1:{
			secondBuilderMethod = bothEnds ;
			outputFileName << "BothEnds";
			break;}
		case 2:{
			secondBuilderMethod = front ;
			outputFileName << "Front";
			break;}
		case 3:{
			secondBuilderMethod = back ;
			outputFileName << "Back";
			break;}
		default: secondBuilderMethod = build_sentinelValue;
			cerr << "ERROR unexpected value in " + string(__FILE__) + " at " + to_string(__LINE__) + "\n";
			exit(6);
			break;
	}
	
	
	outputFileName << "." << to_string(time(0));
	outputFileName << ".txt";
	debugFilename << "debug-" << outputFileName.str();
	
	//string inputBuffer = "";
	//int inputInt,inputVoterCount;
	vector<vector<int>> inputVoterValues;
	vector<int> inputValues;
	vector<Voter> testVoters;
	vector<electorateExperiment> manyElectorates;
	int voterIDCounter = 0;

	Election defaultElection(CANDIDATE_COUNT,electionStyle);

	for(unsigned electorateCount = 0; electorateCount < ELECTORATE_COUNT; electorateCount++)
	{
		voterIDCounter = 0;
		electorateExperiment tempExperimentContainer;
		for(unsigned int voterID = voterIDCounter; voterID < VOTER_COUNT; voterID++)
		{
			tempExperimentContainer.theseVoters.push_back(Voter(voterID,defaultElection,firstPollMethod,firstPairMethod,firstBuilderMethod));
			voterIDCounter++;
		}
		manyElectorates.push_back(tempExperimentContainer);
	}
	researchResults << "Seed: " << voting_seed << "\n";
	bool needsFirstRun = true, needsSecondRun = true;
	int totalTotal = 0;
	while(needsFirstRun || needsSecondRun)
	{
		researchResults << "Election: ";
		switch(electionStyle)
		{
			case approval: researchResults << "approval"; break;
			case plurality: researchResults << "plurality"; break;
			case antiplurality: researchResults << "antiplurality"; break;
			case borda: researchResults << "borda"; break;
			case hare: researchResults << "hare"; break;
			case copeland: researchResults << "copeland"; break;
			case simpson: researchResults << "simpson"; break;
			case schultze: researchResults << "schultze"; break;
			case coombs: researchResults << "coombs"; break;
			case numElectionTypes: researchResults << ""; break;
		}
		researchResults << "\nElectorate Count: " << ELECTORATE_COUNT;
		researchResults << "\nVoter Count: " << VOTER_COUNT;
		researchResults << "\nCandidate Count: " << CANDIDATE_COUNT;
		researchResults << "\nRounds In Each Election: " << NUMBER_OF_ROUNDS;
		researchResults << "\nFrom: " << stratToString(firstPollMethod,firstPairMethod,firstBuilderMethod) << "\n";
		researchResults <<   "To:   " << stratToString(secondPollMethod,secondPairMethod,secondBuilderMethod) << "\n";
		switch(progressiveOrIndv)
		{
			case 1: researchResults << "Progressively changing voters' strategies and keeping changes (YYYYYXXXX)\n"; break;
			case 2: researchResults << "Individually changing voters' strategies, testing, then restoring their original strategy (XXXXYXXXX)\n"; break;
		}
		
		//https://stackoverflow.com/questions/261963/how-can-i-iterate-over-an-enum for the constructor(type+1) idea.
		for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
		{
			electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
			/* for(unsigned int voterID = 0; voterID < VOTER_COUNT; voterID++)
			{
				manyElectorates.at(electorateCount).theseVoters.at(voterID).setStrategy(firstPollMethod,pairing1ToTry,builder1ToTry);
			} */
			Election tempElection = defaultElection;
			tie(experimentReference.initialWinnerRoundCount,experimentReference.initialElectionStable)
				= tempElection.runElection(NUMBER_OF_ROUNDS,experimentReference.theseVoters);
			experimentReference.initialWinner = tempElection.getWinner();
			if(!needsFirstRun)
			{//After the first run, but before the second, we need to clear out
			//	vector<int> winners,individualAdjustments;
				fill(experimentReference.winners.begin(),experimentReference.winners.end(),-1);
				fill(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),0);				
			}
			//cout << "Electorate " << electorateCount << "Winner " << static_cast<char>(experimentReference.initialWinner + 'A') << "\n";
		}
	/* 	for(pollMethod poll2ToTry = FIRST_STRAT; poll2ToTry < STRAT_MAX; poll2ToTry = pollMethod(poll2ToTry + 1))
		{
			for(pairingMethod pairing2ToTry = firstPairMethodING; pairing2ToTry < PAIR_MAX; pairing2ToTry = pairingMethod(pairing2ToTry + 1))
			{
				for(ballotBuildMethod builder2ToTry = firstBuilderMethod; builder2ToTry < BUILDER_MAX; builder2ToTry = ballotBuildMethod(builder2ToTry + 1))
				{
					researchOutput << "Comparing " << ;
				}
			}
		} */
		vector<int> totalPlusses(VOTER_COUNT,0);
		vector<int> totalMinuses(VOTER_COUNT,0);
		vector<int> totalRoundsVector(VOTER_COUNT,0);
		vector<int> totalRoundsWhenStableVector(VOTER_COUNT,0);
		int totalRounds = 0, totalStable = 0, totalRoundsWhenStable = 0;
		for(unsigned voterIndex = 0; voterIndex < VOTER_COUNT; voterIndex++)
		{
			for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
			{
				bool throwAwayTempBool = false;
				electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
				if(experimentReference.theseVoters.size() != VOTER_COUNT)
				{
					cerr << "ERROR incorrect voter count in electorate\n";
					exit(5);
				}
				Election testElection = defaultElection;
				experimentReference.theseVoters.at(voterIndex).setStrategy(secondPollMethod,secondPairMethod,secondBuilderMethod);
				tie(experimentReference.roundsToFinish.at(voterIndex),throwAwayTempBool) = testElection.runElection(NUMBER_OF_ROUNDS,experimentReference.theseVoters);
				experimentReference.electionWasStable.at(voterIndex) = throwAwayTempBool;
				totalRounds += experimentReference.roundsToFinish.at(voterIndex);
				totalRoundsVector.at(voterIndex) += experimentReference.roundsToFinish.at(voterIndex);
				totalRoundsWhenStableVector.at(voterIndex) += (experimentReference.electionWasStable.at(voterIndex) ? experimentReference.roundsToFinish.at(voterIndex) : 0);
				totalRoundsWhenStable += (experimentReference.electionWasStable.at(voterIndex) ? experimentReference.roundsToFinish.at(voterIndex) : 0);
				totalStable += (experimentReference.electionWasStable.at(voterIndex) ? 1 : 0);
				experimentReference.winners.at(voterIndex) = testElection.getWinner();
				int oldWinner;
				oldWinner = ((voterIndex == 0) || (progressiveOrIndv == 2) ? experimentReference.initialWinner : experimentReference.winners.at(voterIndex-1));
				// experimentReference.individualAdjustments.at(voterIndex) = 0;
				if(oldWinner != experimentReference.winners.at(voterIndex))
				{//The winner changed.
					experimentReference.individualAdjustments.at(voterIndex) = experimentReference.theseVoters.at(voterIndex).getHappinessShift(oldWinner,experimentReference.winners.at(voterIndex));
					totalPlusses.at(voterIndex) += (experimentReference.individualAdjustments.at(voterIndex) > 0 ? 1 : 0);
					totalMinuses.at(voterIndex) += (experimentReference.individualAdjustments.at(voterIndex) < 0 ? 1 : 0);
					// oldWinner = experimentReference.winners.at(voterIndex);
				}
				if(progressiveOrIndv >= 2)
				{
					experimentReference.theseVoters.at(voterIndex).setStrategy(firstPollMethod,firstPairMethod,firstBuilderMethod);
				}
			}
		}
		int totalResult = 0, totalUps = 0, totalDowns = 0;
		vector<int> totalChanges(VOTER_COUNT,0);
		vector<int> totalUp(VOTER_COUNT,0);
		vector<int> totalDown(VOTER_COUNT,0);
		vector<int> voterIDiota(VOTER_COUNT,0);
		vector<int> tempVec(VOTER_COUNT,0);
		for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
		{
			electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
			researchOutput << "Electorate " << right << setw(2) << electorateCount;
			researchOutput << left << " - Initial Winner: " << static_cast<char>(experimentReference.initialWinner + 'A') 
				<< " Rounds ran: " << experimentReference.initialWinnerRoundCount << (experimentReference.initialElectionStable ? " (STABLE)" : " (UNSTABLE)") << "\n";
			if(verbositySetting >= 3)
			{
				researchOutput << "Voters:\n";
				for(unsigned voterIndex = 0; voterIndex < experimentReference.theseVoters.size(); voterIndex++)
				{
					researchOutput << right << setw(3) << voterIndex << left << ":";
					researchOutput << experimentReference.theseVoters.at(voterIndex).toString() << "\n";
				}
			}
			iota(voterIDiota.begin(),voterIDiota.end(),0);
			const int outputWidth = 6;
			researchOutput << vectorString(voterIDiota,outputWidth) << " - VoterID\n";
			researchOutput << vectorString(experimentReference.winners,outputWidth,true) << " - Winners\n";
			researchOutput << vectorString(experimentReference.individualAdjustments,outputWidth) << " - Value\n";
			researchOutput << vectorString(experimentReference.roundsToFinish,outputWidth) << " - Rounds\n";
			researchOutput << vectorString(experimentReference.electionWasStable,outputWidth) << " - Stable\n";
			int thisResult = accumulate(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),0);
			researchOutput << "---- Change for this electorate: " << thisResult;
			totalResult += thisResult;
			if(!needsFirstRun)
				totalTotal -= thisResult;
			else
				totalTotal += thisResult;
			int thisUps = count_if(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),[](int inputVal){return inputVal > 0;});
			researchOutput << " up: " << thisUps;
			totalUps += thisUps;
			int thisDowns = count_if(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),[](int inputVal){return inputVal < 0;});
			researchOutput << " down: " << thisDowns;
			totalDowns += thisDowns;
			researchOutput << " ----\n";
			//https://stackoverflow.com/questions/28607912/sum-values-of-2-vectors
			transform(totalChanges.begin(),totalChanges.end(),experimentReference.individualAdjustments.begin(),totalChanges.begin(),plus<>());
			transform(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),tempVec.begin(),[](int val1){return (val1 > 0 ? val1 : 0);});
			transform(totalUp.begin(),totalUp.end(),tempVec.begin(),totalUp.begin(),plus<>());
			tempVec.clear();
			transform(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),tempVec.begin(),[](int val1){return (val1 < 0 ? val1 : 0);});
			tempVec.clear();
			transform(totalDown.begin(),totalDown.end(),tempVec.begin(),totalDown.begin(),plus<>());
			tempVec.clear();
		}
		researchResults << vectorString(voterIDiota,outputWidth) << " - VoterID\n";
		researchResults << vectorString(totalChanges,outputWidth) << " - Total Changes\n";
		researchResults << vectorString(totalUp,outputWidth) << " - Total Up\n";
		researchResults << vectorString(totalDown,outputWidth) << " - Total Down\n";
		researchResults << vectorString(totalPlusses,outputWidth) << " - Total Plusses\n";
		researchResults << vectorString(totalMinuses,outputWidth) << " - Total Minuses\n";
		researchResults << vectorString(totalRoundsVector,outputWidth) << " - Total Rounds To Finish\n";
		researchResults << vectorString(totalRoundsWhenStableVector,outputWidth) << " - Total Rounds When Stabilizing\n";
		researchResults << "====\nTotal : " << totalResult << " Ups: " << totalUps << " Downs: " <<totalDowns << "\n====\n";
		if(needsFirstRun)
		{
			needsFirstRun = false;
			swap(firstPollMethod,secondPollMethod);
			swap(firstPairMethod,secondPairMethod);
			swap(firstBuilderMethod,secondBuilderMethod);
			if(progressiveOrIndv >= 2)
			{//If only in place to make sure we're not doing unnecessary work for progressive.
				for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
				{
					electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
					for(unsigned voterIndex = 0; voterIndex < VOTER_COUNT; voterIndex++)
					{
						experimentReference.theseVoters.at(voterIndex).setStrategy(firstPollMethod,firstPairMethod,firstBuilderMethod);
					}
				}
			}
		}
		else
		{
			needsSecondRun = false;
			researchResults << "TotalTotal : " << totalTotal << "\n";
		}
	};
	if(PRINT_TO_FILE)
	{
		resultsFile.open(outputFileName.str());
		resultsFile << researchResults.str();
		resultsFile.close();
		if(verbositySetting >= 2)
		{
			debugFile.open(debugFilename.str());
			debugFile << researchOutput.str();
			debugFile.close();
		}
	}
	else
	{
		cout << researchResults.str();
		if(verbositySetting >= 2)
			cout << researchOutput.str();
	}


	if(ALERT_WHEN_DONE)
		cerr << "\a";
	return 0;
}

/*

My original effort to use the random.h generator generate random values 
for each spot in a vector turned out to be dumping the same identical 
value in all spots in one vector. Bah. Build it the old fashioned way,
I guess.

*/