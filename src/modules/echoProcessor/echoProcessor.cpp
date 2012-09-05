/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// echo processor implementation
//

#include "logger-v1.hpp"
#include "echoProcessor.hpp"

#include "config/ConfigurationTree.hpp"
#include "config/valueParser.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>

namespace _Wolframe {

//***  Processor configuration  *****************************************
bool EchoProcConfig::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
			    const module::ModulesDirectory* /*modules*/ )
{
	bool retVal = true;
	bool timeoutDefined = false;
	bool opDefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "timeout" ))	{
			if ( !config::Parser::getValue( logPrefix().c_str(), *L1it, m_timeout ))
				retVal = false;
			timeoutDefined = true;
		}
		else if ( boost::algorithm::iequals( L1it->first, "defaultOp" ) ||
			  boost::algorithm::iequals( L1it->first, "defaultOperation" ))	{
			std::string	opName;
			if ( !config::Parser::getValue( logPrefix().c_str(), *L1it, opName ))
				retVal = false;
			if ( boost::algorithm::iequals( opName, "echo" ))	{
				opDefined = true;
				m_operation = EchoProcessorUnit::ECHO;
			}
			else if ( boost::algorithm::iequals( opName, "to_upper" ))	{
				opDefined = true;
				m_operation = EchoProcessorUnit::TO_UPPER;
			}
			else if ( boost::algorithm::iequals( opName, "to_lower" ))	{
				opDefined = true;
				m_operation = EchoProcessorUnit::TO_LOWER;
			}
			else
				MOD_LOG_WARNING << logPrefix() << "unknown default operation: '"
						<< opName << "'";
		}
		else	{
			MOD_LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	if ( ! timeoutDefined )
		m_timeout = 0;
	if ( ! opDefined )
		m_operation = EchoProcessorUnit::ECHO;

	return retVal;
}


bool EchoProcConfig::check() const
{
	return true;
}

void EchoProcConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << std::endl;
	if ( m_timeout > 0 )
		os << indStr << "   Idle timeout: " << m_timeout << "s" << std::endl;
	else
		os << indStr << "   Idle timeout: no timeout" << std::endl;
}

void EchoProcConfig::setCanonicalPathes( const std::string& /*refPath*/ )
{
}


//***  Processor unit container  ****************************************
EchoProcConstructor::EchoProcConstructor( const EchoProcConfig& conf )
{
	m_proc = new EchoProcessorUnit( conf.m_operation );
	MOD_LOG_TRACE << "Echo processor container created";
}


//***  Processor unit  **************************************************
EchoProcessorUnit::EchoProcessorUnit( Operation operation )
{
	m_operation = operation;

	const char *opName;
	switch( m_operation )	{
		case UNDEFINED:
			m_operation = ECHO;
			MOD_LOG_WARNING << "Echo processor operation is undefined. Setting operation to ECHO";
			 // break is intentionally missing
		case ECHO:
			opName = "echo";
			break;
		case TO_UPPER:
			opName = "to upper";
			break;
		case TO_LOWER:
			opName = "to lower";
			break;
		default:
			throw std::logic_error( "Echo Processor constructor: invalid operation" );
	}

	MOD_LOG_DEBUG << "Echo processor unit created with default operation: " << opName;
}


std::string& EchoProcessorUnit::process( std::string& input )
{
	switch( m_operation )	{
		case ECHO:
			break;
		case TO_UPPER:
			boost::algorithm::to_upper( input );
			break;
		case TO_LOWER:
			boost::algorithm::to_lower( input );
			break;
		case UNDEFINED:
		default:
			throw std::logic_error( "Echo Processor constructor: invalid operation" );
	}
	return input;
}

//***  Processor ********************************************************
void EchoProcessorSlice::receiveData( const void* /*data*/, std::size_t /*size*/ )
{
}

const FSM::Operation EchoProcessorSlice::nextOperation()
{
	FSM::Operation	op;
	return op;
}

void EchoProcessorSlice::signal(FSM::Signal /*event*/ )
{
}

std::size_t EchoProcessorSlice::dataLeft( const void*& /*begin*/ )
{
	return 0;
}

} // namespace _Wolframe

