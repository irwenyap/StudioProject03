#include "Vector2.h"

Vector2::Vector2( float a, float b )
{
	x = a;
	y = b;
}

Vector2::Vector2( const Vector2 &rhs )
{
	x = rhs.x;
	y = rhs.y;
}

void Vector2::Set( float a, float b )
{
}

Vector2 Vector2::operator+( const Vector2& rhs ) const
{
	return Vector2();
}

Vector2 Vector2::operator-( const Vector2& rhs ) const
{
	return Vector2();
}

Vector2 Vector2::operator-( void ) const
{
	return Vector2();
}

Vector2 Vector2::operator*( float scalar ) const
{
	return Vector2();
}

float Vector2::Length( void ) const
{
	return 0;
}

float Vector2::Dot( const Vector2& rhs ) const
{
	return 0;
}

Vector2 Vector2::Normalized( void )
{
	return Vector2();
}
