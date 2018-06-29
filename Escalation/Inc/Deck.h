#pragma once

#include <assert.h>
#include <memory>
#include <optional>
#include <type_traits>

struct CardBase
{
	CardBase() : _cardId( _nextCardId++ )
	{
		assert( _cardId != _InvalidId );
	}
	CardBase( CardBase&& other ) : _cardId( other._cardId )
	{
		assert( _cardId != _InvalidId );
	}

	CardBase( const CardBase& ) = delete;
	void operator = ( const CardBase& ) = delete;
	void operator = ( const CardBase&& ) = delete;

	int GetCardId() const { return _cardId; }

private:
	inline static int _nextCardId = 0;
	static const int _InvalidId = -1;

	const int _cardId = _InvalidId;
};


template < typename CardType >
struct DeckCore
{
	static_assert( std::is_base_of_v<CardBase, CardType>, "DeckCore can only contain Cards. Please inherit from CardBase to use." );

	struct CardRef
	{
		CardRef( CardType&& card, DeckCore& deck ) : _card( std::move( card ) ), _deck( deck ) {}
		~CardRef()
		{
			_deck.DiscardCard( /*_card*/ );
		}

		const CardType& GetCard() const { return _card; }

	protected:
		CardType _card;
		DeckCore& _deck;
	};

	using Card = std::unique_ptr< CardRef >;

	void AddNewCard()
	{}

	void DiscardCard()
	{}

	std::optional<Card> DrawCard()
	{
		return {};
	}

protected:
	// TODO : Card storage
};


namespace DeckFunctions
{
	template < typename Derived >
	struct DrawCardFunc
	{
		auto DrawCard()
		{
			return static_cast< Derived* >( this )->GetCore().DrawCard();
		}
	};

	template < typename Derived >
	struct DiscardCardFunc
	{
		void DiscardCard()
		{
			static_cast< Derived* >( this )->GetCore().DiscardCard();
		}
	};
}

template < typename CardType, template< typename > typename... DeckFuncs >
struct DeckHost : DeckFuncs< DeckHost< CardType, DeckFuncs... > >...
{
	DeckCore< CardType >& GetCore()
	{
		return _deck;
	}

protected:
	DeckCore< CardType > _deck;
};

template < typename CardType, template< typename > typename... DeckFuncs >
struct Deck : DeckHost< CardType, DeckFuncs... >
{
	using Base = DeckHost< CardType, DeckFuncs... >;

private:
	DeckCore< CardType >& GetCore()
	{
		return Base::GetCore();
	}
};


/*
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
	using PileType = std::set<CardData>;
	PileType _drawPile;
	PileType _discardPile;
	std::vector<int> _uniqueIds;

	void FillDrawPile();
};*/