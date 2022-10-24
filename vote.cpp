//Get equal values on equally rated preferences at Ctrl+F: candidatesToConsider

#include "vote.h"
#include <algorithm>
#include <chrono>
#include <tuple>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <map>
#include <climits>
#include <cmath>
#include <time.h>

const int DEBUG_MODE = 1;//+4+8+16+64;
const bool PRINT_HARE_RESULTS = true;
/*
0	= Suppress errors? Nothing should be assigned?
1	= Show errors.
2	= generateBallots core dump search debug, probably can remove?
4	= Debug Groupings
8	= Debugging ballot builder order.
16	= I don't remember. Round ballot changes?
32	= Hare testing
64	= currentPoll() strat_pairwise debug.
128	= some other currentPoll debugging?
256 = Chaos theory, or a bug? Progressive changes debugging.
512 = 
*/

// long time_electionConstructor, time_currentPollDefault, time_currentPollType, time_currentPollMethod, time_generateBallots, time_getWinner, time_runHare, time_runElection, time_shiftPairwise;
// long time_ballotConstructor, time_normalizedBallot;
// long time_getPairings, time_generateBallot, time_setStrategy;
// long time_sortedIndexes;

//int voting_seed = time(0);
int voting_seed = 715131022;
default_random_engine randomEngine(voting_seed);
uniform_int_distribution<int> theDistribution(MIN_RATING,MAX_RATING);


void debugOutput(string const &theString)
{//Uncomment to run DEBUG_MODE.
	cerr << theString; //2>&1
}
void debugOutput(string const &theString,const int &debugLevel,int indent = 0)
{//Gatekeeping the debugOutput(string) function behind a debugLevel, so I don't have to bake if() statements in to all my debug lines in the main code.
	if((DEBUG_MODE & debugLevel) == debugLevel)
		debugOutput(string(indent,' ') + theString);
}
void debugOutput(stringstream const&theStringStream,const int&debugLevel,int indent = 0)
{
	if((DEBUG_MODE & debugLevel) == debugLevel)
		debugOutput(string(indent,' ') + theStringStream.str());
}

/*
Converts a 1-dimensional vector to printable output. Ex: {47, 35, 20}
*/

//https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
//Wait, do I even need this?
vector<int> sorted_indexes(const vector<int> theVectorToFakeSort, const bool reverse)
{
	// if(DEBUG_MODE & 256) == 256)
	// {
		// spectime start;
		// clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&start);
	// }
	vector<int> indexesToSort(theVectorToFakeSort.size());
	iota(indexesToSort.begin(),indexesToSort.end(),0);//Populate with index values.
	stable_sort(indexesToSort.begin(),indexesToSort.end(),
		[&theVectorToFakeSort,&reverse](int firstVal, int secondVal)
		{return (!reverse && (theVectorToFakeSort.at(firstVal) > theVectorToFakeSort.at(secondVal)))
					||
				(reverse && (theVectorToFakeSort.at(firstVal) < theVectorToFakeSort.at(secondVal)));});
	return indexesToSort;
	//stable just for determinism/consistency sake.
}

string stratToString(pollMethod aPollMethod, pairingMethod aPairingMethod, ballotBuildMethod aBallotBuildMethod)
{
	stringstream aStream;
	//aStream << "Strat: ";
	aStream << right << setw(27);
	switch(aPollMethod)
	{
		case strat_allZero: aStream << "strat_allZero"; break;
		case strat_approval: aStream << "strat_approval"; break;
		case strat_plurality: aStream << "strat_plurality"; break;
		case strat_antiplurality: aStream << "strat_antiplurality"; break;
/* 		case strat_distanceToWin: aStream << "strat_distanceToWin"; break; */
		case strat_bordaEval: aStream << "strat_bordaEval"; break;
		case strat_copeland: aStream << "strat_copeland"; break;
		case strat_simpson: aStream << "strat_simpson"; break;
		case strat_pairwise: aStream << "strat_pairwise"; break;
		case strat_victoryPairwise: aStream << "strat_victoryPairwise"; break;
		case strat_winOrLosePairwise: aStream << "strat_winOrLosePairwise"; break;
		case strat_hareEliminationSpeed: aStream << "strat_hareEliminationSpeed"; break;
		case strat_sentinelValue: aStream << "ERROR strat_sentinelValue reached! Line " + to_string(__LINE__) + " in " + string(__FILE__) + "\n"; exit(4); break;
	}
	aStream << " " << setw(19);
	switch(aPairingMethod)
	{
		case prioritizeLeader: aStream << "prioritizeLeader"; break;
		case prioritizeRunnerUp: aStream << "prioritizeRunnerUp"; break; 
		case neighbors: aStream << "neighbors"; break; 
		case pair_sentinelValue: aStream << "ERROR pair_sentinelValue reached! Line " + to_string(__LINE__) + " in " + string(__FILE__) + "\n"; exit(4); break;
	}
	aStream << " " << setw(9);
	switch(aBallotBuildMethod)
	{
		case bothEnds: aStream << "bothEnds"; break; 
		case front: aStream << "front"; break; 
		case back: aStream << "back"; break; 
		case build_sentinelValue: aStream << "ERROR build_sentinelValue reached! Line " + to_string(__LINE__) + " in " + string(__FILE__) + "\n"; exit(4); break;
	}
	// aStream << "\n";
	return aStream.str();
}

Election::Election(int numOfCandidates, electionType theElectionType)
{
	this->numCandidates = numOfCandidates;
	this->thisElectionType = theElectionType;
	this->ballots = vector<Ballot>(0);
	this->pairwiseMatrix = vector<vector<int>>(numOfCandidates,vector<int>(numOfCandidates,0));
	//randomEngine.seed(this->seed);
}

