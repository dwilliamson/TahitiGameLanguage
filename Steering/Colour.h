

#define _INCLUDED_COLOUR_H


template <typename T> T min(const T &a, const T &b) { return (a < b ? a : b); }
template <typename T> T max(const T &a, const T &b) { return (a > b ? a : b); }


class cColour
{
public:
	// Public components
	float	r, g, b, a;

	// Constructors
	cColour(void);
	cColour(float r, float g, float b, float a = 1);
	cColour(unsigned int colour);

	// Methods
	cColour	&Add(const cColour &other);
	cColour	&Subtract(const cColour &other);
	cColour	&Scale(float scale);
	cColour	&Clamp(void);
	cColour	&Set(float r, float g, float b, float a = 1);
	cColour	&Zero(void);

	// Operator overloads that create temporaries
	cColour	operator + (const cColour &other) const;
	cColour	operator - (const cColour &other) const;
	cColour	operator * (float scale) const;

	// Operator overloads that don't create temporaries
	cColour	&operator += (const cColour &other);
	cColour	&operator -= (const cColour &other);
	cColour &operator *= (float scale);

	// For conversion to an ARGB 32-bit value
	operator unsigned int (void) const;
};


cColour operator * (float scale, const cColour &other);