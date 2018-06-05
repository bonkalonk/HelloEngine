#pragma once

#include <vector>

#include "Deck.h"
#include "Utility.h"

namespace Escalation_v1
{
	struct HazardPolicy
	{
	protected:
		bool CanPlay( const Deck::Card& card, int remainingValue ) const
		{
			return card->Value() <= remainingValue;
		}

		bool IsComplete( int currentValue, int cardValue ) const
		{
			return currentValue == cardValue;
		}
	};

	struct RoundPolicy
	{
	protected:
		bool CanPlay( const Deck::Card& card, int remainingValue ) const
		{
			return true;
		}

		bool IsComplete( int currentValue, int cardValue ) const
		{
			return currentValue > cardValue;
		}
	};

	template < typename EventSpecificsPolicy >
	struct EventCard : EventSpecificsPolicy
	{
		EventCard( Deck::Card eventCard ) : _eventCard( std::move( eventCard ) ) {}

		void PlayCard( Deck::Card card )
		{
			assert( CanPlay( card ) );

			_timeCards.push_back( std::move( card ) );
		}

		bool CanPlay( const Deck::Card& card ) const
		{
			return EventSpecificsPolicy::CanPlay( card, GetRemainingValue() );
		}

		bool IsComplete() const
		{
			int currentValue = GetCurrentValue();

			return EventSpecificsPolicy::IsComplete( currentValue, _eventCard->Value() );
		}

		bool WillComplete( const Deck::Card& card ) const
		{
			return card->Value() == GetRemainingValue();
		}

		int GetCurrentValue() const
		{
			int currentValue = 0;

			ForEach( _timeCards, [&currentValue]( const Deck::Card& card )
			{
				currentValue += card->Value();
			} );

			return currentValue;
		}

		int GetRemainingValue() const
		{
			int currentValue = GetCurrentValue();

			return _eventCard->Value() - currentValue;
		}

		int GetTargetValue() const
		{
			return _eventCard->Value();
		}

		std::vector<Deck::Card>& GetPlayedCards()
		{
			return _timeCards;
		}

	protected:
		Deck::Card _eventCard;
		std::vector<Deck::Card> _timeCards;

		friend struct Hazard;
	};

	struct Event : EventCard<RoundPolicy>
	{
		using EventCard<RoundPolicy>::EventCard;
	};

	struct Hazard : EventCard<HazardPolicy>
	{
		using EventCard<HazardPolicy>::EventCard;
		Hazard( Event&& event ) : EventCard<HazardPolicy>( std::move( event._eventCard ) ) {}
	};

	struct Player
	{
		Player( bool isSaboteur ) : _isSab( isSaboteur ) {}

		void SelectCard( std::vector<Deck::Card>& availableCards, Event& currentEvent, Deck& timeDeck ) const;
		void AssignCard( std::vector<Deck::Card>& availableCards, std::vector<Hazard>& currentHazards, Deck& timeDeck ) const;

	protected:
		bool _isSab;
	};


	struct Game
	{
		Game();

		void PlayRound( int roundNum );

		enum class GameState : int
		{
			InProgress = 0,
			CrewWin = 1,
			SabsWin = 2,
			EventDeckOut = 3,

			MAX
		};

		GameState GetCurrentGameState() const;

	protected:
		Deck _timeDeck;
		Deck _eventDeck;
		std::vector<Player> _players;

		int _firstPlayerIndex = 0;
		std::vector<Hazard> _currentHazards;

		int GetNumInPlayHazards() const;
		int GetNumCardsToDraw() const;
	};


	struct Simulation
	{
		void Run();
		void LogResults();
		Game::GameState GetGameState() const;
		int GetNumRounds() const;

	protected:
		Game _game;
		Game::GameState _currentGameState = Game::GameState::InProgress;
		int _numRounds = 0;
		const int _maxRounds = 100;
	};

};
