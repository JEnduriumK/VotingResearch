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

const string RESULTS_DIRECTORY = "";
const bool PRINT_TO_FILE = false;
const bool ALERT_WHEN_DONE = false;
const bool PRINT_DETAILS = false;
const bool PRINT_VOTERS = false;//Won't work unless PRINT_DETAILS is true.

/* 
1 = debug weird hang during round1 of electorate1
 */

const string PROGRESSION_TYPE = "I";

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
	<< "7 - strat_harePairwise\n"
	<< "8 - strat_hareEliminationSpeed\n"
	<< "9 - strat_copeland\n"
	<< "10 - strat_simpson\n"
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
	<< "7 - strat_harePairwise\n"
	<< "8 - strat_hareEliminationSpeed\n"
	<< "9 - strat_copeland\n"
	<< "10 - strat_simpson\n"
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
		cerr << tempBuildInputY << "\n\n\n";
	}
	else
	{
		cin >> tempBuildInputY;
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

	outputFileName << PROGRESSION_TYPE << ".";
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
			firstPollMethod = strat_harePairwise;
			outputFileName << "harePair";
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
			secondPollMethod = strat_harePairwise;
			outputFileName << "harePair";
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
	
	outputFileName << ".txt";
	debugFilename << "debug-" << outputFileName.str();
	
	//string inputBuffer = "";
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
	Election defaultElection(CANDIDATE_COUNT,electionStyle);
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
			tempExperimentContainer.theseVoters.push_back(Voter(voterID,defaultElection,firstPollMethod,firstPairMethod,firstBuilderMethod));
			//cout << electorateCount << "-" << voterID << ":" << tempExperimentContainer.theseVoters.back().toString() << "\n";
			voterIDCounter++;
		}
		//tempExperimentContainer.theElectionObject = defaultElection;
		manyElectorates.push_back(tempExperimentContainer);
	}
//	}
	
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
	researchResults << "XXXXYXXXXXX\n";
	
	//https://stackoverflow.com/questions/261963/how-can-i-iterate-over-an-enum for the constructor(type+1) idea.
	for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
	{
		electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
		/* for(unsigned int voterID = 0; voterID < VOTER_COUNT; voterID++)
		{
			manyElectorates.at(electorateCount).theseVoters.at(voterID).setStrategy(firstPollMethod,pairing1ToTry,builder1ToTry);
		} */
		Election tempElection = defaultElection;
		tempElection.runElection(NUMBER_OF_ROUNDS,experimentReference.theseVoters);
		experimentReference.initialWinner = tempElection.getWinner();
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
			experimentReference.theseVoters.at(voterIndex).setStrategy(secondPollMethod,secondPairMethod,secondBuilderMethod);
			testElection.runElection(NUMBER_OF_ROUNDS,experimentReference.theseVoters);
			experimentReference.winners.at(voterIndex) = testElection.getWinner();
			int oldWinner;
			oldWinner = (voterIndex == 0 ? experimentReference.initialWinner : experimentReference.winners.at(voterIndex-1));
			if(oldWinner != experimentReference.winners.at(voterIndex))
			{//The winner changed.
				experimentReference.individualAdjustments.at(voterIndex) = experimentReference.theseVoters.at(voterIndex).getHappinessShift(oldWinner,experimentReference.winners.at(voterIndex));
				totalPlusses.at(voterIndex) += (experimentReference.individualAdjustments.at(voterIndex) > 0 ? 1 : 0);
				totalMinuses.at(voterIndex) += (experimentReference.individualAdjustments.at(voterIndex) < 0 ? 0 : 1);
				oldWinner = experimentReference.winners.at(voterIndex);
			}
			experimentReference.theseVoters.at(voterIndex).setStrategy(firstPollMethod,firstPairMethod,firstBuilderMethod);
		}
	}
	int totalResult = 0, totalUps = 0, totalDowns = 0;
	vector<int> totalChanges(VOTER_COUNT,0);
	vector<int> totalUp(VOTER_COUNT,0);
	vector<int> totalDown(VOTER_COUNT,0);

	vector<int> tempVec(VOTER_COUNT,0);
	for(unsigned electorateCount = 0; electorateCount < manyElectorates.size(); electorateCount++)
	{
		electorateExperiment&experimentReference = manyElectorates.at(electorateCount);
		researchOutput << "Electorate " << right << setw(2) << electorateCount;
		researchOutput << left << " - Initial Winner: " << static_cast<char>(experimentReference.initialWinner + 'A') << "\n";
		if(PRINT_DETAILS && PRINT_VOTERS)
		{
			researchOutput << "Voters:\n";
			for(unsigned voterIndex = 0; voterIndex < experimentReference.theseVoters.size(); voterIndex++)
			{
				researchOutput << right << setw(3) << voterIndex << left << ":";
				researchOutput << experimentReference.theseVoters.at(voterIndex).toString() << "\n";
			}
		}
		iota(tempVec.begin(),tempVec.end(),0);
		researchOutput << vectorString(tempVec,3) << " - VoterID\n";
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
		//https://stackoverflow.com/questions/28607912/sum-values-of-2-vectors
		transform(totalChanges.begin(),totalChanges.end(),experimentReference.individualAdjustments.begin(),totalChanges.begin(),plus<>());
		transform(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),tempVec.begin(),[](int val1){return (val1 > 0 ? val1 : 0);});
		transform(totalUp.begin(),totalUp.end(),tempVec.begin(),totalUp.begin(),plus<>());
		transform(experimentReference.individualAdjustments.begin(),experimentReference.individualAdjustments.end(),tempVec.begin(),[](int val1){return (val1 < 0 ? val1 : 0);});
		transform(totalDown.begin(),totalDown.end(),tempVec.begin(),totalDown.begin(),plus<>());
	}
	iota(tempVec.begin(),tempVec.end(),0);
	researchResults << vectorString(tempVec,4) << " - VoterID\n";
	researchResults << vectorString(totalChanges,4) << " - Total Changes\n";
	researchResults << vectorString(totalUp,4) << " - Total Up\n";
	researchResults << vectorString(totalDown,4) << " - Total Down\n";
	researchResults << vectorString(totalPlusses,4) << " - Total Plusses\n";
	researchResults << vectorString(totalMinuses,4) << " - Total Minuses\n";
	researchResults << "====\nTotal : " << totalResult << " Ups: " << totalUps << " Downs: " <<totalDowns << "\n====\n";
	if(PRINT_TO_FILE)
	{
		resultsFile.open(outputFileName.str());
		resultsFile << researchResults.str();
		resultsFile.close();
		if(PRINT_DETAILS)
		{
			debugFile.open(debugFilename.str());
			debugFile << researchOutput.str();
			debugFile.close();
		}
	}
	else
	{
		cout << researchResults.str();
		if(PRINT_DETAILS)
			cout << researchOutput.str();
	}
	
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