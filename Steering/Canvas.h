

#ifndef	_INCLUDED_COLOUR_H
#include "Colour.h"
#endif


#define _INCLUDED_CANVAS_H


#pragma comment(lib, "ptcdebug.lib")


// This is to avoid PTC messing my program up with windows.h


struct cCanvas
{
	// Lock/Unlock methods (a bit quicker than doing this per pixel)
	virtual void	BeginFrame(void) = 0;
	virtual void	EndFrame(void) = 0;

	// Draw/get yer pixel
	virtual void	PlotPixel(int x, int y, const cColour &colour) = 0;
	virtual void	GetPixel(int x, int y, cColour &colour) = 0;

	// Is a key being pressed?
	virtual bool	IsKeyPressed(void) = 0;

	// Retrieve canvas dimensions
	virtual int		GetWidth(void) = 0;
	virtual int		GetHeight(void) = 0;

	// Clear!
	virtual void	ClearScreen(const cColour &colour) = 0;

	virtual void	DumpBin(const char *filename) = 0;

	// These are already implemented
	void	DrawBresenhamCircle(int centre_x, int centre_y, int radius, const cColour &colour);
	void	DrawBresenhamCircleFilled(int centre_x, int centre_y, int radius, const cColour &colour);
	void	DrawBresenhamCircleOutline(int centre_x, int centre_y, int radius, const cColour &inner, const cColour &outer);
	void	DrawLine(float x1, float y1, float x2, float y2, const cColour &colour);
};


cCanvas *CreateCanvas(int width, int height);
void	DestroyCanvas(cCanvas *canvas_ptr);