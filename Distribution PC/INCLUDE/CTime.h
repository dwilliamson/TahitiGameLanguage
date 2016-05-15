
#ifndef	_INCLUDED_CTIME_H
#define	_INCLUDED_CTIME_H


class CTime
{
public:
	// Current year as a number
	uWORD		Year;

	// Current month, Jan = 1, Feb = 2, etc...
	uWORD		Month;

	// Current day of the week, Sun = 0, Mon = 1, etc...
	uWORD		DayOfWeek;

	// Current day of the month
	uWORD		Day;

	// Current hour
	uWORD		Hour;

	// Current minute
	uWORD		Minute;

	// Current second
	uWORD		Second;

	// Current milli-second
	uWORD		Millisecond;

	int			operator == (CTime &time)
	{
		// Compare each member
		return ((Year == time.Year) && (Month == time.Month) && (Day == time.Day) &&
			(DayOfWeek == time.DayOfWeek) && (Hour == time.Hour) &&
			(Minute == time.Minute) && (Second == time.Second) &&
			(Millisecond == time.Millisecond));
	}
};


#endif	/* _INCLUDED_CTIME_H */