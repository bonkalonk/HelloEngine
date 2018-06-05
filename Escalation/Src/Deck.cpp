#include <algorithm>
#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <time.h>
#include <vector>

#include "Deck.h"

void Deck::AddCards( int cardValue, int numCards )
{
	for ( int i = 0; i < numCards; i++ )
	{
		_discardPile.push_back( cardValue );
		_uniqueIds.push_back( _discardPile.back()._uniqueId );
	}
}

Deck::Card Deck::DrawCard()
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

void Deck::DiscardCard( const CardData& discardCard )
{
	_discardPile.push_back( discardCard );

	assert( std::find( _uniqueIds.begin(), _uniqueIds.end(), discardCard._uniqueId ) != _uniqueIds.end() );
}

void Deck::FillDrawPile()
{
	assert( _drawPile.size() == 0 );
	_drawPile = std::move( _discardPile );
	_discardPile = std::vector<CardData>();
}

bool Deck::IsEmpty() const
{
	return ( _drawPile.size() == 0 );
}

unsigned int Deck::CardData::_cardCount = 0;
