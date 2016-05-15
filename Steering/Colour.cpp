

#include "Colour.h"


cColour::cColour(void)
{
	// ... Optimise me out please, Mr. Compiler ...
}


cColour::cColour(float r, float g, float b, float a)
{
	cColour::r = r;
	cColour::g = g;
	cColour::b = b;
	cColour::a = a;
}


cColour::cColour(unsigned int colour)
{
	r = ((colour >> 16) & 255) / 255.0f;
	g = ((colour >> 8)  & 255) / 255.0f;
	b =  (colour        & 255) / 255.0f;
	a = ((colour >> 24) & 255) / 255.0f;
}


cColour &cColour::Add(const cColour &other)
{
	r += other.r;
	g += other.g;
	b += other.b;
	a += other.a;

	return (*this);
}


cColour &cColour::Subtract(const cColour &other)
{
	r -= other.r;
	g -= other.g;
	b -= other.b;
	a -= other.a;

	return (*this);
}


cColour &cColour::Scale(float scale)
{
	r *= scale;
	g *= scale;
	b *= scale;
	a *= scale;

	return (*this);
}


cColour &cColour::Clamp(void)
{
	r = min(1.0f, max(0.0f, r));
	g = min(1.0f, max(0.0f, g));
	b = min(1.0f, max(0.0f, b));
	a = min(1.0f, max(0.0f, a));

	return (*this);
}


cColour &cColour::Set(float r, float g, float b, float a)
{
	cColour::r = r;
	cColour::g = g;
	cColour::b = b;
	cColour::a = a;

	return (*this);
}


cColour &cColour::Zero(void)
{
	cColour::r = 0;
	cColour::g = 0;
	cColour::b = 0;
	cColour::a = 0;

	return (*this);
}


cColour cColour::operator + (const cColour &other) const
{
	cColour result = *this;
	result.Add(other);
	return (result);
}


cColour cColour::operator - (const cColour &other) const
{
	cColour result = *this;
	result.Subtract(other);
	return (result);
}


cColour cColour::operator * (float scale) const
{
	cColour result = *this;
	result.Scale(scale);
	return (result);
}


cColour &cColour::operator += (const cColour &other)
{
	return (Add(other));
}


cColour &cColour::operator -= (const cColour &other)
{
	return (Subtract(other));
}


cColour &cColour::operator *= (float scale)
{
	return (Scale(scale));
}


cColour::operator unsigned int (void) const
{
	unsigned int r, g, b, a;

	// Convert to an 8-bit value
	r = (unsigned int)(cColour::r * 255);
	g = (unsigned int)(cColour::g * 255);
	b = (unsigned int)(cColour::b * 255);
	a = (unsigned int)(cColour::a * 255);

	// Return the complete 32-bit value
	return ((a << 24) | (r << 16) | (g << 8) | (b));
}


cColour operator * (float scale, const cColour &other)
{
	cColour result = other;
	result.Scale(scale);
	return (result);
}