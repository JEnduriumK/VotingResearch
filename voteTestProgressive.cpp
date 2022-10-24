#include "vote.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

const int ELECTORATE_COUNT = 100;
const int VOTER_COUNT = 30;
const int CANDIDATE_COUNT = 6;
const int NUMBER_OF_ROUNDS = 10;
const int MAX_CANDIDATES = 40;

const int DEBUG_MODE = 1;

const bool PRINT_VOTERS = false;
/* 
1 = debug weird hang during round1 of electorate1
 */

const electionType ELECTION_STYLE = approval;

const pollMethod FIRST_POLL = strat_approval;
const pairingMethod FIRST_PAIR = neighbors;
const ballotBuildMethod FIRST_BUILDER = bothEnds;

const pollMethod SECOND_POLL = strat_approval;
const pairingMethod SECOND_PAIR = prioritizeRunnerUp;
const ballotBuildMethod SECOND_BUILDER = bothEnds;

struct electorateExperiment {
	//Election theElectionObject;
	int initialWinner;
	vector<Voter> theseVoters;
	vector<int> winners,individualAdjustments;
	//vector<bool> changes;
	electorateExperiment() : 
	winners(VOTER_COUNT,-1),
	individualAdjustments(VOTER_COUNT,0)
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
	//bool usingInputFile = false;
	//ifstream inputFile;
	//int candidateCount = 0;
	string inputBuffer = "";
	//int inputInt,inputVoterCount;
	vector<vector<int>> inputVoterValues;
	vector<int> inputValues;
	vector<Voter> testVoters;
	vector<electorateExperiment> manyElectorates;
	int voterIDCounter = 0;
	/* 
	if(argc > 1)
	{
		//https://cplusplus.com/reference/fstream/fstream/open/
		//Maybe use https://pubs.opengroup.org/onlinepubs/9699919799/functions/access.html to check for existence?
		//Based on https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
		inputFile.open(argv[1],fstream::in);
		usingInputFile = true;
		inputFile >> candidateCount >> inputBuffer;
		if(inputBuffer.find("candidate") == string::npos || !((candidateCount < MAX_CANDIDATES) && (candidateCount > 0)))
		{
			//cout << candidateCount << "\n" <<  inputBuffer << "\n";
			cerr << "Unexpected file format, aborting.\n";
			return 1;
		}
		//https://stackoverflow.com/questions/22290891/reading-in-file-with-delimiter
		while(getline(inputFile,inputBuffer,':'))
		{//Grab how many voters we're building.
			// cout << "First buffer: " << inputBuffer << '\n';
			try //https://en.cppreference.com/w/cpp/language/try_catch
			{//Make sure the first value is an integer.
				inputInt = stoi(inputBuffer);
			}
			catch(exception &Ex)
			{
				cerr << "\nUnexpected non-integer for first value in line!\n";
				exit(2);
			}
			inputVoterCount = inputInt;//Save how many voters we're making.
			//inputBuffer = "";//Clear the buffer just in case?
			//https://stackoverflow.com/questions/37957080/can-i-use-2-or-more-delimiters-in-c-function-getline
			getline(inputFile,inputBuffer);//Grab the rest of the line, their preferences.
			stringstream inputStream(inputBuffer);
			// cout << inputInt << " - The buffer was: " << inputBuffer << "\n";
			// cout << "Hrrrgh: " << inputStream.str() << '\n';
			inputBuffer = "";
			for(int index = 0; index < candidateCount; index++)
			{
				// cout << inputStream.str() << " ARRGH\n";
				inputStream >> inputInt;
				// cout << "Waagh: " << inputInt << '\n';
				//cout << "Grabbed " << inputInt << "\n";
				// try //https://en.cppreference.com/w/cpp/language/try_catch
				// {//Make sure these values are an integer, too.
					// inputInt = stoi(inputBuffer);
				// }
				// catch(exception &Ex)
				// {
					// cerr << "\nUnexpected non-integer for voter preference!\n";
					// return 2;
				// }
				//cout << inputInt << " - Inputted\n";
				inputValues.push_back(inputInt);
			}
			for(int index = 0; index < inputVoterCount; index++)
			{//Shove that collection of preferences into the vector of vectors of ints.
				//We'll send these to the voter constructor later.
				inputVoterValues.push_back(inputValues);
			}
			inputValues.clear();
			//cout << "Generated " << inputVoterValues.size() << " voters.\n";
		}
		inputFile.close();
	}
	else
	{
		candidateCount = CANDIDATE_COUNT;
	}
	 */
	Election defaultElection(CANDIDATE_COUNT,ELECTION_STYLE);
/* 	pollMethod electionTypeStrat = strat_approval;
	switch(ELECTION_STYLE)
	{
		case approval:
			electionTypeStrat = strat_approval;
			break;
		case plurality:
			electionTypeStrat = strat_plurality;
			break;
		case antiplurality:
			electionTypeStrat = strat_antiplurality;
			break;
		case borda:
			electionTypeStrat = strat_bordaEval;
			break;
		case hare:
			electionTypeStrat = strat_harePairwise;
			// electionTypeStrat = strat_hareEliminationSpeed;
			// electionTypeStrat = strat_allZero;
			
			break;
		case numElectionTypes:
			cerr << "ERROR: You reached numElectionTypes at " << to_string(__LINE__) << " in " << string(__FILE__) << ", aborting.\n";
			exit(3);
			break;
			
	} */
/* 	
	if(usingInputFile)
	{
		//cout << "Number of Candidates: " << candidateCount << "\n";
		for(unsigned int index = 0; index < inputVoterValues.size(); index++)
		{
			//testVoters.emplace_back(inputVoterValues.at(index),voterIDCounter++,defaultElection,strat_allZero);
			testVoters.emplace_back(inputVoterValues.at(index),voterIDCounter++,defaultElection,electionTypeStrat);
		}
	}
	else//Could be if(!usingInputFile && ADD_ADDITIONAL_VOTERS)?
	 */
//	{
	for(unsigned electorateCount = 0; electorateCount < ELECTORATE_COUNT; electorateCount++)
	{
		voterIDCounter = 0;
		electorateExperiment tempExperimentContainer;
		for(unsigned int voterID = voterIDCounter; voterID < VOTER_COUNT; voterID++)
		{
			tempExperimentContainer.theseVoters.push_back(Voter(voterID,defaultElection,FIRST_POLL,FIRST_PAIR,FIRST_BUILDER));
			//cout << electorateCount << "-" << voterID << ":" << tempExperimentContainer.theseVoters.back().toString() << "\n";
			voterIDCounter++;
		}
		//tempExperimentContainer.theElectionObject = defaultElection;
		manyElectorates.push_back(tempExperimentContainer);
	}
//	}
	stringstream researchOutput;
	researchOutput << "Election: ";
	switch(ELECTION_STYLE)
	{
		case approval: researchOutput << "approval"; break;
		case plurality: researchOutput << "plurality"; break;
		case antiplurality: researchOutput << "antiplurality"; break;
		case borda: researchOutput << "borda"; break;
		case hare: researchOutput << "hare"; break;
		case copeland: researchOutput << "copeland"; break;
		case simpson: researchOutput << "simpson"; break;
		case schultze: researchOutput << "schultze"; break;
		case coombs: researchOutput << "coombs"; break;
		case numElectionTypes: researchOutput << ""; break;
	}
	researchOutput << "\nElectorate Count: " << ELECTORATE_COUNT;
	researchOutput << "\nVoter Count: " << VOTER_COUNT;
	researchOutput << "\nCandidate Count: " << CANDIDATE_COUNT;
	researchOutput << "\nRounds In Each Election: " << NUMBER_OF_ROUNDS;
	researchOutput << "\nFrom: " << stratToString(FIRST_POLL,FIRST_PAIR,FIRST_BUILDER) << "\n";
	researchOutput <<   "To:   " << stratToString(SECOND_POLL,SECOND_PAIR,SECOND_BUILDER) << "\n";
	researchOutput << "YYYYYXXXXXX\n";
	
