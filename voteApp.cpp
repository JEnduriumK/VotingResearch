#include "vote.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

const int VOTER_COUNT = 3;
const int CANDIDATE_COUNT = 4;
const electionType ELECTION_STYLE = approval;
const int NUMBER_OF_ROUNDS = 3;
const int MAX_CANDIDATES = 40;

pollMethod electionTypeStrat = strat_approval;

/* template<typename T> string vectorString(const vector<T>&theVector, int width = 3)
{
	stringstream theStream;
	theStream << "{";
	for(unsigned int index = 0; index < theVector.size(); index++)
	{
		theStream << right << setw(width) << to_string(theVector.at(index));
		if(index+1 != theVector.size())
		{
			theStream << ",";
		}
	}
	theStream << "}";
	return theStream.str();
} */


//https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp/
//For possible file input?
int main(int argc, char *argv[])
{
	bool usingInputFile = false;
	ifstream inputFile;
	int candidateCount = 0;
	string inputBuffer = "";
	int inputInt,inputVoterCount;
	vector<vector<int>> inputVoterValues;
	vector<int> inputValues;
	vector<Voter> testVoters;
	int voterIDCounter = 0;
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
	
	Election testElection(candidateCount,ELECTION_STYLE);
	// pollMethod electionTypeStrat = strat_winOrLosePairwise;
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
/*		case hare:
			electionTypeStrat = strat_pairwise;
			// electionTypeStrat = strat_hareEliminationSpeed;
			// electionTypeStrat = strat_allZero;
			break;
 		case copeland:
		case simpson:
		case schultze:
		case coombs:
			electionTypeStrat = strat_allZero;
			break; */
		case numElectionTypes:
			cerr << "ERROR: You reached numElectionTypes at " << to_string(__LINE__) << " in " << string(__FILE__) << ", aborting.\n";
			exit(3);
			break;
			
	}
	if(usingInputFile)
	{
		//cout << "Number of Candidates: " << candidateCount << "\n";
		for(unsigned int index = 0; index < inputVoterValues.size(); index++)
		{
			// testVoters.emplace_back(inputVoterValues.at(index),voterIDCounter++,testElection,strat_allZero);
			testVoters.emplace_back(inputVoterValues.at(index),voterIDCounter++,testElection,strat_approval,prioritizeLeader,bothEnds);
		}
	}
	else//Could be if(!usingInputFile && ADD_ADDITIONAL_VOTERS)?
	{
		for(unsigned int voterID = voterIDCounter; voterID < VOTER_COUNT; voterID++)
		{
			testVoters.emplace_back(voterID,testElection,strat_winOrLosePairwise,prioritizeLeader,front);
			voterIDCounter++;
		}
	}
	//srandom(time(0));
	//cout << "Seed: " << time(0) << "\n";
	//randomEngine.seed(time(0));
	
	//Voter testVoter(0,testElection);
	
	// testVoters.at(0).setStrategy(strat_allZero);
	// testVoters.at(1).setStrategy(strat_allZero);
	// testVoters.at(2).setStrategy(strat_bordaEval);
	//cout << testVoter.toString() + '\n';
	cout << "Voters: \n";
	for(unsigned int index = 0; index < testVoters.size(); index++)
	{
		cout << setw(5) << right << testVoters.at(index).getVoterID() << left << ": " << testVoters.at(index).toString() << '\n';
	}
	// cout << "\nFirst round:\n";
	// testElection.generateBallots(testVoters);
	// cout << testElection.toString() + '\n';
	// cout << "\nSecond round:\n";
	// testElection.generateBallots(testVoters);
	// cout << testElection.toString() + '\n';
	pair<int,bool> electionStatus = testElection.runElection(NUMBER_OF_ROUNDS,testVoters);
	cout << "\aRan " << get<0>(electionStatus) << " rounds " << (get<1>(electionStatus) ? "(stabilized)" : "(unstable)") << " out of a requested " << NUMBER_OF_ROUNDS << " rounds:\n" + testElection.toString() + '\n';
	//testVoters.at(0).theElection.runElection(NUMBER_OF_ROUNDS,testVoters);
	cout << "Pairwise test: " << vectorString(testElection.currentPoll(strat_pairwise)) << '\n';
	cout << "strat_victoryPairwise test: " << vectorString(testElection.currentPoll(strat_victoryPairwise)) << '\n';
	cout << "strat_winOrLosePairwise test: " << vectorString(testElection.currentPoll(strat_winOrLosePairwise)) << '\n';
	return 0;
}

/*

My original effort to use the random.h generator generate random values 
for each spot in a vector turned out to be dumping the same identical 
value in all spots in one vector. Bah. Build it the old fashioned way,
I guess.

*/