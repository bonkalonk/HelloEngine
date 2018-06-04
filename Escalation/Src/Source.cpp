#include <algorithm>
#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <time.h>
#include <vector>

const int NumStartingHazards = 2;
const int NumHazardsForCrewWin = 0;
const int NumHazardsForSabWin = 4;

template < typename VectorType, typename Func >
void ForEach( VectorType& vector, Func func )
{
	for ( auto It = vector.begin(); It < vector.end(); ++It )
	{
		func( *It );
	}
}

struct Player;

struct Deck
{
protected:
	struct CardData
	{
		CardData( int value ) : _value( value ), _uniqueId( _cardCount++ ) {}
		CardData( const CardData& other ) : _value( other._value ), _uniqueId( other._uniqueId ) {}
		//void operator = ( const CardData& other ) = delete;
		//void operator = ( const CardData&& other ) = delete;

		/*const*/ int _value;
		/*const*/ unsigned int _uniqueId;


		bool operator == ( const CardData& other ) const
		{
			return ( _value == other._value ) && ( _uniqueId == other._uniqueId );
		}

	protected:
		static unsigned int _cardCount;
	};

public:

	struct CardRef
	{
		CardRef( const CardData& card, Deck& deck ) : _card( card ), _deck( deck ) {}
		CardRef( const CardRef& other ) = delete;
		CardRef( const CardRef&& other ) = delete;
		void operator = ( const CardRef& other ) = delete;
		void operator = ( const CardRef&& other ) = delete;

		~CardRef()
		{
			_deck.DiscardCard( _card );
		}

		int Value() const
		{
			return _card._value;
		}

		bool operator == ( const CardRef& other ) const
		{
			return _card == other._card;
		}

	protected:
		CardData _card;
		Deck& _deck;
	};

	using Card = std::unique_ptr<CardRef>;

	void AddCards( int cardValue, int numCards )
	{
		for ( int i = 0; i < numCards; i++ )
		{
			_discardPile.push_back( cardValue );
			_uniqueIds.push_back( _discardPile.back()._uniqueId );
		}
	}

	Card DrawCard()
	{
		if ( _drawPile.empty() )
		{
			FillDrawPile();
		}

		assert( !_drawPile.empty() );

		const int randomCardIndex = rand() % _drawPile.size();

		CardData returnCard = _drawPile.at( randomCardIndex );
		_drawPile.erase( _drawPile.begin() + randomCardIndex );

		return std::make_unique<CardRef>( returnCard, *this );
	}

	void DiscardCard( const CardData& discardCard )
	{
		_discardPile.push_back( discardCard );

		assert( std::find_if( _uniqueIds.begin(), _uniqueIds.end(), [&]( const int& other ) { return other == discardCard._uniqueId; } ) != _uniqueIds.end() );
	}

	void FillDrawPile()
	{
		assert( _drawPile.size() == 0 );
		_drawPile = std::move( _discardPile );
		_discardPile = std::vector<CardData>();
	}

	bool IsEmpty() const
	{
		return ( _drawPile.size() == 0 );
	}

protected:
	std::vector<CardData> _drawPile;
	std::vector<CardData> _discardPile;
	std::vector<int> _uniqueIds;
};

unsigned int Deck::CardData::_cardCount = 0;

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
	EventCard( Deck::Card eventCard, Deck& sourceDeck ) : _eventCard( std::move( eventCard ) ), _sourceDeck( &sourceDeck ) {}

	void PlayCard( Deck::Card& card )
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
	Deck* _sourceDeck;

	friend struct Hazard;
};

struct Event : EventCard<RoundPolicy>
{
	using EventCard<RoundPolicy>::EventCard;
};

struct Hazard : EventCard<HazardPolicy>
{
	using EventCard<HazardPolicy>::EventCard;
	Hazard( Event&& event ) : EventCard<HazardPolicy>( std::move( event._eventCard ), *event._sourceDeck ) {}
};

struct Player
{
	Player( bool isSaboteur ) : _isSab( isSaboteur ) {}

	void SelectCard( std::vector<Deck::Card>& availableCards, Event& currentEvent, Deck& timeDeck ) const
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

			const bool shouldPlayCard = [&]
			{
				return _isSab ? ( cardValue > cardToPlay._cardValue ) : ( cardValue < cardToPlay._cardValue );
			}( );

			const bool shouldDiscardCard = [&]
			{
				return _isSab ? ( cardValue < cardToDiscard._cardValue ) : ( cardValue > cardToDiscard._cardValue );
			}( );

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

		auto PlayCard = [&]
		{
			currentEvent.PlayCard( *( availableCards.begin() + cardToPlay._cardIndex ) );

			availableCards.erase( availableCards.begin() + cardToPlay._cardIndex );
		};

		auto DiscardCard = [&]
		{
			availableCards.erase( availableCards.begin() + cardToDiscard._cardIndex );
		};

		// dirty but..
		if ( cardToPlay._cardIndex < cardToDiscard._cardIndex )
		{
			DiscardCard();
			PlayCard();
		}
		else
		{
			PlayCard();
			DiscardCard();
		}
	}

	void AssignCard( std::vector<Deck::Card>& availableCards, std::vector<Hazard>& currentHazards, Deck& timeDeck ) const
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
			bestHazard->PlayCard( lastCard );
		}

		availableCards.pop_back();
	}

