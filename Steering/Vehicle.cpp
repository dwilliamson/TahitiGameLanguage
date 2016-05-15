
#include "main.h"


const double PI = 3.1415926535897932384626433832795;


// NICE!
extern cCanvas	*g_CanvasPtr;


Vehicle::Vehicle(void)
{
	// Seed RNG
	srand((unsigned int)time(NULL));
}


void Vehicle::RegisterMethods(void)
{
	vm_START_METHOD_REGISTER(Vehicle);

	// Sets the position to draw the vehicle circle
	vm_REGISTER_METHOD(SetDrawPosition, "void", "int x, int y");

	// Sets the velocity so that the velocity direction/magnitude can be drawn
	vm_REGISTER_METHOD(SetDrawVelocity, "void", "int x, int y");

	// Return the sine of the passed angle (angle in radians)
	vm_REGISTER_METHOD(sin, "float", "float angle");

	// Return the cosine of the passed angle (angle in radians)
	vm_REGISTER_METHOD(cos, "float", "float angle");

	// Generate a random number from 0 to 1
	vm_REGISTER_METHOD(Random, "float", "void");

	// Calculate the distance between two points
	vm_REGISTER_METHOD(Distance, "float", "float xa, float ya, float xb, float yb");

	// Print a string to console
	vm_REGISTER_METHOD(PrintString, "void", "string value");

	// Print a floating point value to console
	vm_REGISTER_METHOD(PrintFloat, "void", "float value");

	vm_END_METHOD_REGISTER;
}


void Vehicle::SetDrawPosition(int x, int y)
{
	// Backup position
	m_OldDrawPos[0] = m_DrawPos[0];
	m_OldDrawPos[1] = m_DrawPos[1];

	// Set new
	m_DrawPos[0] = x;
	m_DrawPos[1] = y;
}


void Vehicle::SetDrawVelocity(int x, int y)
{
	m_DrawVel[0] = x;
	m_DrawVel[1] = y;
}


float Vehicle::sin(float angle)
{
	return ((float)(::sin(angle)));
}


float Vehicle::cos(float angle)
{
	return ((float)(::cos(angle)));
}


float Vehicle::Random(void)
{
	return ((float)((rand()%1000) / 1000.0f));
}


float Vehicle::Distance(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1, dy = y2 - y1;
	return ((float)sqrt(dx * dx + dy * dy));
}


void Vehicle::PrintFloat(float value)
{
	printf("%f\n", value);
}


void Vehicle::PrintString(char *string)
{
	printf("%s\n", string);
}


void Vehicle::Draw(void)
{
	// Vehicle body
	g_CanvasPtr->DrawBresenhamCircleOutline(
		m_DrawPos[0],
		m_DrawPos[1],
		DRAW_RADIUS,
		cColour(0.5f, 1, 0.5f),
		cColour(0, 0, 0));

	// Scaled velocity vector
	g_CanvasPtr->DrawLine(
		(float)m_DrawPos[0],
		(float)m_DrawPos[1],
		m_DrawPos[0] + m_DrawVel[0] / 10.0f,
		m_DrawPos[1] + m_DrawVel[1] / 10.0f,
		cColour(1, 0, 1));
}