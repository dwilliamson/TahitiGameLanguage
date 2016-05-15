

#include <cassert>
#include <cstdio>
#include <cmath>
#include "Canvas.h"
#include <ptc.h>


class cCanvasImpl : public cCanvas
{
public:
	// Constructor/Destructor
	cCanvasImpl(int width, int height);
	~cCanvasImpl(void);

	// Lock/Unlock methods (a bit quicker than doing this per pixel)
	void	BeginFrame(void);
	void	EndFrame(void);

	// Draw yer pixel
	void	PlotPixel(int x, int y, const cColour &colour);
	void	GetPixel(int x, int y, cColour &colour);

	// Is a key being pressed?
	bool	IsKeyPressed(void);

	// Retrieve canvas dimensions
	int		GetWidth(void);
	int		GetHeight(void);

	// Clear!
	void	ClearScreen(const cColour &colour);

	void	DumpBin(const char *filename);

private:
	// Format of the surface
	Format			*m_Format;

	// Console where the surface is displayed
	Console			m_Console;

	// Frame buffer
	unsigned int	*m_Buffer;

	// Canvas dimensions
	int				m_Width, m_Height;
};


cCanvas *CreateCanvas(int width, int height)
{
	return (new cCanvasImpl(width, height));
}


void DestroyCanvas(cCanvas *canvas_ptr)
{
	delete (cCanvasImpl *)canvas_ptr;
}


cCanvasImpl::cCanvasImpl(int width, int height)
{
	// Create the surface format
	m_Format = new Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF);

	// Open the console
	m_Console.open("Steering", width, height, *m_Format);

	// Defaults
	m_Buffer = new unsigned int[width * height];
	m_Width = width;
	m_Height = height;
}


cCanvasImpl::~cCanvasImpl(void)
{
	// Release the PTC resources
	m_Console.close();
	delete m_Format;
	delete [] m_Buffer;
}


void cCanvasImpl::BeginFrame(void)
{
}


void cCanvasImpl::EndFrame(void)
{
	// Copy the surface to the console
	m_Console.load(m_Buffer, m_Width, m_Height, m_Width * 4, *m_Format, Palette());

	// Update the console
	m_Console.update();
}


void cCanvasImpl::PlotPixel(int x, int y, const cColour &colour)
{
	// Just make sure this is being called within a beginframe/endframe block
	assert(m_Buffer);

	// Convert to 32-bit
	unsigned int int_colour = colour;

	// Plot the pixel
	if (x >= 0 && x < m_Width && y >= 0 && y < m_Height)
		m_Buffer[y * m_Width + x] = int_colour;
}


void cCanvasImpl::GetPixel(int x, int y, cColour &colour)
{
	// Make sure this is being called within a beginframe/endframe block
	assert(m_Buffer);
	
	// Get the converted pixel
	if (x >= 0 && x < m_Width && y >= 0 && y < m_Height)
		colour = m_Buffer[y * m_Width + x];
}


bool cCanvasImpl::IsKeyPressed(void)
{
	return (m_Console.key());
}


int cCanvasImpl::GetWidth(void)
{
	return (m_Width);
}


int cCanvasImpl::GetHeight(void)
{
	return (m_Height);
}


void cCanvasImpl::ClearScreen(const cColour &colour)
{
	// Convert to packed
	unsigned int col = colour;

	// Calculate copy parameters
	void *buffer = m_Buffer;
	int len = m_Width * m_Height;

	// Clear the screen
	__asm
	{
		mov	edi, buffer
		mov eax, col
		mov ecx, len

		rep stosd
	}
}


void cCanvasImpl::DumpBin(const char *filename)
{
	int		i;

	FILE *fp = fopen(filename, "wb");

	for (i = 0; i < m_Width * m_Height; i++)
	{
		fputc((m_Buffer[i] >> 16) & 255, fp);
		fputc((m_Buffer[i] >> 8) & 255, fp);
		fputc(m_Buffer[i] & 255, fp);
	}

	fclose(fp);
}


/*
=== Primitive implementations ===
*/


void cCanvas::DrawBresenhamCircle(int centre_x, int centre_y, int radius, const cColour &colour)
{
	int	x = 0, y = radius, p = 3 - (radius << 1);

	while (x <= y)
	{
		// Draw each octant pixel
		PlotPixel(centre_x + x, centre_y + y, colour);
		PlotPixel(centre_x - x, centre_y + y, colour);
		PlotPixel(centre_x + x, centre_y - y, colour);
		PlotPixel(centre_x - x, centre_y - y, colour);
		PlotPixel(centre_x + y, centre_y + x, colour);
		PlotPixel(centre_x - y, centre_y + x, colour);
		PlotPixel(centre_x + y, centre_y - x, colour);
		PlotPixel(centre_x - y, centre_y - x, colour);

		// x always loops linearly (within the octant)
		if (p < 0)
			p += (x++ << 2) + 6;
		else
			p += ((x++ - y--) << 2) + 10;
	}
}


void cCanvas::DrawBresenhamCircleFilled(int centre_x, int centre_y, int radius, const cColour &colour)
{
	int x = 0, y = radius, p = 3 - (radius << 1), i, j = 0;

	while (x <= y)
	{
		// Loop from the octant diagonal to the circle circumference
		for (i = j; i < y; i++)
		{
			// Draw each octant pixel
			PlotPixel(centre_x + x, centre_y + i, colour);
			PlotPixel(centre_x - x, centre_y + i, colour);
			PlotPixel(centre_x + x, centre_y - i, colour);
			PlotPixel(centre_x - x, centre_y - i, colour);
			PlotPixel(centre_x + i, centre_y + x, colour);
			PlotPixel(centre_x - i, centre_y + x, colour);
			PlotPixel(centre_x + i, centre_y - x, colour);
			PlotPixel(centre_x - i, centre_y - x, colour);
		}

		if (p < 0)
			p += (x++ << 2) + 6;
		else
			p += ((x++ - y--) << 2) + 10;

		j++;
	}
}


void cCanvas::DrawBresenhamCircleOutline(int centre_x, int centre_y, int radius, const cColour &inner, const cColour &outer)
{
	DrawBresenhamCircleFilled(centre_x, centre_y, radius, inner);
	DrawBresenhamCircle(centre_x, centre_y, radius, outer);
}


void cCanvas::DrawLine(float x1, float y1, float x2, float y2, const cColour &colour)
{
	float	hl = (float)fabs(x2 - x1), vl = (float)fabs(y2 - y1), length = (hl > vl) ? hl : vl;
	float	delta_x = (x2 - x1) / (float)length, delta_y = (y2 - y1) / (float)length;
	int		i;

	if (x1 == x2 && y1 == y2)
		return;

	for (i = 0; i <= (int)length; i++)
	{
		unsigned int x = (int)x1 , y = (int)y1;
		PlotPixel(x, y, colour);
		x1 += delta_x;
		y1 += delta_y;
	}
}