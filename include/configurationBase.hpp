//
// processorConfig.hpp
//

#ifndef _CONFIGURATION_BASE_HPP_INCLUDED
#define _CONFIGURATION_BASE_HPP_INCLUDED

#include <string>
#include <ostream>

#include <boost/property_tree/ptree.hpp>

//// forward declaration for boost::property_tree
//// we should make this work one day
//namespace boost	{
//	namespace property_tree	{
//		struct	ptree;
//	}
//}

namespace _Wolframe {
	namespace Configuration	{

		struct ConfigurationBase	{
		public:
			ConfigurationBase( const std::string& dispName )
								{ dispName_ = dispName; }

			const std::string& displayName() const	{ return dispName_; }

			virtual bool parse( const boost::property_tree::ptree& pt, const std::string& nodeName,
					    std::ostream& os ) = 0;

			virtual void setCanonicalPathes( const std::string& /* referencePath */ )	{}

			/// Check if the server configuration makes sense
			///
			/// Be aware that this function does NOT test if the configuration
			/// can be used. It only tests if it MAY be valid.
			virtual bool check( std::ostream& os ) const = 0;

			/// these functions are not implemented / implementable yet
			// virtual bool test( std::ostream& os ) const = 0;
			virtual bool test( std::ostream& os ) const
								{ os << "Not implemented yet !" << std::endl; return false; }

			///
			virtual void print( std::ostream& os ) const = 0;
		private:
			std::string	dispName_;
		};

	} // namespace Configuration
} // namespace _Wolframe

#endif // _CONFIGURATION_BASE_HPP_INCLUDED
