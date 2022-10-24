#ifndef VOTER_H
#define VOTER_H

#include <string>
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>
#include <type_traits>

using namespace std;

//https://stackoverflow.com/questions/12290451/access-extern-variable-in-c-from-another-file
extern int voting_seed;

enum electionType {
	approval,
	plurality,
	antiplurality,
	borda,
	hare,
	copeland,
	simpson,
	schultze,
	coombs,
	numElectionTypes
	};

const string possibleCandidates = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

const unsigned int DEFAULT_CANDIDATE_COUNT = 5;

const int MIN_RATING = 0;
const int MAX_RATING = 100 * (DEFAULT_CANDIDATE_COUNT <= 20 ? 1 : DEFAULT_CANDIDATE_COUNT / 5);

const bool FORBID_PREFERENCE_TIES = false;

const electionType DEFAULT_ELECTION_TYPE = approval;

//something to estimate strengths?
//enum strategy {strategyA, strategy_sentinel};
enum pollMethod {
	strat_allZero,
	strat_approval,
	strat_plurality,
	strat_antiplurality,
	/* strat_distanceToWin, */
	strat_bordaEval,
	strat_pairwise,
	strat_victoryPairwise,
	strat_winOrLosePairwise,
	strat_hareEliminationSpeed,
	strat_copeland,
	// strat_copelandPairwise,
	strat_simpson,
	// strat_simpsonPairwise,
	strat_sentinelValue
	};
enum pairingMethod {
	prioritizeLeader,
	prioritizeRunnerUp, 
	neighbors, 
	pair_sentinelValue
	};
enum ballotBuildMethod {
	bothEnds, 
	front, 
	back, 
	build_sentinelValue
	};

void debugOutput(stringstream const&theStringStream,const int&debugLevel,int indent);
void debugOutput(string const &theString,const int &debugLevel,int indent);
void debugOutput(string const &theString);

vector<int> sorted_indexes(const vector<int> theVectorToFakeSort, const bool reverse = false);
string stratToString(pollMethod aPollMethod, pairingMethod aPairingMethod, ballotBuildMethod aBallotBuildMethod);

template<typename T> string vectorString(const vector<T>&theVector, int width = 3, bool letters = false)
{
	stringstream theStream;
	theStream << "{";
	for(unsigned int index = 0; index < theVector.size(); index++)
	{
		theStream << right << setw(width);
		if(letters)
		{
			theStream << static_cast<char>(theVector.at(index)+'A');
		}
		//https://stackoverflow.com/questions/6251889/type-condition-in-template
		else if(is_same<T,bool>::value)
		{
			theStream << (theVector.at(index) ? "T" : "F");
		}
		else
		{
			theStream << to_string(theVector.at(index));
		}
		if(index+1 != theVector.size())
		{
			theStream << ",";
		}
	}
	theStream << "}";
	return theStream.str();
};

//https://stackoverflow.com/questions/4964482/how-to-create-two-classes-in-c-which-use-each-other-as-data

class Ballot;
class Voter;

/* default_random_engine randomEngine;
uniform_int_distribution<int> theDistribution(MIN_RATING,MAX_RATING);
 */
