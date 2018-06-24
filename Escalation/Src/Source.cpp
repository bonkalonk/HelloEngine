#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <time.h>

#include "Escalation_v1.h"

int main( void )
{
	std::cout << "Starting sims..." << std::endl;
	srand( (unsigned int)time( NULL ) );

	const unsigned int numResultTypes = (int)Escalation_v1::Game::GameState::MAX;
	int numResults[ numResultTypes ];
	memset( numResults, 0, numResultTypes * sizeof( int ) );
	int numRounds = 0;
	int minRounds = 100;
	int maxRounds = 0;

	// THREAD THESE UP

	const int numRuns = 1000000;
	const int runPerPercent = numRuns / 100;

	for ( int runIndex = 0; runIndex < numRuns; runIndex++ )
	{
		Escalation_v1::Simulation sim;
		sim.Run();

		auto gameEndState = sim.GetGameState();

		numResults[ (int)gameEndState ]++;
		const int numRoundsSimmed = sim.GetNumRounds();

		numRounds += numRoundsSimmed;
		minRounds = minRounds < numRoundsSimmed ? minRounds : numRoundsSimmed;

		if ( gameEndState != Escalation_v1::Game::GameState::EventDeckOut )
		{
			maxRounds = maxRounds > numRoundsSimmed ? maxRounds : numRoundsSimmed;
		}

		if ( ( runIndex % runPerPercent ) == 0 )
		{
			std::cout << runIndex / runPerPercent << "% complete..." << std::endl;
		}
	}

	float resultPercentages[ numResultTypes ] = { 0, 0, 0 };

	for ( unsigned int i = 0; i < numResultTypes; ++i )
	{
		resultPercentages[ i ] = 100.0f * (float)numResults[ i ] / (float)numRuns;
	}

	float averageGameLength = (float)numRounds / (float)numRuns;

	std::cout << "Played: " << numRuns << " games" << std::endl;

	auto LogResult = [&]( const char* text, const Escalation_v1::Game::GameState& state )
	{
		std::cout << text << resultPercentages[ (int)state ] << "%" << std::endl;
	};

	LogResult( "Crew won: ", Escalation_v1::Game::GameState::CrewWin );
	LogResult( "Sabs won: ", Escalation_v1::Game::GameState::SabsWin );
	LogResult( "Event Deck Ran out: ", Escalation_v1::Game::GameState::EventDeckOut );
	LogResult( "Drew : ", Escalation_v1::Game::GameState::InProgress );

	std::cout << "Games last: " << averageGameLength << " rounds, min=" << minRounds << ", max=" << maxRounds << std::endl;

	return 0;
}
