#pragma once

template < typename ContainerType, typename Func >
void ForEach( ContainerType& vector, Func func )
{
	for ( auto& It : vector )
	{
		func( It );
	}
}