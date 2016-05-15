
#ifndef	_INCLUDED_VEHICLE_H
#define	_INCLUDED_VEHICLE_H


class Vehicle
{
	vm_DECLARE_EXPORTED_CLASS(Vehicle);

public:
	Vehicle(void);

	void	Draw(void);

private:
	// Exported methods
	void	SetDrawPosition(int x, int y);
	void	SetDrawVelocity(int x, int y);
	float	sin(float angle);
	float	cos(float angle);
	float	Random(void);
	float	Distance(float x1, float y1, float x2, float y2);
	void	PrintFloat(float value);
	void	PrintString(char *string);

	enum { DRAW_RADIUS = 10, RECT_LEN = 30 };

	// Parameters needed to draw the vehicle
	int		m_DrawPos[2];
	int		m_OldDrawPos[2];
	int		m_DrawVel[2];
};


#endif	/* _INCLUDED_VEHICLE_H */