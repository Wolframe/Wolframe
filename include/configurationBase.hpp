//
// processorConfig.hpp
//

#ifndef _CONFIGURATION_BASE_HPP_INCLUDED
#define _CONFIGURATION_BASE_HPP_INCLUDED

#include <string>
#include <list>

namespace _SMERP {

	// forward declaration for boost::property_tree
	class	ptree;

	struct ConfigurationBase	{
	public:
		ConfigurationBase( std::string rt, std::string ds )
							{ root_ = rt; dispStr_ = ds; }
		std::string& root()			{ return root_; }
		std::string& displayStr()		{ return dispStr_; }

		std::string& errMsg()			{ return errMsg_; }

		virtual bool parse( ptree& pt ) = 0;
		virtual bool check( std::ostream& os ) const = 0;
		virtual bool test( std::ostream& os ) const = 0;
		virtual void print( std::ostream& os ) const = 0;
	private:
		std::string	root_;
		std::string	dispStr_;
		std::string	errMsg_;
	};

} // namespace _SMERP

#endif // _CONFIGURATION_BASE_HPP_INCLUDED
