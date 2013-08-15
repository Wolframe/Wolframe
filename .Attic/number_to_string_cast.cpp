namespace {
class NumTable
{
public:
	NumTable()
	{
		std::size_t ii=0,dd=0;
		for (; ii<100; ++ii,dd+=2)
		{
			tab.chr[dd+0] = (ii/10) + '0';
			tab.chr[dd+1] = (ii%10) + '0';
		}
	}

	struct Number64
	{
		union
		{
			char chr[22];
			short ar[11];
		} tab;
	};

	const char* getNumberString( boost::uint64_t val, Number64& str) const
	{
		std::size_t ii=0;
		if (val == 0) return "0";
		str.tab.chr[ 20] = '\0';
		for (; ii<10 && val>0; ++ii)
		{
			std::size_t ai = val % 100;
			val /= 100;

			str.tab.ar[ 9-ii] = tab.ar[ ai];
		}
		std::size_t ni = 20-(ii<<1);
		ni += (str.tab.chr[ ni] == '0');
		return str.tab.chr + ni;
	}

private:
	union
	{
		char chr[200];
		short ar[100];
	} tab;
};
}//anonymous namespace

static void get_number_string( std::string& rt, boost::uint64_t val)
{
	static const NumTable tab;
	NumTable::Number64 numstrbuf;
	rt.append( tab.getNumberString( val, numstrbuf));
}

static void get_number_string( std::string& rt, boost::int64_t val)
{
	static const NumTable tab;
	NumTable::Number64 numstrbuf;
	if (val < 0)
	{
		rt.push_back( '-');
		rt.append( tab.getNumberString( (boost::uint64_t)-val, numstrbuf));
	}
	else
	{
		rt.append( tab.getNumberString( (boost::uint64_t)val, numstrbuf));
	}
}

static std::string number_string_cast( boost::int64_t val)
{
	std::string rt;
	get_number_string( rt, val);
	return rt;
}

static std::string number_string_cast( boost::uint64_t val)
{
	std::string rt;
	get_number_string( rt, val);
	return rt;
}

