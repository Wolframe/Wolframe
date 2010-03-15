#ifndef _VERSION_HPP_INCLUDED
#define _VERSION_HPP_INCLUDED

#include <string>

namespace _SMERP {

	class Version {
	private:
		unsigned short	major_;
		unsigned short	minor_;
		unsigned short	revision_;
		unsigned	build_;
		std::string	verStr_;
	public:
		Version( unsigned short M, unsigned short m, unsigned short r = 0, unsigned b = 0 );

		unsigned short major()		{ return major_; };
		unsigned short minor()		{ return minor_; };
		unsigned short revision()	{ return revision_; };
		unsigned build()		{ return build_; };
		const std::string& toString()	{ return verStr_; };
	};

} // namespace _SMERP

#endif // _VERSION_HPP_INCLUDED
