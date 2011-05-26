#include <boost/property_tree/ptree.hpp>
#include <cstddef>

#include "config/structParser.hpp"
#include "config/descriptionBase.hpp"

using namespace _Wolframe::config;

bool DescriptionBase::parse( void* configStruct, const boost::property_tree::ptree& pt, std::string& errmsg) const
{
	try
	{
		try
		{
			unsigned int ii,nn=m_ar.size();
			for (ii=0; ii<nn; ii++)
			{
				boost::property_tree::ptree::const_iterator end = pt.end();
				for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != end; ++it)
				{
					if (boost::iequals( it->first, m_ar[ii].m_name))
					{
						m_ar[ii].m_parse( m_ar[ii].m_name.c_str(), configStruct, m_ar[ii].m_ofs, it->second);
					}
				}
			}
			return true;
		}
		catch (const ParseError& e)
		{
			errmsg.clear();
			errmsg.append( "parse error in configuration at element ");
			errmsg.append( e.m_location);
			errmsg.append( ": '");
			errmsg.append( e.m_message);
			errmsg.append( "'");
		}
	}
	catch (const std::exception& e)
	{
		errmsg = e.what();
	}
	return false;
}