string Election::toString() const
{
	//string returnValue;
	stringstream tempStream;
	unsigned winner = 'z';
	vector<int> oneDVector;
	vector<vector<int>> roundStates;
	// returnValue += "Seed: " + to_string(seed) + '\n';
	// returnValue += "Election Type: ";
	tempStream << "Seed: " << voting_seed << '\n';
	tempStream << "Election Type: ";
	switch(getElectionType())
	{
		// case approval: returnValue += "approval"; break;
		// case plurality: returnValue += "plurality"; break;
		// case antiplurality: returnValue += "antiplurality"; break;
		// case borda: returnValue += "borda"; break;
		// case hare: returnValue += "hare"; break;
		// default: returnValue += "ERROR!"; break;
		case approval: tempStream << "approval"; break;
		case plurality: tempStream << "plurality"; break;
		case antiplurality: tempStream << "antiplurality"; break;
		case borda: tempStream << "borda"; break;
		case hare: tempStream << "hare"; break;
		case copeland: tempStream << "copeland"; break;
		case simpson: tempStream << "simpson"; break;
		case schultze: tempStream << "schultze"; break;
		case coombs: tempStream << "coombs"; break;
		//default: tempStream << "ERROR!"; break;
		case numElectionTypes: 
			debugOutput("ERROR: You reached numElectionTypes at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
			exit(3);
			break;
	}
	tempStream << '\n';
	tempStream << "Number of Candidates: " << getNumCandidates() << '\n';
	tempStream << "Ballots: \n";
	// returnValue += '\n';
	// returnValue += "Number of Candidates: " + to_string(getNumCandidates()) + '\n';
	// returnValue += "Ballots: \n";
	for(unsigned int ballotIndex = 0; ballotIndex < this->ballots.size(); ballotIndex++)
	{
		//returnValue += to_string(ballotIndex) + ": {";
		tempStream << right << setw(5) << ballotIndex << left << ": {" << right;
		for(unsigned int index = 0; index < this->ballots.at(ballotIndex).getRatings().size(); index++)
		{
			tempStream << setw(4) << this->ballots.at(ballotIndex).getRatings().at(index);
			//returnValue += to_string(this->ballots.at(ballotIndex).getRatings().at(index));
			if(index < this->ballots.at(ballotIndex).getRatings().size()-1)
			{
				tempStream << ", ";
				//returnValue += ", ";
			}
		}
		tempStream << left << "} Normalized: {" << right;
		//returnValue += "} Normalized: {";
		for(unsigned int index = 0; index < this->ballots.at(ballotIndex).normalizedBallot(getElectionType()).size(); index++)
		{
			tempStream << setw(3) << this->ballots.at(ballotIndex).normalizedBallot(getElectionType()).at(index);
			//returnValue += to_string(this->ballots.at(ballotIndex).normalizedBallot(getElectionType()).at(index));
			if(index < this->ballots.at(ballotIndex).normalizedBallot(getElectionType()).size()-1)
			{
				tempStream << ", ";
				//returnValue += ", ";
			}
		}
		tempStream << left << "}\n";
		//returnValue += "}\n";
	}
	tempStream << "Pairwise Matrix: \n";
	for(unsigned matrixIndex = 0; matrixIndex < this->pairwiseMatrix.size(); matrixIndex++)
	{
		tempStream << static_cast<char>('A'+matrixIndex) << " " << vectorString(this->pairwiseMatrix.at(matrixIndex)) << '\n';
	}
	switch(this->getElectionType()){
		case approval:
		case plurality:
		case antiplurality:
		case borda:
		{
			vector<int> theCurrentPoll = this->currentPoll();
			tempStream << "Vote totals: " << right;
			//returnValue += "Vote totals: ";
			for(unsigned int index = 0; index < theCurrentPoll.size(); index++)
			{
				tempStream << setw(4) << theCurrentPoll.at(index);
				//returnValue += to_string(this->currentPoll().at(index));
				if(index < theCurrentPoll.size()-1)
				{
					tempStream << ", ";
					//returnValue += ", ";
				}
			}
			winner = this->getWinner();
			break;
		}
		case hare:
		{
			tie(winner,oneDVector,roundStates) = runHare();
			//https://www.tutorialspoint.com/determining-how-many-digits-there-are-in-an-integer-in-cplusplus
			break;
		}
		case copeland:
		{
			tie(winner,oneDVector) = runCopeland();			
			tempStream << "Copeland scores: " << vectorString(oneDVector) << "\n";
			break;
		}
		case simpson:
		{
			tie(winner,oneDVector) = runSimpson();
			tempStream << "Simpson scores: " << vectorString(oneDVector) << "\n";
			break;
		}
		case schultze:
		{
			tie(winner,roundStates) = runSchultze();
			tempStream << "--->These aren't rounds, they're something else...?\n";
			break;
		}
		case coombs:
		{
			tie(winner,oneDVector,roundStates) = runCoombs();
			tempStream << "Coombs rankings: " << vectorString(oneDVector) << "\n";
			break;
		}
		case numElectionTypes:
			debugOutput("ERROR: You reached numElectionTypes at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
			exit(3);
			break;
	}
	switch(this->getElectionType())
	{
		case hare:
		case schultze:
		case coombs:
		{
			int width = int(log10(this->ballots.size()) + 2) ;
			for(unsigned int round = 0; round < roundStates.size();round++)
			{
				tempStream << "Round " + to_string(round+1) + ":{";
				for(unsigned int index = 0; index < roundStates.at(round).size(); index++)
				{
					tempStream << right << setw(width) << (roundStates.at(round).at(index) != -1 ? to_string(roundStates.at(round).at(index)) : "X");
				}
				tempStream << left << "}\n";
			}
			break;
		}
		default:
			break;
	}
	tempStream << "\nThe winner is candidate " << static_cast<char>(winner+'A') << '\n';
	return tempStream.str();
//Line for printing out ballots here.
}

/* vector<vector<int>> generatePairwiseMatches(const vector<int> &newVector, const vector<int> &oldVector)
{
	vector<vector<int>> returnValue(newVector.size(),vector<int>(newVector.size(),0));
	for(unsigned candidateIndex = 0; candidateIndex < newVector.size(); candidateIndex++)
	{
		for(unsigned competitorIndex = 0; competitorIndex < newVector.size(); competitorIndex++)
		{
			// if(candidateIndex == competitorIndex)
				// continue;
			if(newVector.at(candidateIndex) > newVector.at(competitorIndex))
			{
				returnValue.at(candidateIndex).at(competitorIndex) += 1;
			}
			if(oldVector.at(candidateIndex) > oldVector.at(competitorIndex))
			{
				returnValue.at(candidateIndex).at(competitorIndex) -= 1;
			}
		}
	}
	return returnValue;
} */

/*
Returns true if the normalized ballots are changed in any way.
Asks entire electorate to give their ballots one at a time.
*/
// bool Election::generateBallots(const vector<Voter>&theElectorate)
// {
	// bool ballotsChanged = false;
	// vector<int> oldNormalizedBallot, newNormalizedBallot;
	// if(this->ballots.size() == 0)
	// {
		// ballotsChanged = true;
		// for(unsigned int index = 0; index < theElectorate.size(); index++)
		// {
			// this->ballots.push_back(Ballot(*this,theElectorate.at(index)));
			////this->ballots.at(index)=(Ballot(*this,theElectorate.at(index)));
		// }
	// }
	// else if(this->ballots.size() != 0)
	// {
		// debugOutput("Ballots generating\n",2);
		// for(unsigned int index = 0; index < theElectorate.size(); index++)
		// {
			// oldNormalizedBallot = this->ballots.at(index).normalizedBallot(this->getElectionType());
			////this->ballots.push_back(Ballot(*this,theElectorate.at(index)));
			// this->ballots.at(index)=(Ballot(*this,theElectorate.at(index)));
			// newNormalizedBallot = this->ballots.at(index).normalizedBallot(this->getElectionType());
			// if(oldNormalizedBallot != newNormalizedBallot)
			// {
				// ballotsChanged = true;
				// debugOutput("Voter " + to_string(index) + " changed vote from " + vectorString(oldNormalizedBallot) + " to " + vectorString(newNormalizedBallot) + '\n',16);
			// }
		// }
	// }
	// else
	// {
		// debugOutput("error in ballot count\n",1);
	// }
	// debugOutput("Ballots generated\n",2);
	// return ballotsChanged;
// }

/*
Typically called from runElection(). generateBallots runs a single round of the election,
asking each voter in the provided electorate what their ballot would be given the 
state of the current election.

Also calls shiftPairwise() to update the pairwiseMatrix inside the Election object.

If at any point ANY other function in the Election object updates the ballots object,
you MUST be careful to update the pairwiseMatrix appropriately.
*/

bool Election::generateBallots(const vector<Voter>&theElectorate)
{
	bool ballotsChanged = false;
	vector<int> newNormalizedBallot, oldNormalizedBallot;
	vector<vector<int>> pairwiseAdjustment = vector<vector<int>>(theElectorate.size(),vector<int>(theElectorate.size(),0));
	if(this->ballots.size() > theElectorate.size())
	{
		debugOutput("error in ballot count - currentBallotCount:" + to_string(this->ballots.size()) + " numCandidates:" + to_string(this->getNumCandidates()) + "\n",1);
		exit(1);
	}
	// if(this->ballots.size() != this->getNumCandidates())
	// {
		// ballotsChanged = true;
	// }
	debugOutput("Ballots generating for " + to_string(theElectorate.size()) + " voters.\n",2);
	for(unsigned int index = 0; index < theElectorate.size(); index++)
	{
		bool doIHaveOldBallots = false;
		oldNormalizedBallot = vector<int>(this->getNumCandidates(),0);
		if(this->ballots.size() > index)
		{
			doIHaveOldBallots = true;
			oldNormalizedBallot = this->ballots.at(index).normalizedBallot(this->getElectionType());
			this->ballots.at(index) = Ballot(*this,theElectorate.at(index));
		}
		else
		{
			this->ballots.push_back(Ballot(*this,theElectorate.at(index)));
		}
		
		newNormalizedBallot = this->ballots.at(index).normalizedBallot(this->getElectionType());
		if(oldNormalizedBallot != newNormalizedBallot)
		{
			/*
			Was going to be something like 
			{{1,0,0}		{{0,0,-1}
			{0,1,0}		+	{0,-1,0}		
			{1,1,0}}        {0,-1,0}}
			*/
			/*Will be true if generating ballots for the first time, 
			because oldNormalizedBallot is set to all zeros when initialized,
			and newNormalizedBallot should never be all zeros.*/
			ballotsChanged = true;
			debugOutput("Voter " + to_string(index) + " changed vote from " + vectorString(oldNormalizedBallot) + " to " + vectorString(newNormalizedBallot) + '\n',16);
			//https://stackoverflow.com/questions/44807347/using-stdaccumulate-on-a-two-dimensional-stdarray
			/*
			Lets go through the vectors in pairwiseAdjustment (sized to the same size as a pairwiseMatrix)
			and set the values to
			generatePairwiseMatches(newNormalizedBallot) - generatePairwiseMatches(oldNormalizedBallot)
			*/
			/*
			for_each(pairwiseAdjustment.begin(),pairwiseAdjustment.end(),
				[&oldNormalizedBallot,&newNormalizedBallot](vector<int> &candidateVector){
				vector<int> tempOld = generatePairwiseMatches(oldNormalizedBallot);
				vector<int> tempNew = generatePairwiseMatches(newNormalizedBallot);
				transform(tempNew.begin(),tempNew.end(),tempOld.begin(),candidateVector.begin(),minus<>{});
			});
			// :(
			*/
			//pairwiseAdjustment = generatePairwiseMatches(newNormalizedBallot,oldNormalizedBallot);
			/* transform(pairwiseAdjustment.begin(),pairwiseAdjustment.end(),this->pairwiseMatrix.begin(),this->pairwiseMatrix.begin(),
			[](const vector<int> &adjustmentVector,vector<int>&matrixVector){
				transform(matrixVector.begin(),matrixVector.end(),adjustmentVector.begin(),matrixVector.begin(),plus<>{});
			}); */
			//https://stackoverflow.com/questions/3752019/how-to-get-the-index-of-a-value-in-a-vector-using-for-each
			/* int transformIndex = 0;
			for_each(this->pairwiseMatrix.begin(),this->pairwiseMatrix.end(),
			[&pairwiseAdjustment,&transformIndex](vector<int>&matrixVector){
				transform(matrixVector.begin(),matrixVector.end(),(pairwiseAdjustment.begin()+transformIndex),matrixVector.begin(),plus<>{});
				transformIndex++;
			}
			); */
			this->shiftPairwise(newNormalizedBallot,oldNormalizedBallot,doIHaveOldBallots);
		}
		

	}
	debugOutput("Ballots generated\n",2);
	return ballotsChanged;
}

/*
Shifts the stored values inside the pairwiseMatrix inside the Election object.

Candidate A beats Candidate B? Candidate A gets two points. 

Candidate A beats Candidate B, but Candidate B USED TO beat Candidate A on the old ballot?
Candidate A gets two points, Candidate B loses two points it had from the old ballot.

Candidates tie? Each get a point. Candidates were tied? Candidates lose a point each.

Keeps track of a pairwise Matrix of who wins if you were to simply and only pair each
candidate off against each other, one-on-one.
*/

void Election::shiftPairwise(const vector<int>&newVector, const vector<int>&oldVector,bool oldBallotsPresent)
{
	for(unsigned candidateIndex = 0; candidateIndex < newVector.size(); candidateIndex++)
	{
		for(unsigned competitorIndex = 0; competitorIndex < newVector.size(); competitorIndex++)
		{
			if(candidateIndex == competitorIndex)
				continue;
			if(newVector.at(candidateIndex) > newVector.at(competitorIndex))
			{
				this->pairwiseMatrix.at(candidateIndex).at(competitorIndex) += 2;
			}
			if(newVector.at(candidateIndex) == newVector.at(competitorIndex))
			{
				this->pairwiseMatrix.at(candidateIndex).at(competitorIndex) += 1;
			}
			if(oldBallotsPresent)
			{
				if(oldVector.at(candidateIndex) > oldVector.at(competitorIndex))
				{
					this->pairwiseMatrix.at(candidateIndex).at(competitorIndex) -= 2;
				}
				if(oldVector.at(candidateIndex) == oldVector.at(competitorIndex))
				{
					this->pairwiseMatrix.at(candidateIndex).at(competitorIndex) -= 1;
				}
			}
		}
	}
}

vector<int> Election::currentPoll() const
{
	vector<int> sumOfVotes(this->getNumCandidates(),0);
	//https://cplusplus.com/reference/algorithm/transform/
	for(unsigned int ballotIndex = 0; ballotIndex < ballots.size(); ballotIndex++)
	{
		transform(sumOfVotes.begin(),sumOfVotes.end(),ballots.at(ballotIndex).normalizedBallot(getElectionType()).begin(),sumOfVotes.begin(),plus<int>());
	}
	return sumOfVotes;
}

vector<int> Election::currentPoll(electionType thisElectionType) const
{
	vector<int> sumOfVotes(this->getNumCandidates(),0);
	//https://cplusplus.com/reference/algorithm/transform/
	for(unsigned int ballotIndex = 0; ballotIndex < ballots.size(); ballotIndex++)
	{
		transform(sumOfVotes.begin(),sumOfVotes.end(),ballots.at(ballotIndex).normalizedBallot(thisElectionType).begin(),sumOfVotes.begin(),plus<int>());
	}
	debugOutput("currentPoll(electionType) is returning: " + vectorString(sumOfVotes) + "\n",128);
	return sumOfVotes;
}

vector<int> Election::currentPoll(pollMethod thisStrategy) const
{
	bool winnerObtained = false; //For pairwise code reuse. Switch statements will conditionally run elections?
	bool justVictories = false, justWinOrLose = false; //Booleans to control alternate pairwise options?
	vector<int> evaluationPoll(this->getNumCandidates(),0);
	switch(thisStrategy)
	{
		case strat_approval:
			evaluationPoll = this->currentPoll(approval);
			return evaluationPoll;
			break;
		case strat_plurality:
			evaluationPoll = this->currentPoll(plurality);
			return evaluationPoll;
			break;
		case strat_antiplurality:
			evaluationPoll = this->currentPoll(antiplurality);
			return evaluationPoll;
			break;
		case strat_bordaEval:
			evaluationPoll = this->currentPoll(borda);
			return evaluationPoll;
			break;
		case strat_allZero:
			evaluationPoll = vector<int>(this->getNumCandidates(),0);
			return evaluationPoll;
			break;
		default:
			break;
	}
	unsigned int currentWinner = this->getWinner();
	stringstream debugStream;
	debugStream << "Pairwise Evaluation - Current Winner: " << static_cast<char>(currentWinner+'A') << " (worth " << this->ballots.size() * 2/*  + 1  */<< ") ";
	/* if(thisStrategy == strat_match)
	{
		switch(this->getElectionType())
		{
			case approval:
				thisStrategy = strat_approval;
				break;
			case plurality:
				thisStrategy = strat_plurality;
				break;
			case antiplurality:
				thisStrategy = strat_antiplurality;
				break;
			case borda:
				thisStrategy = strat_bordaEval;
				break;
			case hare:
				//Defaulting to Elimination speed?
				thisStrategy = strat_hareEliminationSpeed;
				break;
			case numElectionTypes:
				debugOutput("ERROR: You reached numElectionTypes at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
				exit(3);
				break;
		}
	} */

	switch(thisStrategy)
	{
		case strat_approval: debugOutput("ERROR passed initial switch in " + string(__FILE__) + " at " + to_string(__LINE__),1); exit(5); break;
		case strat_plurality: debugOutput("ERROR passed initial switch in " + string(__FILE__) + " at " + to_string(__LINE__),1); exit(5); break;
		case strat_antiplurality: debugOutput("ERROR passed initial switch in " + string(__FILE__) + " at " + to_string(__LINE__),1); exit(5); break;
		case strat_bordaEval: debugOutput("ERROR passed initial switch in " + string(__FILE__) + " at " + to_string(__LINE__),1); exit(5); break;
		case strat_allZero: debugOutput("ERROR passed initial switch in " + string(__FILE__) + " at " + to_string(__LINE__),1); exit(5); break;
		
		case strat_winOrLosePairwise:
			justWinOrLose = true;
			//Intentionally fall through to strat_pairwise
		case strat_victoryPairwise://Intentionally BELOW strat_winOrLosePairwise
			justVictories = true;
			//Intentionally fall through to strat_pairwise
		/* 
		case strat_harePairwise:
		case strat_bordaPairwise:
		case strat_copelandPairwise:
		*/
		case strat_pairwise:
		{//Code block also intentionally used by strat_victoryPairwise and strat_winOrLosePairwise.
			evaluationPoll.at(currentWinner) = this->ballots.size() * 2;
			//debugStream << "Pairwise Evaluation - Current Winner: " << static_cast<char>(currentWinner+'A') << " (worth " << this->ballots.size() * 2/*  + 1  */<< ") ";
			for(unsigned int ballotIndex = 0; ballotIndex < this->ballots.size(); ballotIndex++)
			{//Go through each ballot and...
				for(unsigned int pairwiseIndex = 0;pairwiseIndex < evaluationPoll.size(); pairwiseIndex++)
				{//...look at how well the current winner of the election performed on THAT BALLOT against others on THAT BALLOT.
					if(pairwiseIndex == currentWinner)
						continue;
					if(this->ballots.at(ballotIndex).at(pairwiseIndex) > this->ballots.at(ballotIndex).at(currentWinner))
					{//If someone did better than the election's current winner, they get 2 points.
						evaluationPoll.at(pairwiseIndex) = evaluationPoll.at(pairwiseIndex) + 2;
					}
					else if (this->ballots.at(ballotIndex).at(pairwiseIndex) == this->ballots.at(ballotIndex).at(currentWinner)) // important if we ever have tied ratings in ballots
					{//Ties get 1 point. LeGrand's fix to my forgetting to cover this.
						evaluationPoll.at(pairwiseIndex) = evaluationPoll.at(pairwiseIndex) + 1;
					}
				}
			}
			//The following for() loop conditionally runs only when strat_winOrLosePairwise
			//or strat_victoryPairwise.
			for (unsigned int pairwiseIndex = 0;justVictories && (pairwiseIndex < evaluationPoll.size()); pairwiseIndex++)
			{//Ignoring ballots, go through each candidate in the current evaluation of the election.
				if (evaluationPoll.at(pairwiseIndex) <= this->ballots.size()) // or could use < instead of <=
				{//If they don't have a score that is at least half that of the winner's score...? Zero them out?
					evaluationPoll.at(pairwiseIndex) = 0;
				}
				else if (justWinOrLose && (evaluationPoll.at(pairwiseIndex) < evaluationPoll.at(currentWinner)))
				{//If they're at half-or-better, but not tied with the winner or doing better than them, put them at half?
					evaluationPoll.at(pairwiseIndex) = this->ballots.size();
				}
			}
			break;
		}
		case strat_hareEliminationSpeed:
		{
			//https://stackoverflow.com/questions/8833559/can-i-assign-in-2-variables-at-the-same-time-in-c
			tie(ignore,evaluationPoll,ignore) = this->runHare();
			break;
		}
		case strat_copeland:
		{
			tie(ignore,evaluationPoll) = this->runCopeland();
			break;
		}
		case strat_simpson:
		{
			tie(ignore,evaluationPoll) = this->runSimpson();
			break;
		}
		case strat_sentinelValue:
			debugOutput("ERROR: You reached strat_sentinelValue at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
			exit(3);
			break;
		// default:
			// return this->currentPoll();
			// break;
	}
	debugStream << vectorString(evaluationPoll) << "\n";
	debugOutput(debugStream,64);
	return evaluationPoll;
}

unsigned int Election::getWinner() const
{
	unsigned int theWinner = 0;
	int theMaxScore = -1;
	switch(this->getElectionType())
	{
		case approval:
		case plurality:
		case antiplurality:
		case borda:
		{
			vector<int> theCurrentPoll = this->currentPoll();
			for(unsigned int index = 0; index < theCurrentPoll.size(); index++)
			{
				if(this->currentPoll().at(index) > theMaxScore)
				{
					theWinner = index;
					theMaxScore = theCurrentPoll.at(index);
				}
			}
			break;
		}
		case hare:
			return get<0>(this->runHare());
			break;
		case copeland:
			return get<0>(this->runCopeland());
			break;
		case simpson:
			return get<0>(this->runSimpson());
			break;
		case schultze:
			return get<0>(this->runSchultze());
			break;
		case coombs:
			return get<0>(this->runCoombs());
			break;
		case numElectionTypes:
			debugOutput("ERROR: You reached numElectionTypes at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
			exit(3);
			break;
		// default:
			// break;
	}
	return theWinner;
}

/*
runHare() returns
winner
eliminationSpeed
round-by-round results
*/
tuple<unsigned int,vector<int>,vector<vector<int>>> Election::runHare() const
{
	unsigned int theWinner = 0;
	stringstream theStream;
	int theHighestIndex, theHighestValue;
	int theLowestIndex, theLowestValue;
	vector<bool> validCandidates(this->getNumCandidates(),true);
	vector<int> highestPlaceCounts(this->getNumCandidates(),0);
	//vector<int> eliminationSpeed(this->getNumCandidates(),-1);
	vector<int> eliminationSpeed(this->getNumCandidates(),this->getNumCandidates()-1);
	vector<vector<int>> roundStates;
	for(unsigned int round = 0; round < this->getNumCandidates()-1; round++)
	{//We're going to run through this numCandidates-1 times, and drop the lowest each time.
		fill(highestPlaceCounts.begin(),highestPlaceCounts.end(),0);
		for(vector<Ballot>::const_iterator ballotIterator = this->ballots.begin(); ballotIterator < this->ballots.end(); ballotIterator++)
		{
			theHighestValue = INT_MIN;
			theHighestIndex = -1;
			for(unsigned int candidateInBallot = 0; candidateInBallot < this->getNumCandidates(); candidateInBallot++)
			{
				if(validCandidates.at(candidateInBallot) && ballotIterator->at(candidateInBallot) > theHighestValue)
				{
					theHighestIndex = candidateInBallot;
					theHighestValue = ballotIterator->at(candidateInBallot);
				}
			}
			highestPlaceCounts.at(theHighestIndex)++;
		}
		theLowestValue = INT_MAX;
		theLowestIndex = -1;
		// for(vector<int>::const_reverse_iterator highestCountIterator = highestPlaceCounts.crbegin(); highestCountIterator < highestPlaceCounts.crend(); highestCountIterator++)
		// {//Do this in reverse and only check for <, not <=, so we drop tied candidates in Z-A order.
			// if(*highestCountIterator < theHighestValue && )
			// {
				// theHighestIndex = highestPlaceCounts.size() - (highestPlaceCounts.crend() - highestCountIterator);
				// theHighestValue = *highestCountIterator;
			// }
		// }
		debugOutput("\nLooking for lowest ranked candidate: ",32);
		for(unsigned int index = highestPlaceCounts.size()-1;/* index >= 0 &&  */index < highestPlaceCounts.size(); index--)
		//for(unsigned int index = 0;index < highestPlaceCounts.size(); index++)
		{
			// if(index > this->getNumCandidates())
			// {
				// debugOutput("ERROR: This should not have happened! Somehow we're outside the range of candidates!",32);
				// continue;
			// }
			debugOutput("Checking: " + to_string(index),32);
			if(validCandidates.at(index) && highestPlaceCounts.at(index) < theLowestValue)
			{
				theLowestIndex = index;
				theLowestValue = highestPlaceCounts.at(index);
			}
			debugOutput("\n",32);
		}
		roundStates.push_back(highestPlaceCounts);
		transform(roundStates.back().begin(),roundStates.back().end(),validCandidates.begin(),roundStates.back().begin(),[](int theValue, bool theBool){return (theBool ? theValue : -1);});
		// theStream << "Round " + to_string(round+1) + ":{";
		// for(unsigned int index = 0; index < highestPlaceCounts.size(); index++)
		// {
			// theStream << right << setw(3) << (validCandidates.at(index) ? to_string(highestPlaceCounts.at(index)) : "X");
		// }
		// theStream << left << "}\n";
		//debugOutput("\nLowest candidate was " + to_string(theLowestIndex) + " out of \n" + vectorString(validCandidates) + "\n" + vectorString(highestPlaceCounts) + "\n",32);
		// theStream << left << "\nLowest candidate was " << static_cast<char>(theLowestIndex+'A') << " out of \n" << vectorString(validCandidates) << "\n" << vectorString(highestPlaceCounts) << "\n";
		validCandidates.at(theLowestIndex) = false;
		//eliminationSpeed.at(theLowestIndex) = theLowestValue;
		transform(eliminationSpeed.begin(),eliminationSpeed.end(),validCandidates.begin(),eliminationSpeed.begin(),[](int theValue, bool theBool){return theValue - (1 * !theBool);});
	}
	debugOutput("Hare results:\n" + vectorString(validCandidates) + "\n" + vectorString(highestPlaceCounts) + "\n",32);
	//theHighestIndex = -1;
	theHighestValue = INT_MIN;
	for(unsigned int index = 0;index < highestPlaceCounts.size(); index++)
	{
		if(validCandidates.at(index) && highestPlaceCounts.at(index) > theHighestValue)
		{
			theHighestValue = highestPlaceCounts.at(index);
			//theHighestIndex = index;
			theWinner = index;
		}
	}
	if(PRINT_HARE_RESULTS) cout << theStream.str();
	return make_tuple(theWinner,eliminationSpeed,roundStates);
}

/* 
Returns winner and Copeland scores. 
Ties in copeland scores are probably broken alphabetically.
 */
tuple<unsigned int,vector<int>> Election::runCopeland() const
{
	vector<int> pairwiseScores(this->getNumCandidates(),0);
	for(unsigned candidateA = 0; candidateA < this->getNumCandidates(); candidateA++)
	{
		for(unsigned candidateB = 0; candidateB < this->getNumCandidates(); candidateB++)
		{
			if(candidateA == candidateB)
				continue;
			int abScore = this->pairwiseMatrix.at(candidateA).at(candidateB);
			int baScore = this->pairwiseMatrix.at(candidateB).at(candidateA);
			if(abScore > baScore)
			{
				pairwiseScores.at(candidateA) += 2;//4? Then divide everything by 2?
			}
			else if(abScore == baScore)
			{
				pairwiseScores.at(candidateA) += 1;
			}
		}
	}
	unsigned winner = distance(pairwiseScores.begin(),max_element(pairwiseScores.begin(),pairwiseScores.end()));
	return make_tuple(winner,pairwiseScores);
}



tuple<unsigned int,vector<int>> Election::runSimpson() const
{
	vector<int> pairwiseScores(this->getNumCandidates(),0);
	for(unsigned candidateA = 0; candidateA < this->getNumCandidates(); candidateA++)
	{
		// pairwiseScores.at(candidateA) = *min_element(pairwiseMatrix.at(candidateA).begin(),pairwiseMatrix.at(candidateA).end());
		int smallestValue = INT_MAX;
		for(unsigned candidateB = 0; candidateB < this->getNumCandidates(); candidateB++)
		{
			if(candidateA == candidateB)
				continue;
			if(pairwiseMatrix.at(candidateA).at(candidateB) < smallestValue)
			{
				smallestValue = pairwiseMatrix.at(candidateA).at(candidateB);
			}
		}
		pairwiseScores.at(candidateA) = smallestValue;
	}
	unsigned winner = distance(pairwiseScores.begin(),max_element(pairwiseScores.begin(),pairwiseScores.end()));
	return make_tuple(winner,pairwiseScores);
}

/* Uhhh. What? */

tuple<unsigned int,vector<vector<int>>> Election::runSchultze() const
{
	vector<vector<int>> matrixScores(this->getNumCandidates(),vector<int>(this->getNumCandidates(),0));
	for(unsigned candidateA = 0; candidateA < this->getNumCandidates(); candidateA++)
	{
		for(unsigned candidateB = 0; candidateB < this->getNumCandidates(); candidateB++)
		{
			if(candidateA == candidateB)
				continue;
			int abScore = this->pairwiseMatrix.at(candidateA).at(candidateB);
			int baScore = this->pairwiseMatrix.at(candidateB).at(candidateA);
			if(abScore > baScore)
			{
				matrixScores.at(candidateA).at(candidateB) = abScore;
			}
		}
	}
	
	for(unsigned candidateA = 0; candidateA < this->getNumCandidates(); candidateA++)
	{
		for(unsigned candidateB = 0; candidateB < this->getNumCandidates(); candidateB++)
		{
			if(candidateA == candidateB)
				continue;
			//https://en.wikipedia.org/wiki/Schulze_method ?!?!?
			for(unsigned candidateK = 0; candidateK < this->getNumCandidates(); candidateK++)
			{
				if(candidateA == candidateK)
					continue;
				if(candidateB == candidateK)
					continue;
				matrixScores.at(candidateB).at(candidateK) = 
					max(matrixScores.at(candidateB).at(candidateK), 
						min(matrixScores.at(candidateB).at(candidateA),matrixScores.at(candidateA).at(candidateK)));
			}
		}
	}
	vector<int> matrixLosses(this->getNumCandidates(),0);
	for(unsigned index = 0;index < this->getNumCandidates();index++)
	{
		matrixLosses.at(index) = count(matrixScores.at(index).begin(),matrixScores.at(index).end(),0);
	}
	unsigned winner = distance(matrixLosses.begin(),min_element(matrixLosses.begin(),matrixLosses.end()));
	return make_tuple(winner,matrixScores);
}



tuple<unsigned int,vector<int>,vector<vector<int>>> Election::runCoombs() const
{
	vector<int> lastRanks(this->getNumCandidates(),0),ranking(this->getNumCandidates(),this->getNumCandidates()*2-2);
	vector<vector<int>> rankRounds;
	vector<bool> validCandidate(this->getNumCandidates(),true);
	int smallestValue;
	unsigned lossSequencer = 0;
	for(unsigned round = 0; round < this->getNumCandidates()-1;round++)
	{
		fill(lastRanks.begin(),lastRanks.end(),0);
		for(unsigned ballotIndex = 0; ballotIndex < this->ballots.size();ballotIndex++)
		{
			smallestValue = INT_MAX;
			for(unsigned smallIndex = 0; smallIndex < this->getNumCandidates(); smallIndex++)
			{
				if(validCandidate.at(smallIndex))// && this->ballots.at(ballotIndex).at(smallIndex) < smallestValue
					smallestValue = min(this->ballots.at(ballotIndex).at(smallIndex),smallestValue);
			}
			for(unsigned candidateIndex = 0;candidateIndex < this->getNumCandidates();candidateIndex++)
			{
				if(ballots.at(ballotIndex).at(candidateIndex) == smallestValue)
					lastRanks.at(candidateIndex) += 1;//No double borda? What about three way ties?
			}
		}
		unsigned worstCandidate = distance(lastRanks.begin(),max_element(lastRanks.begin(),lastRanks.end()));
		rankRounds.push_back(lastRanks);
		validCandidate.at(worstCandidate) = false;//Ties?!
		ranking.at(worstCandidate) = lossSequencer;
		lossSequencer += 2;
	}
	unsigned winner = distance(validCandidate.begin(),max_element(validCandidate.begin(),validCandidate.end()));
	return make_tuple(winner,ranking,rankRounds);
}

/*
Runs an election according to the provided number of rounds until ballots stabilize
and do not change between two rounds.

Returns the round it finished on, and whether or not the election stabilized (true = stable).
*/

pair<int,bool> Election::runElection(int numberOfRounds,const vector<Voter>&theElectorate)
{//Returns the number of rounds it ran.
	bool ballotsChanged = true;
	int round;
	for(round = 0; round < numberOfRounds && ballotsChanged; round++)
	{
		debugOutput("Round " + to_string(round+1) + ":\n",16);
		ballotsChanged=this->generateBallots(theElectorate);
	}
	return make_pair(round,!ballotsChanged);
}

Ballot::Ballot()
{
	this->ratings = vector<int>(0,0);
}

Ballot::Ballot(const Election &theElection, const Voter &theVoter)
{
	//cout << "Generating " << theVoter.getVoterID() << '\n';
	/* int maxIncrementer = MAX_RATING;
	int minIncrementer = MIN_RATING;
	bool tie = false;
	int leadingVoteCount = 0;
	int leadingVoteIndex = -1;
	
	for(int index = 0; index < theElection.currentPoll().size(); index++)
	{
		if(theElection.currentPoll().at(index) > leadingVoteCount)
		{
			tie = false;
			leadingVoteCount = theElection.currentPoll().at(index);
			leadingVoteIndex = index;
		}
		else if(theElection.currentPoll().at(index) == leadingVoteCount)
		{
			tie = true;
		}
	} */
	vector<int> tempRatings = theVoter.generateBallot(theElection);
	this->ratings = vector<int>(theElection.getNumCandidates(),0);
	if(tempRatings.size() == this->ratings.size())
	{
		this->ratings = tempRatings;
	}
	else
	{
		debugOutput("ERROR! Invalid Ballot Size Returned By Voter# " + to_string(theVoter.getVoterID()) + "\n",1);
		//Error of some kind? Silently stay 0s?
	}
	//vector<int> voterRanks = theVoter.getRanks();
	//cout << "Index: " << find(voterRanks.begin(),voterRanks.end(),theElection.getNumCandidates()-1)-voterRanks.begin() << '\n';
	//this->ratings.at(find(voterRanks.begin(),voterRanks.end(),theElection.getNumCandidates()-1)-voterRanks.begin()) = 1;
	//cout << "Generated " << theVoter.getVoterID() << '\n';
}

vector<int> Ballot::normalizedBallot(const electionType &theElectionType) const
{
	unsigned int sum = accumulate(this->ratings.begin(),this->ratings.end(),0);
	//int average = (MAX_RATING + MIN_RATING) / 2;
	int foundValue = MIN_RATING - 1;
	int foundIndex = -1;
	/* if(ratings.size() > 0)
	{
		average = sum / this->ratings.size();
	} 
	else
	{
		cout << "Warning! Attempt at averaging ballot revealed size 0 ballot!\n";
	}*/
	vector<int> theNormalizedBallot(this->ratings.size(),0);
	switch(theElectionType)
	{
		case approval:
			debugOutput("Hit approval branch in normalizedBallot!\n",128);
			for(unsigned int index = 0; index < theNormalizedBallot.size(); index++)
			{
				if((this->ratings.at(index) * this->ratings.size()) > sum)//Weird averaging!
				{
					theNormalizedBallot.at(index) = 1;
				}
			}
			break;
		case plurality:
			for(unsigned int index = 0; index < theNormalizedBallot.size(); index++)
			{
				if(this->ratings.at(index) > foundValue)
				{
					foundIndex = index;
					foundValue = this->ratings.at(index);
				}
			}
			if(foundIndex >= 0 && foundIndex < static_cast<int>(theNormalizedBallot.size()))
			{
				theNormalizedBallot.at(foundIndex) = 1;
			}
			break;
		case antiplurality:
			foundValue = MAX_RATING + 1;
			for(unsigned int index = 0; index < theNormalizedBallot.size(); index++)
			{
				theNormalizedBallot.at(index) = 1;
				if(this->ratings.at(index) < foundValue)
				{
					foundIndex = index;
					foundValue = this->ratings.at(index);
				}
			}
			if(foundIndex >= 0 && foundIndex < static_cast<int>(theNormalizedBallot.size()))
			{
				theNormalizedBallot.at(foundIndex) = 0;
			}
			break;
		case borda:
		case hare:
		case copeland:
		case simpson:
		case schultze:
		case coombs:
			/*
			
            ratings =   [12, 76, 98, 76, 12, 76]
            
            sorted =    [12, 12, 76, 76, 76, 98]
            
            count =     [ 2,  2,  3,  3,  3,  1]
			
			map =       [12:1, 76: 
			
			*/
			//unordered_map<int,int> bordaValues;
			int countSum;
			for(unsigned int index = 0; index < this->ratings.size(); index++)
			{
				int thisValue = this->ratings.at(index);
				countSum = 0;
				//https://en.cppreference.com/w/cpp/algorithm/count
				//unordered_map.insert({this->ratings.at(index),count(this->ratings.begin(),this->ratings.end(),this->ratings.at(index))});
				//https://en.cppreference.com/w/cpp/utility/functional/less ?? Nahh.
				//unordered_map.at(this->ratings.at(index)) += count_if(this->ratings.begin(),this->ratings.end(),[](int checkValue){return checkValue < this->ratings.at(index);});
				//https://stackoverflow.com/questions/57151983/counting-elements-greater-than-a-number-in-vector
				countSum = count_if(this->ratings.begin(),this->ratings.end(),
					[thisValue](int checkValue){return checkValue < thisValue;});
				countSum += count_if(this->ratings.begin(),this->ratings.end(),
					[thisValue](int checkValue){return checkValue <= thisValue;})-1;
				theNormalizedBallot.at(index) = countSum;
			}
			//https://en.cppreference.com/w/cpp/container/unordered_map/insert
			
			/* vector<int> sortedRatings = this->ratings;
			//https://en.cppreference.com/w/cpp/algorithm/sort
			sort(sortedRatings.begin(),sortedRatings.end());
			vector<int> ratingsCount(sortedRatings.size(),0);
			for(int index = 0; index < ratingsCount.size(); index++)
			{
				ratingsCount.at(index) = count(sortedRatings.begin(),sortedRatings.end(),sortedRatings.at(index));
			}
			for(int index = 0; index < theNormalizedBallot.size(); index++)
			{
				
			} */
			break;
		case numElectionTypes:
			debugOutput("ERROR: You reached numElectionTypes at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
			exit(3);
			break;
	}
	debugOutput("normalizedBallot is returning: " + vectorString(theNormalizedBallot) + "\n",128);
	return theNormalizedBallot;
}

Voter::Voter(vector<int> inputPreferences, int VoterID, const Election &theElection, pollMethod theStrengthStrategy, pairingMethod thePairingStrategy, ballotBuildMethod theBallotBuildStrategy)
{
	if(inputPreferences.size() != theElection.getNumCandidates())
	{
		debugOutput("ERROR: Voter passed incorrect number of voter preferences (" + to_string(inputPreferences.size()) + "), aborting.",1);
		exit(2);
	}
	this->preferences = inputPreferences;
	//this->ranks = sorted_indexes(this->preferences,true);
	this->strengthStrategy = theStrengthStrategy;
	this->pairingStrategy = thePairingStrategy;
	this->ballotBuildStrategy = theBallotBuildStrategy;
	this->voterIDNumber = VoterID;
}
Voter::Voter(int VoterID, const Election &theElection, pollMethod theStrengthStrategy, pairingMethod thePairingStrategy, ballotBuildMethod theBallotBuildStrategy)
{
	int aRandom = theDistribution(randomEngine);
	this->preferences = vector<int>(theElection.getNumCandidates(),-1);//Trying to do theDistribution(randomEngine) here was doing five of the same random value.
	for(unsigned int index = 0; index < this->preferences.size(); index++)
	{
		preferences.at(index) = theDistribution(randomEngine);
		if(FORBID_PREFERENCE_TIES && (theElection.getNumCandidates() <= (MAX_RATING - MIN_RATING + 1)))
		{//Go through the freshly generated preferences and if any value is duplicated, regenerate that position's value.
			//cout << preferences.at(index) << " Count: " << count(this->preferences.begin(),this->preferences.end(),this->preferences.at(index)) << '\n';
			if(count(this->preferences.begin(),this->preferences.end(),this->preferences.at(index)) > 1)
			{
				do
				{
					aRandom = theDistribution(randomEngine);
				}while(count(this->preferences.begin(),this->preferences.end(),aRandom) > 0);
				this->preferences.at(index) = aRandom;
				//Should ensure no ties?
			}
		}
	}
	// vector<int> temp(this->preferences);
	// sort(temp.begin(),temp.end());
	// for(unsigned int index = 0; index < temp.size();index++)
	// {
		//cout << temp.at(index) << ",";
		// ranks.push_back(find(temp.begin(),temp.end(),this->preferences.at(index)) - temp.begin());
	// }
	// this->ranks = sorted_indexes(this->preferences,true);
	//cout << '\n';
	this->strengthStrategy = theStrengthStrategy;
	this->pairingStrategy = thePairingStrategy;
	this->ballotBuildStrategy = theBallotBuildStrategy;
	this->voterIDNumber = VoterID;
}

string Voter::toString() const
{
	//string returnValue;
	stringstream tempStream;
	tempStream << "Prefs: {" << right;
	//returnValue += "TruePreferences: {";
	for(unsigned int index = 0; index < preferences.size();index ++)
	{
		tempStream << setw(4) << preferences.at(index);
		//returnValue += to_string(preferences.at(index));
		if(index < preferences.size()-1)
		{
			tempStream << ", ";
			//returnValue += ", ";
		}
	}
	tempStream << left << "}";
	/* 
	tempStream << left << "} Ranked: {" << right;
	//returnValue += "} TruePreferences Ranked: {";
	for(unsigned int index = 0; index < ranks.size();index ++)
	{
		tempStream << setw(3) << ranks.at(index);
		//returnValue += to_string(ranks.at(index));
		if(index < ranks.size()-1)
		{
			tempStream << ", ";
			//returnValue += ", ";
		}
	}
	tempStream << left << "}";
	 */
	//returnValue += "}";
	tempStream << "Strat: " << this->printStrat();
	return tempStream.str();//returnValue;
}

vector<tuple<int,int,int>> Voter::getPairings(const vector<int> &currentPoll) const
{
	const int exceedingValue = 1000;
/* 	for(int index = 0; index < currentPoll.size(); index++)
	{
		cout << "cP" << index << " " << currentPoll.at(index) << " \n";
	}
	cout << "GettingPairings\n"; */
	vector<int> doubleBordaPoll(currentPoll.size());
	int doubleBordaSum;
	int thisValue;
	for(unsigned int index = 0; index < doubleBordaPoll.size(); index++)
	{
		doubleBordaSum = 0;
		thisValue = currentPoll.at(index);
		doubleBordaSum = count_if(currentPoll.begin(),currentPoll.end(),
			[thisValue](int checkValue){return checkValue < thisValue;});
		doubleBordaSum += count_if(currentPoll.begin(),currentPoll.end(),
			[thisValue](int checkValue){return checkValue <= thisValue;})-1;
		doubleBordaPoll.at(index) = doubleBordaSum;
	}
/* 	for(int index = 0; index < doubleBordaPoll.size(); index++)
	{
		cout << "DB" << index << " " << doubleBordaPoll.at(index) << " \n";
	} */
	//https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
	//vector<int> sortedCandidateIndexes = sorted_indexes(doubleBordaPoll);
	//So now I know the order of the candidates from highest to lowest.
 	// for(int index = 0; index < sortedCandidateIndexes.size(); index++)
	// {
		// cout << "s_c" << doubleBordaPoll.at(index) << " " << sortedCandidateIndexes.at(index) << " \n";
	// } 
 	// cout << '\n';
	vector<tuple<int,int,int>> thePairings;
	switch(this->pairingStrategy)
	{
		case prioritizeLeader:
			for(unsigned int frontPair = 0; frontPair < currentPoll.size()-1; frontPair++)
			{
				for(unsigned int backPair = frontPair+1; backPair < currentPoll.size(); backPair++)
				{
					thePairings.push_back(make_tuple(frontPair,backPair,
						max(doubleBordaPoll.at(frontPair),doubleBordaPoll.at(backPair))*exceedingValue+min(doubleBordaPoll.at(frontPair),doubleBordaPoll.at(backPair))
					));
				}
			}
			break;
		case prioritizeRunnerUp:
			for(unsigned int backPair = 1; backPair < currentPoll.size(); backPair++)
			{
				for(unsigned int frontPair = 0; frontPair < currentPoll.size()-1 && frontPair < backPair; frontPair++)
				{
					thePairings.push_back(make_tuple(frontPair,backPair,
						min(doubleBordaPoll.at(frontPair),doubleBordaPoll.at(backPair))*exceedingValue+max(doubleBordaPoll.at(frontPair),doubleBordaPoll.at(backPair))
					));
				}
			}
			break;
		case neighbors:
			// for(unsigned int gap = 1;gap < currentPoll.size();gap++)
			// {
				// for(unsigned int frontPair = 0, backPair = frontPair + gap;backPair < currentPoll.size();frontPair++, backPair++)
				// {
			for(unsigned int frontPair = 0; frontPair < currentPoll.size()-1; frontPair++)
			{
				for(unsigned int backPair = frontPair+1; backPair < currentPoll.size(); backPair++)
				{
					thePairings.push_back(make_tuple(frontPair,backPair,
						((currentPoll.size() * 2) - abs(doubleBordaPoll.at(backPair) - doubleBordaPoll.at(frontPair))) * exceedingValue + max(doubleBordaPoll.at(frontPair),doubleBordaPoll.at(backPair))
					));
				}
			}
			break;
		case pair_sentinelValue:
			debugOutput("ERROR: You reached pair_sentinelValue at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
			exit(3);
			break;
	}
	//https://stackoverflow.com/questions/23030267/custom-sorting-a-vector-of-tuples
	stable_sort(thePairings.begin(),thePairings.end(),[](const tuple<int, int, int>&firstVal, const tuple<int, int, int>&secondVal){return get<2>(firstVal) > get<2>(secondVal);});
	// cout << "Here is what getPairings is returning:\n";
	// for(unsigned int index = 0; index < thePairings.size(); index++)
	// {
		// cout << get<0>(thePairings.at(index)) << get<1>(thePairings.at(index)) << ":" << get<2>(thePairings.at(index)) << '\n';
	// }
	//cout << "GotPairings\n";
	return thePairings;
}

/*
Asks the voter: Given the state of this election, what is the ballot you would
cast right now?
*/
vector<int> Voter::generateBallot(const Election&thisElection) const
{
	vector<int> currentPoll = thisElection.currentPoll(this->getStrengthEval());
	stringstream debugStream;

	int maxCounter = MAX_RATING+1;
	int minCounter = MIN_RATING-1;

	vector<tuple<int,int,int>> thePairings = this->getPairings(currentPoll);
	//map<int,vector<int>> theGroupings;
	map<int,vector<pair<int,int>>> theGroupings;
	/*
	A map where the keys are the score, and the vector 
	contains pairs that are the pairs of candidates.
	*/
	int pairScore;
	for(unsigned int index = 0; index < thePairings.size(); index++)
	{
		pairScore = get<2>(thePairings.at(index));
		//vector<int> toInsert = {get<0>(thePairings.at(index)),get<1>(thePairings.at(index))};
		pair<int,int> toInsert{get<0>(thePairings.at(index)),get<1>(thePairings.at(index))};
		/*
		Will either successfully create a fresh new entry OR will fail.
		In either case, a pair containing an iterator to the proper location
		in the map will be returned, along with a bool describing success
		or failure. If failure, lets tack on the pair to the existing iterator.
		
		The alternative is find()ing whether or not the element exists and 
		deciding how to operate from there. 
		*/
		//pair<map<int,vector<int>>::iterator,bool> insertionTest = theGroupings.insert({pairScore,toInsert});
		pair<map<int,vector<pair<int,int>>>::iterator,bool> insertionTest = theGroupings.insert({pairScore,vector<pair<int,int>>(1,toInsert)});
		//Holy shit, I nailed the map<int,vector<int>>::iterator on my first try. I haven't used iterators in a year+?
		if(!insertionTest.second)
		{
			//https://stackoverflow.com/questions/4527686/how-to-update-stdmap-after-using-the-find-method
			//https://stackoverflow.com/questions/2551775/appending-a-vector-to-a-vector
			//insertionTest.first->second.insert(insertionTest.first->second.end(),toInsert.begin(),toInsert.end());
			insertionTest.first->second.insert(insertionTest.first->second.end(),toInsert);
		}
	}
	//https://en.cppreference.com/w/cpp/container/map
	if((DEBUG_MODE & 4) == 4)
	{
		debugOutput("Here's what's in theGroupings:\n",4);
		for (const auto& element : theGroupings)
		{
			debugOutput(to_string(element.first) + ":",4);
			for(unsigned int index2 = 0; index2 < element.second.size(); index2++)
			{
				debugOutput(string(1,static_cast<char>(element.second.at(index2).first+'A')) + "," + string(1,static_cast<char>(element.second.at(index2).second+'A')) + " ",4);
			}
			debugOutput("\n",4);
		}
	}
	debugOutput("- Voter " + to_string(this->getVoterID()) + " generating ballot -\n",4);
	vector<int> buildABallot(currentPoll.size(),-1);

	unsigned int groupingSum;
	unsigned int candidateIndex;
	
	int groupingSize;
	bool buildFromFront = this->getBuildMethod() == bothEnds || this->getBuildMethod() == front;
	bool buildFromBack = this->getBuildMethod() == bothEnds || this->getBuildMethod() == back;
	
	int firstCandidate, secondCandidate;
	vector<int> sortedCandidateIndexes = sorted_indexes(this->preferences);
	vector<int> reversedSortedCandidateIndexes = sortedCandidateIndexes;
	reverse(reversedSortedCandidateIndexes.begin(),reversedSortedCandidateIndexes.end());
	vector<int> candidatesToConsider;
	vector<int> approvalList, disapprovalList;
	
	for (map<int, vector<pair<int,int>>>::const_reverse_iterator mapIterator = theGroupings.crbegin(); mapIterator != theGroupings.crend() && count(buildABallot.begin(),buildABallot.end(),-1) > 0; mapIterator++)
	{
		approvalList.clear();
		disapprovalList.clear();
		debugOutput("------\nGrouping " + to_string(mapIterator->first) + "\n",4);
		groupingSum = 0;
		groupingSize = mapIterator->second.size();
		if(groupingSize <= 0)
		{
			debugOutput("\nERROR: AAAAH! How is this grouping vector EMPTY?! " + to_string(mapIterator->first),0);
			continue;
		}
		for(const int&candidateWeAreConsideringByPreference : sortedCandidateIndexes)
		{
			if(buildABallot.at(candidateWeAreConsideringByPreference) != -1)
			{
				debugOutput("Candidate " + string(1,static_cast<char>(sortedCandidateIndexes.at(candidateWeAreConsideringByPreference) + 'A')) + " already has " + to_string(buildABallot.at(candidateWeAreConsideringByPreference)) + ", skipping.\n",4);
				continue;
			}
			debugOutput("Considering Candidate " + string(1,static_cast<char>(candidateWeAreConsideringByPreference+'A')) + ":\n",4);
			groupingSum = 0;
			candidatesToConsider.clear();
			for(int pairedIndex = 0; pairedIndex < groupingSize; pairedIndex++)
			{
				firstCandidate = mapIterator->second.at(pairedIndex).first;
				secondCandidate = mapIterator->second.at(pairedIndex).second;
				if(firstCandidate == candidateWeAreConsideringByPreference || secondCandidate == candidateWeAreConsideringByPreference)
				{
					candidatesToConsider.push_back(firstCandidate);
					candidatesToConsider.push_back(secondCandidate);
				}
			}
			for(unsigned int index = 0; index < candidatesToConsider.size(); index++)
			{
				candidateIndex = candidatesToConsider.at(index);
				groupingSum += this->preferences.at(candidateIndex);
				debugOutput("Adding candidate " + string(1,static_cast<char>(candidatesToConsider.at(index)+'A')) + ":" + to_string(this->preferences.at(candidateIndex)) + "\n",4);
			}
			if(buildABallot.at(candidateWeAreConsideringByPreference) == -1)
			{
				unsigned thisCandidateValue = this->preferences.at(candidateWeAreConsideringByPreference) * candidatesToConsider.size();
				debugOutput("Sum:" + to_string(groupingSum) + "\n",4);
				if(buildFromFront && (thisCandidateValue > groupingSum))//More weird averaging!
				{
					approvalList.push_back(candidateWeAreConsideringByPreference);
					debugOutput("Approve because candidate " + string(1,static_cast<char>(candidateWeAreConsideringByPreference+'A')) + 
						" is 'worth' " + to_string(this->preferences.at(candidateWeAreConsideringByPreference) * candidatesToConsider.size()) + "\n",4);
				}
				else if(buildFromBack && (thisCandidateValue < groupingSum))
				{
					disapprovalList.push_back(candidateWeAreConsideringByPreference);
					debugOutput("Disapprove: because candidate " + string(1,static_cast<char>(candidateWeAreConsideringByPreference+'A')) + 
						" is 'worth' " + to_string(this->preferences.at(candidateWeAreConsideringByPreference) * candidatesToConsider.size()) + "\n",4);
				}
				else
				{
					debugOutput("(" + to_string(groupingSum) + ")Tied, skipping\n",4);
				}
			}
		}
		int oldPreference = -1;
		for(const int&candidateWeAreConsideringByPreference : sortedCandidateIndexes)
		{
			int currentPreference = this->preferences.at(candidateWeAreConsideringByPreference);
			if(find(approvalList.begin(),approvalList.end(),candidateWeAreConsideringByPreference) != approvalList.end())
			{
				buildABallot.at(candidateWeAreConsideringByPreference) = (oldPreference != currentPreference ? --maxCounter : maxCounter);
				oldPreference = currentPreference;
			}
		}
		oldPreference = -1;
		for(const int&candidateWeAreConsideringByPreference : reversedSortedCandidateIndexes)
		{
			int currentPreference = this->preferences.at(candidateWeAreConsideringByPreference);
			if(find(disapprovalList.begin(),disapprovalList.end(),candidateWeAreConsideringByPreference) != disapprovalList.end())
			{
				buildABallot.at(candidateWeAreConsideringByPreference) = (oldPreference != currentPreference ? ++minCounter : minCounter);
				oldPreference = currentPreference;
			}
		}
	}
	for(const int&candidateWeAreConsideringByPreference : sortedCandidateIndexes)
	{//Anything that didn't get assigned a value now finally gets assigned a default.
		if(buildABallot.at(candidateWeAreConsideringByPreference) >= 0)
		{
			//Intentionally blank, because this candidate got assigned a value.
		}
		else
		{
			switch(this->getBuildMethod())
			{
				case bothEnds:
					buildABallot.at(candidateWeAreConsideringByPreference) = (MAX_RATING + MIN_RATING) / 2;
					break;
				case front:
					//buildABallot.at(candidateWeAreConsideringByPreference) = minCounter++;
					break;
				case back:
					buildABallot.at(candidateWeAreConsideringByPreference) = maxCounter--;
					break;
				case build_sentinelValue:
					debugOutput("ERROR: You reached build_sentinelValue at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
					exit(4);
					break;
				// default:
					// debugOutput("\nERROR: Reached unexpected build method option for leftover candidate " + to_string(index) + ", " + to_string(this->getBuildMethod()) + "\n",1);
					// break;
			}
		}
	}
	for(const int&candidateWeAreConsideringByPreference : reversedSortedCandidateIndexes)
	{//Anything that didn't get assigned a value now finally gets assigned a default.
		if(buildABallot.at(candidateWeAreConsideringByPreference) >= 0)
		{
			//Intentionally blank, because this candidate got assigned a value.
		}
		else
		{
			switch(this->getBuildMethod())
			{
				case bothEnds:
					buildABallot.at(candidateWeAreConsideringByPreference) = (MAX_RATING + MIN_RATING) / 2;
					break;
				case front:
					buildABallot.at(candidateWeAreConsideringByPreference) = minCounter++;
					break;
				case back:
					//buildABallot.at(candidateWeAreConsideringByPreference) = maxCounter--;
					break;
				case build_sentinelValue:
					debugOutput("ERROR: You reached build_sentinelValue at " + to_string(__LINE__) + " in vote.cpp, aborting.\n",1);
					exit(4);
					break;
				// default:
					// debugOutput("\nERROR: Reached unexpected build method option for leftover candidate " + to_string(index) + ", " + to_string(this->getBuildMethod()) + "\n",1);
					// break;
			}
		}
	}
	if((DEBUG_MODE & 8) == 8)
	{
		debugOutput("Vote was: ",8);
		for(unsigned int index = 0; index < currentPoll.size(); index++)
		{
			debugOutput(to_string(currentPoll.at(index)) + " ");
		}
		debugOutput("\nPrefs(" + to_string(this->getVoterID()) + ") were: ",8);
		for(size_t index = 0; index < this->preferences.size(); index++)
		{
			debugOutput(to_string(this->preferences.at(index)) + " ");
		}
		debugOutput("\nBallot is: ",8);
		for(unsigned int index = 0; index < this->preferences.size(); index++)
		{
			debugOutput(to_string(buildABallot.at(index)) + " ");
		}
		debugOutput("\n\n",8);
	}
	return buildABallot;
}

void Voter::setStrategy(pollMethod pollMethodToSet, pairingMethod pairMethodToSet,ballotBuildMethod buildMethodToSet)
{
	this->strengthStrategy = pollMethodToSet;
	this->pairingStrategy = pairMethodToSet;
	this->ballotBuildStrategy = buildMethodToSet;
}

string Voter::printStrat() const
{
	return stratToString(this->getStrengthEval(),this->getPairing(),this->getBuildMethod());
}

int Voter::preferenceAt(unsigned candidateIndex) const 
{
	if(candidateIndex >= this->preferences.size())
	{
		cerr << "Requested candidate " << candidateIndex << " which is larger than " << this->preferences.size() << "\n";
		exit(10);
	}	
	return this->preferences.at(candidateIndex);
}