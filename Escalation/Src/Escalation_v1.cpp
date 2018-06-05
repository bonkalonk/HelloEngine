#include <algorithm>
#include <assert.h>
#include <iostream>

#include "Escalation_v1.h"

using namespace Escalation_v1;

const int NumStartingHazards = 2;
const int NumHazardsForCrewWin = 0;
const int NumHazardsForSabWin = 4;


void Player::SelectCard( std::vector<Deck::Card>& availableCards, Event& currentEvent, Deck& timeDeck ) const
{
	assert( availableCards.size() >= 2 );

	struct CardInfo
	{
		CardInfo( int index, std::vector<Deck::Card>& availableCards )
			: _cardIndex( index )
			, _cardValue( availableCards[ index ]->Value() )
		{}

		void Set( int index, int value )
		{
			_cardIndex = index;
			_cardValue = value;
		}

		int _cardIndex;
		int _cardValue;
	};

	CardInfo cardToPlay{ 0, availableCards };
	CardInfo cardToDiscard{ 1, availableCards };

	for ( int i = 0; i < availableCards.size(); ++i )
	{
		const Deck::Card& card = availableCards[ i ];
		const int cardValue = card->Value();

		const bool shouldPlayCard = _isSab ? ( cardValue > cardToPlay._cardValue ) : ( cardValue < cardToPlay._cardValue );
		const bool shouldDiscardCard = _isSab ? ( cardValue < cardToDiscard._cardValue ) : ( cardValue > cardToDiscard._cardValue );

		if ( shouldPlayCard )
		{
			cardToPlay.Set( i, cardValue );
		}

		if ( shouldDiscardCard )
		{
			cardToDiscard.Set( i, cardValue );
		}
	}

	assert( cardToPlay._cardIndex != cardToDiscard._cardIndex );

	currentEvent.PlayCard( std::move( *( availableCards.begin() + cardToPlay._cardIndex ) ) );
	availableCards.erase( availableCards.begin() + std::max( cardToPlay._cardIndex, cardToDiscard._cardIndex ) );
	availableCards.erase( availableCards.begin() + std::min( cardToPlay._cardIndex, cardToDiscard._cardIndex ) );
}

void Player::AssignCard( std::vector<Deck::Card>& availableCards, std::vector<Hazard>& currentHazards, Deck& timeDeck ) const
{
	assert( currentHazards.size() != 0 );

	Deck::Card& lastCard = availableCards.back();

	Hazard* bestHazard = [&]
	{
		Hazard* matchingHazard = nullptr;
		for ( Hazard& hazard : currentHazards )
		{
			if ( hazard.WillComplete( lastCard ) )
			{
				return &hazard;
			}
			else if ( ( matchingHazard == nullptr ) && hazard.CanPlay( lastCard ) )
			{
				matchingHazard = &hazard;
			}
		}
		return matchingHazard;
	}( );

	if ( bestHazard )
	{
		int hazardValuePreCard = bestHazard->GetCurrentValue();
		bestHazard->PlayCard( std::move( lastCard ) );
	}

	availableCards.pop_back();
}


Game::Game()
{
	// Todo : variable
	_timeDeck.AddCards( 0, 3 );
	_timeDeck.AddCards( 1, 5 );
	_timeDeck.AddCards( 2, 5 );
	_timeDeck.AddCards( 3, 5 );
	_timeDeck.AddCards( 4, 5 );
	_timeDeck.AddCards( 5, 5 );

	_eventDeck.AddCards( 6, 2 );
	_eventDeck.AddCards( 7, 2 );
	_eventDeck.AddCards( 8, 2 );
	_eventDeck.AddCards( 9, 2 );

	_players.emplace_back( false );
	_players.emplace_back( false );
	_players.emplace_back( false );
	_players.emplace_back( true );

	for ( int i = 0; i < NumStartingHazards; ++i )
	{
		_currentHazards.emplace_back( _eventDeck.DrawCard() );
	}
}

void Game::PlayRound( int roundNum )
{
	Event roundEvent( _eventDeck.DrawCard() );

	const int numCardsToDraw = GetNumCardsToDraw();
	std::vector<Deck::Card> timeCards;

	for ( int i = 0; i < numCardsToDraw; ++i )
	{
		timeCards.emplace_back( _timeDeck.DrawCard() );
	}

	for ( int i = 0; i < _players.size(); ++i )
	{
		int playerIndex = ( i + _firstPlayerIndex ) % _players.size();
		const Player& player = _players[ playerIndex ];

		timeCards.emplace_back( _timeDeck.DrawCard() );
		player.SelectCard( timeCards, roundEvent, _timeDeck );
	}

	if ( roundEvent.IsComplete() )
	{
		_currentHazards.push_back( std::move( roundEvent ) );
	}
	else
	{
		std::vector<Deck::Card>& roundCards = roundEvent.GetPlayedCards();
		while ( roundCards.size() > 0 )
		{
			// just use last player for now, all players the same
			_players.back().AssignCard( roundCards, _currentHazards, _timeDeck );
		}

		for ( int i = (int)_currentHazards.size() - 1; i >= 0; --i )
		{
			if ( _currentHazards[ i ].IsComplete() )
			{
				_currentHazards.erase( _currentHazards.begin() + i );
			}
		}
	}

	_firstPlayerIndex = ( _firstPlayerIndex + 1 ) % _players.size();
};

Game::GameState Game::GetCurrentGameState() const
{
	const int numInPlayHazards = GetNumInPlayHazards();

	if ( numInPlayHazards == NumHazardsForCrewWin )
	{
		return GameState::CrewWin;
	}
	else if ( numInPlayHazards == NumHazardsForSabWin )
	{
		return GameState::SabsWin;
	}
	else if ( _eventDeck.IsEmpty() )
	{
		return GameState::EventDeckOut;
	}

	return GameState::InProgress;
}

int Game::GetNumInPlayHazards() const
{
	return (int)_currentHazards.size();
}

int Game::GetNumCardsToDraw() const
{
	return (int)( _players.size() * 1 ) + 1;
}


void Simulation::Run()
{
	while ( ( _numRounds < _maxRounds ) && ( _currentGameState == Game::GameState::InProgress ) )
	{
		_game.PlayRound( _numRounds );
		_currentGameState = _game.GetCurrentGameState();
		_numRounds++;
	}
}

void Simulation::LogResults()
{
	std::cout << std::endl;

	switch ( _currentGameState )
	{
	case Game::GameState::CrewWin:
		std::cout << "Crew Won!" << std::endl;
		break;
	case Game::GameState::SabsWin:
		std::cout << "Sabs Won!" << std::endl;
		break;
	case Game::GameState::EventDeckOut:
		std::cout << "Event Deck ran out." << std::endl;
		break;
	case Game::GameState::InProgress:
	default:
		std::cout << "It was a draw..." << std::endl;
	}
}

Game::GameState Simulation::GetGameState() const
{
	return _currentGameState;
}

int Simulation::GetNumRounds() const
{
	return _numRounds;
}