protected:
	bool _isSab;
};

struct EscalationGame
{
	EscalationGame()
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

		_players.push_back( false );
		_players.push_back( false );
		_players.push_back( false );
		_players.push_back( true );

		for ( int i = 0; i < NumStartingHazards; ++i )
		{
			_currentHazards.push_back( Hazard( _eventDeck.DrawCard(), _eventDeck ) );
		}
	}

	void PlayRound( int roundNum )
	{
		Event roundEvent( _eventDeck.DrawCard(), _eventDeck );

		const int numCardsToDraw = GetNumCardsToDraw();
		std::vector<Deck::Card> timeCards;

		for ( int i = 0; i < numCardsToDraw; ++i )
		{
			timeCards.push_back( _timeDeck.DrawCard() );
		}

		for ( int i = 0; i < _players.size(); ++i )
		{
			int playerIndex = ( i + _firstPlayerIndex ) % _players.size();
			const Player& player = _players[ playerIndex ];

			timeCards.push_back( _timeDeck.DrawCard() );
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

	enum class GameState : int
	{
		InProgress = 0,
		CrewWin = 1,
		SabsWin = 2,
		EventDeckOut = 3,

		MAX
	};

	int GetNumInPlayHazards() const
	{
		int numHazards = 0;
		for ( const Hazard& hazard : _currentHazards )
		{
			if ( hazard.IsComplete() == false )
			{
				numHazards++;
			}
		}
		return numHazards;
	}

	GameState GetCurrentGameState() const
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

	int GetNumCardsToDraw() const
	{
		return (int)( _players.size() * 1 ) + 1;
	}

protected:
	Deck _timeDeck;
	Deck _eventDeck;
	std::vector<Player> _players;

	int _firstPlayerIndex = 0;
	std::vector<Hazard> _currentHazards;
};

struct EscalationSimulation
{
	void Run()
	{
		while ( ( _numRounds < _maxRounds ) && ( _currentGameState == EscalationGame::GameState::InProgress ) )
		{
			_game.PlayRound( _numRounds );
			_currentGameState = _game.GetCurrentGameState();
			_numRounds++;
		}
	}

	void LogResults()
	{
		std::cout << std::endl;

		switch ( _currentGameState )
		{
		case EscalationGame::GameState::CrewWin:
			std::cout << "Crew Won!" << std::endl;
			break;
		case EscalationGame::GameState::SabsWin:
			std::cout << "Sabs Won!" << std::endl;
			break;
		case EscalationGame::GameState::InProgress:
		default:
			std::cout << "It was a draw..." << std::endl;
		}
	}

	EscalationGame::GameState GetGameState() const
	{
		return _currentGameState;
	}

	int GetNumRounds() const
	{
		return _numRounds;
	}

protected:
	EscalationGame _game;
	EscalationGame::GameState _currentGameState = EscalationGame::GameState::InProgress;
	int _numRounds = 0;
	const int _maxRounds = 100;
};

int main( void )
{
	std::cout << "Starting sims..." << std::endl;
	srand( (unsigned int)time( NULL ) );

	const unsigned int numResultTypes = (int)EscalationGame::GameState::MAX;
	int numResults[ numResultTypes ] = { 0, 0, 0 };
	int numRounds = 0;
	int minRounds = 100;
	int maxRounds = 0;

	// THREAD THESE UP

	const int numRuns = 10000;

	for ( int runIndex = 0; runIndex < numRuns; runIndex++ )
	{
		EscalationSimulation sim;
		sim.Run();

		EscalationGame::GameState gameEndState = sim.GetGameState();

		numResults[ (int)gameEndState ]++;
		const int numRoundsSimmed = sim.GetNumRounds();

		numRounds += numRoundsSimmed;
		minRounds = minRounds < numRoundsSimmed ? minRounds : numRoundsSimmed;

		if ( gameEndState != EscalationGame::GameState::EventDeckOut )
		{
			maxRounds = maxRounds > numRoundsSimmed ? maxRounds : numRoundsSimmed;
		}

		if ( ( runIndex % 100 ) == 0 )
		{
			std::cout << runIndex / 100 << "% complete..." << std::endl;
		}
	}

	float resultPercentages[ numResultTypes ] = { 0, 0, 0 };

	for ( unsigned int i = 0; i < numResultTypes; ++i )
	{
		resultPercentages[ i ] = 100.0f * (float)numResults[ i ] / (float)numRuns;
	}

	float averageGameLength = (float)numRounds / (float)numRuns;

	std::cout << "Played: " << numRuns << " games" << std::endl;

	auto LogResult = [&]( const char* text, const EscalationGame::GameState& state )
	{
		std::cout << text << resultPercentages[ (int)state ] << "%" << std::endl;
	};

	LogResult( "Crew won: ", EscalationGame::GameState::CrewWin );
	LogResult( "Sabs won: ", EscalationGame::GameState::SabsWin );
	LogResult( "Event Deck Ran out: ", EscalationGame::GameState::EventDeckOut );
	LogResult( "Drew : ", EscalationGame::GameState::InProgress );

	std::cout << "Games last: " << averageGameLength << " rounds, min=" << minRounds << ", max=" << maxRounds << std::endl;

	return 0;
}