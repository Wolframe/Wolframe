#ifndef _VERSION_HPP_INCLUDED
#define _VERSION_HPP_INCLUDED

#include <string>

namespace _Wolframe {

	class Version {
	private:
		unsigned short	major_;
		unsigned short	minor_;
		unsigned short	revision_;
		bool		hasRevision_;
		unsigned	build_;
		bool		hasBuild_;
	public:
		Version();
		Version( unsigned short M, unsigned short m );
		Version( unsigned short M, unsigned short m, unsigned short r );
		Version( unsigned short M, unsigned short m, unsigned short r, unsigned b );

		unsigned short major()		{ return major_; }
		unsigned short minor()		{ return minor_; }
		unsigned short revision()	{ return revision_; }
		unsigned build()		{ return build_; }

		bool operator== ( const Version &other ) const;
		bool operator!= ( const Version &other ) const	{ return !( *this == other ); }

		bool operator> ( const Version &other ) const;
		bool operator>= ( const Version &other ) const	{ return !( *this < other ); }
		bool operator< ( const Version &other ) const	{ return ( other > *this ); }
		bool operator<= ( const Version &other ) const	{ return !( *this > other ); }

		std::string toString() const;
	};

} // namespace _Wolframe

#endif // _VERSION_HPP_INCLUDED
