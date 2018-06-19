#pragma once

#include <memory>
#include <set>
#include <vector>

struct Deck
{
protected:
	struct CardData
	{
		CardData( int value ) : _value( value ), _uniqueId( _cardCount++ ) {}
		CardData( const CardData& other ) : _value( other._value ), _uniqueId( other._uniqueId ) {}
		void operator = ( const CardData& other ) = delete;
		void operator = ( const CardData&& other ) = delete;

		const int _value;
		const unsigned int _uniqueId;


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
		CardRef( const CardData& card, Deck& deck ) : _cardData( card ), _deck( deck ) {}
		CardRef( const CardRef&& other ) : _cardData( other._cardData ), _deck( other._deck ) {}

		CardRef( const CardRef& other ) = delete;
		void operator = ( const CardRef& other ) = delete;
		void operator = ( const CardRef&& other ) = delete; // But should be do-able

		~CardRef()
		{
			_deck.DiscardCard( _cardData );
		}

		int Value() const
		{
			return _cardData._value;
		}

		bool operator == ( const CardRef& other ) const
		{
			return _cardData == other._cardData;
		}

	protected:
		CardData _cardData;
		Deck& _deck;
	};

	using Card = CardRef;

	void AddCards( int cardValue, int numCards );
	Card DrawCard();
	void DiscardCard( const CardData& discardCard );
	bool IsEmpty() const;


protected:
	std::vector<CardData> _drawPile;
	std::vector<CardData> _discardPile;
	std::vector<int> _uniqueIds;
	
	void FillDrawPile();
};