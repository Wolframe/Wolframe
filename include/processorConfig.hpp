//
// processorConfig.hpp
//

#ifndef _PROCESSOR_CONFIG_HPP_INCLUDED
#define _PROCESSOR_CONFIG_HPP_INCLUDED

#include <string>

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
		virtual bool check() = 0;
		virtual bool test() = 0;
		virtual void print( std::ostream& os ) const = 0;
	private:
		std::string	root_;
		std::string	dispStr_;
		std::string	errMsg_;
	};

} // namespace _SMERP

#endif // _PROCESSOR_CONFIG_HPP_INCLUDED
