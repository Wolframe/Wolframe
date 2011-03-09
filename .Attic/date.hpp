//
// date manipulation - safty classes
//

class Year	{
public:
	explicit Year( int year ) : year_( year )	{}
	operator int() const				{ return year_; }
private:
	int year_;
};

class Month	{
	explicit Month( int month ) : month_( month )	{}
	operator int() const				{ return month_; }
private:
	int month_;
};

class Day	{
	explicit Day( int day ) : day_( day )		{}
	operator int() const				{ return day_; }
private:
	int day_;
};

class Date {
public:
	Date( Year year, Month month, Day day );
	Date( Month month, Day day, Year year );
	Date( Day day, Month month, Year year );
}
