# CFLAGS = -g -Wall
# CFLAGS = -O2 -Wall
CFLAGS = -O3 -Wall
CC = g++

all: voteMain voteResearch1000 vote3Research1k

voteMain: vote.cpp voteMain.cpp
	$(CC) $(CFLAGS) -o voteMain vote.cpp voteMain.cpp

voteTestProgressive: vote.cpp voteTestProgressive.cpp
	$(CC) $(CFLAGS) -o voteTestProgressive vote.cpp voteTestProgressive.cpp

voteTestIndividual: vote.cpp voteTestIndividual.cpp
	$(CC) $(CFLAGS) -o voteTestIndividual vote.cpp voteTestIndividual.cpp

voteTestIndividual2: vote.cpp voteTestIndividual2.cpp
	$(CC) $(CFLAGS) -o voteTestIndividual2 vote.cpp voteTestIndividual2.cpp

voteTestIndividual3: vote.cpp voteTestIndividual3.cpp
	$(CC) $(CFLAGS) -o voteTestIndividual3 vote.cpp voteTestIndividual3.cpp

voteTestProgressive3: vote.cpp voteTestProgressive3.cpp
	$(CC) $(CFLAGS) -o voteTestProgressive3 vote.cpp voteTestProgressive3.cpp

voteResearch: vote.cpp voteResearch.cpp
	$(CC) $(CFLAGS) -o voteResearch vote.cpp voteResearch.cpp	

voteResearch100: vote.cpp voteResearch100.cpp
	$(CC) $(CFLAGS) -o voteResearch100 vote.cpp voteResearch100.cpp	

voteResearch1000: vote.cpp voteResearch1000.cpp
	$(CC) $(CFLAGS) -o voteResearch1000 vote.cpp voteResearch1000.cpp
	
vote2Research1k: vote.cpp vote2Research1k.cpp
	$(CC) $(CFLAGS) -o vote2Research1k vote.cpp vote2Research1k.cpp	
	
vote3Research1k: vote.cpp vote3Research1k.cpp
	$(CC) $(CFLAGS) -o vote3Research1k vote.cpp vote3Research1k.cpp	

clean:
	rm -f *.o core voteMain voteTestIndividual voteTestProgressive voteTestIndividual2 voteTestIndividual3 voteTestProgressive3 voteResearch voteResearch100 vote2Research1k voteResearch1000 vote3Research1k