	//https://stackoverflow.com/questions/261963/how-can-i-iterate-over-an-enum for the constructor(type+1) idea.
	for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
	{
		electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
		/* for(unsigned int voterID = 0; voterID < VOTER_COUNT; voterID++)
		{
			manyElectorates.at(electorateCount).theseVoters.at(voterID).setStrategy(FIRST_POLL,pairing1ToTry,builder1ToTry);
		} */
		Election tempElection = defaultElection;
		tempElection.runElection(NUMBER_OF_ROUNDS,experimentReference.theseVoters);
		experimentReference.initialWinner = tempElection.getWinner();
		//cout << "Electorate " << electorateCount << "Winner " << static_cast<char>(experimentReference.initialWinner + 'A') << "\n";
	}
/* 	for(pollMethod poll2ToTry = FIRST_STRAT; poll2ToTry < STRAT_MAX; poll2ToTry = pollMethod(poll2ToTry + 1))
	{
		for(pairingMethod pairing2ToTry = FIRST_PAIRING; pairing2ToTry < PAIR_MAX; pairing2ToTry = pairingMethod(pairing2ToTry + 1))
		{
			for(ballotBuildMethod builder2ToTry = FIRST_BUILDER; builder2ToTry < BUILDER_MAX; builder2ToTry = ballotBuildMethod(builder2ToTry + 1))
			{
				researchOutput << "Comparing " << ;
			}
		}
	} */
	for(unsigned voterIndex = 0; voterIndex < VOTER_COUNT; voterIndex++)
	{
		for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
		{
			electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
			if(experimentReference.theseVoters.size() != VOTER_COUNT)
			{
				cerr << "ERROR incorrect voter count in electorate\n";
				exit(5);
			}
			Election testElection = defaultElection;
			experimentReference.theseVoters.at(voterIndex).setStrategy(SECOND_POLL,SECOND_PAIR,SECOND_BUILDER);
			testElection.runElection(NUMBER_OF_ROUNDS,experimentReference.theseVoters);
			experimentReference.winners.at(voterIndex) = testElection.getWinner();
			int oldWinner;
			oldWinner = (voterIndex == 0 ? experimentReference.initialWinner : experimentReference.winners.at(voterIndex-1));
			if(oldWinner != experimentReference.winners.at(voterIndex))
			{//The winner changed.
				experimentReference.individualAdjustments.at(voterIndex) = experimentReference.theseVoters.at(voterIndex).getHappinessShift(oldWinner,experimentReference.winners.at(voterIndex));
				oldWinner = experimentReference.winners.at(voterIndex);
			}
		}
	}
	int totalResult = 0, totalUps = 0, totalDowns = 0;
	for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
	{
		electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
		researchOutput << "Electorate " << right << setw(2) << electorateCount;
		researchOutput << left << " - Initial Winner: " << static_cast<char>(experimentReference.initialWinner + 'A') << "\n";
		if(PRINT_VOTERS)
		for(unsigned voterIndex = 0; voterIndex < experimentReference.theseVoters.size(); voterIndex++)
		{
			researchOutput << experimentReference.theseVoters.at(voterIndex).toString() << "\n";
		}
		researchOutput << vectorString(experimentReference.winners,3,true) << " - Winners\n";
		researchOutput << vectorString(experimentReference.individualAdjustments,3) << " - Value\n";
		int thisResult = accumulate(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),0);
		researchOutput << "---- Change for this electorate: " << thisResult;
		totalResult += thisResult;
		int thisUps = count_if(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),[](int inputVal){return inputVal > 0;});
		researchOutput << " up: " << thisUps;
		totalUps += thisUps;
		int thisDowns = count_if(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),[](int inputVal){return inputVal < 0;});
		researchOutput << " down: " << thisDowns;
		totalDowns += thisDowns;
		researchOutput << " ----\n";		
	}
	researchOutput << "====\nTotal : " << totalResult << " Ups: " << totalUps << " Downs: " <<totalDowns << "\n====\n";
	cout << researchOutput.str();
	//srandom(time(0));
	//cout << "Seed: " << time(0) << "\n";
	//randomEngine.seed(time(0));
	
	//Voter testVoter(0,defaultElection);
	
	// testVoters.at(0).setStrategy(strat_allZero);
	// testVoters.at(1).setStrategy(strat_allZero);
	// testVoters.at(2).setStrategy(strat_bordaEval);
	//cout << testVoter.toString() + '\n';
	/* cout << "Voters: \n";
	for(unsigned int index = 0; index < testVoters.size(); index++)
	{
		cout << setw(5) << right << testVoters.at(index).getVoterID() << left << ": " << testVoters.at(index).toString() << '\n';
	}
	// cout << "\nFirst round:\n";
	// defaultElection.generateBallots(testVoters);
	// cout << defaultElection.toString() + '\n';
	// cout << "\nSecond round:\n";
	// defaultElection.generateBallots(testVoters);
	// cout << defaultElection.toString() + '\n';
	pair<int,bool> electionStatus = defaultElection.runElection(NUMBER_OF_ROUNDS,testVoters);
	cout << "\aRan " << get<0>(electionStatus) << " rounds " << (get<1>(electionStatus) ? "(stabilized)" : "(unstable)") << " out of a requested " << NUMBER_OF_ROUNDS << " rounds:\n" + defaultElection.toString() + '\n';
	//testVoters.at(0).theElection.runElection(NUMBER_OF_ROUNDS,testVoters);
	cout << "Pairwise test: " << vectorString(defaultElection.currentPoll(strat_harePairwise)) << '\n'; */
	cerr << "\a";
	return 0;
}

/*

My original effort to use the random.h generator generate random values 
for each spot in a vector turned out to be dumping the same identical 
value in all spots in one vector. Bah. Build it the old fashioned way,
I guess.

*/