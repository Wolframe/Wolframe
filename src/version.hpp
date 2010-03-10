#ifndef _VERSION_HPP_INCLUDED
#define _VERSION_HPP_INCLUDED

#include <string>

namespace _SMERP {

	class version {
	private:
		unsigned short	major_;
		unsigned short	minor_;
		unsigned short	revision_;
		unsigned	build_;
		std::string	verStr;
	public:
		version( unsigned short M, unsigned short m );
		version( unsigned short M, unsigned short m, unsigned short r );
		version( unsigned short M, unsigned short m, unsigned short r, unsigned b );

		unsigned short major()		{ return major_; };
		unsigned short minor()		{ return minor_; };
		unsigned short revision()	{ return revision_; };
		unsigned build()		{ return build_; };
		const std::string& string()	{ return verStr; };
	};

} // namespace _SMERP

#endif // _VERSION_HPP_INCLUDED