class Election
{
private:
	vector<Ballot> ballots;
	//vector<Ballot> newBallots, currentBallots;
	//Then use swap() between rounds for batch.
	//How do I toss out a voter's previous ballot?
	//vector<Ballot> oldBallots;// For batch mode maybe?
	//Replace by voter index!
	electionType thisElectionType;
	unsigned int numCandidates;
	string hareOutput;
	//https://en.cppreference.com/w/cpp/numeric/random/random_device
	//https://www.cplusplus.com/reference/random/uniform_int_distribution/
	//https://stackoverflow.com/questions/21516575/fill-a-vector-with-random-numbers-c
	//https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector
	//https://stackoverflow.com/questions/34126262/use-random-in-a-c-class
	//https://stackoverflow.com/questions/61348210/how-do-i-use-c11-random-engine-and-uniform-distribution-in-class-constructor
	//https://www.cplusplus.com/reference/random/uniform_int_distribution/operator()/
	//int seed = time(0);
	//https://en.cppreference.com/w/cpp/chrono/system_clock/now ??
	vector<vector<int>> pairwiseMatrix;
public:
	//Election();
	Election(int num = DEFAULT_CANDIDATE_COUNT, electionType theElectionType = DEFAULT_ELECTION_TYPE);
	//WhoWon()
	vector<int> currentPoll() const;
	vector<int> currentPoll(electionType thisElectionType) const;
	vector<int> currentPoll(pollMethod thisStrategy) const;
	//NewRound()?
	unsigned int getNumCandidates() const {return this->numCandidates;}
	electionType getElectionType() const {return this->thisElectionType;}
	//int getRandomInt() const {return theDistribution(randomEngine);}
	string toString() const;
	//int getSeed() const {return seed;}
	bool generateBallots(const vector<Voter>&theElectorate);
	unsigned int getWinner() const;
	tuple<unsigned int,vector<int>,vector<vector<int>>> runHare() const;
	tuple<unsigned int,vector<int>> runCopeland() const;
	tuple<unsigned int,vector<int>> runSimpson() const;
	tuple<unsigned int,vector<vector<int>>> runSchultze() const;
	tuple<unsigned int,vector<int>,vector<vector<int>>> runCoombs() const;
	pair<int,bool> runElection(int numberOfRounds,const vector<Voter>&theElectorate);
	//const vector<Ballot>&getBallots() const {return this->ballots;};
	void shiftPairwise(const vector<int>&, const vector<int>&,bool);
};

class Ballot
{
private:
	vector<int> ratings;
	//Rankings are ordinal, ratings are cardinal value,
	//Preferences (true-mind) example: {25, 21, 19, 52, 44}
	//Ratings example: {98, 99, 100, 0, 1}
	//normalizedBallot for Approval: {1, 1, 1, 0, 0}
	//normalizedBallot for Plurality: {0, 0, 1, 0, 0}
	//normalizedBallot for antiplurality:  {1, 1, 1, 0, 1}
	//normalizedBallot for borda:  {2, 3, 4, 0, 1}
	
public:
	Ballot();
	// Ballot(const vector<int> &givenRatings) {this->ratings = givenRatings;}
	Ballot(const Election &theElection, const Voter &theVoter);
	vector<int> normalizedBallot(const electionType &theElectionType) const;
	//Approval - Average> : 1, Average<= : 0
	vector<int> getRatings() const {return ratings;};
	int at(unsigned int indexValue) const {return this->getRatings().at(indexValue);};
};

class Voter
{
private:
	vector<int> preferences;
	// vector<int> ranks;
	unsigned int voterIDNumber;
	//strategy overallStrategy;
	pollMethod strengthStrategy;
	pairingMethod pairingStrategy;
	ballotBuildMethod ballotBuildStrategy;
	static int VoterIDCounter;
	//vector<tuple<int,int>> pairings;
public:
	Voter(int VoterID, const Election &theElection, pollMethod theStrengthStrategy = strat_approval,pairingMethod thePairingMethod = prioritizeLeader,ballotBuildMethod theBallotBuildStrategy = bothEnds);
	Voter(vector<int> inputPreferences, int VoterID, const Election &theElection, pollMethod theStrengthStrategy = strat_approval,pairingMethod thePairingMethod = prioritizeLeader,ballotBuildMethod theBallotBuildStrategy = bothEnds);
	string toString() const;
	int getVoterID() const {return voterIDNumber;}
	//strategy getStrategy() const {return overallStrategy;}
	pollMethod getStrengthEval() const {return strengthStrategy;}
	pairingMethod getPairing() const {return pairingStrategy;}
	ballotBuildMethod getBuildMethod() const {return ballotBuildStrategy;}
	//vector<int> getRanks() const {return ranks;}
	vector<tuple<int,int,int>> getPairings(const vector<int> &currentPoll) const;
	//vector<int> generateBallot(vector<int> currentPoll) const;
	vector<int> generateBallot(const Election &thisElection) const;
	void setStrategy(pollMethod pollMethodToSet,pairingMethod pairMethodToSet,ballotBuildMethod buildMethodToSet);
	string printStrat() const;
	int getHappinessShift(int oldCandidate, int newCandidate) const {return this->preferences.at(newCandidate) - this->preferences.at(oldCandidate);}
	int preferenceAt(unsigned candidateIndex) const;
};

//Take ballots and preferences and return ballot.

#endif // #ifndef VOTER_H