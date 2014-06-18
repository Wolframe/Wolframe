/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file config/valueParser.hpp
/// \brief Parser for the configuration tokens

#ifndef _Wolframe_CONFIG_VALUE_PARSER_HPP_INCLUDED
#define _Wolframe_CONFIG_VALUE_PARSER_HPP_INCLUDED

#include "logger-v1.hpp"
#include "types/propertyTree.hpp"
#include <stdexcept>
#include <boost/type_traits.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/utility/value_init.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/utility.hpp>
#include <boost/limits.hpp>
#include <string>
#include <ostream>
#include <utility>

namespace _Wolframe {
namespace config {

namespace traits {
template <typename T> typename boost::enable_if<boost::is_enum<T>, T>::type increment( const T& i) { return (T)((int)i + 1); }
template <typename T> typename boost::disable_if<boost::is_enum<T>, T>::type increment( const T& i) { T rt = i; return ++rt; }
}//namespace


/// \class Parser
/// \brief Configuration parser for parsing the tokens of the configuration
class Parser
{
public:
	typedef std::string string;

	/// \class BaseTypeDomain
	/// \brief Describes a domain without value restriction except the one given by the C++ value type itself
	//
	//  All domains have to implement a check(ValueType&,string&) and a parse(ValueType&,const string&,string&) method
	//
	struct BaseTypeDomain
	{
		/// \brief Constructor
		BaseTypeDomain(){}

		/// \brief Checks if the configuration value is in the expected domain
		/// \tparam ValueType type of the value to check
		template <typename ValueType>
		bool check( const ValueType&, string&) const
		{
			return true;
		}

		/// \brief Parses the token string and maps it to its value representation
		/// \tparam ValueType type of the value to get parsed
		/// \param[out] value parsed value returned
		/// \param[in] token token string to parse
		/// \param[out] explanation error string that will be part of the log message
		template <typename ValueType>
		bool parse( ValueType& value, const string& token, string& explanation) const
		{
			using boost::bad_lexical_cast;
			try
			{
				value = boost::lexical_cast<ValueType>( token);
			}
			catch (bad_lexical_cast& e)
			{
				explanation = e.what();
				return false;
			};
			return true;
		}
	};

	/// \class RangeDomain
	/// \brief Describes a value domain for a range of values
	/// \tparam DomainValueType type of the value the range is formed of
	template <typename DomainValueType>
	struct RangeDomain :public BaseTypeDomain
	{
		/// \brief Constructor
		/// \param[in] min_ minimal allowed value of the range
		/// \param[in] max_ maximal allowed value of the range
		RangeDomain( const DomainValueType& min_, const DomainValueType& max_=std::numeric_limits<DomainValueType>::max()) :m_min(min_),m_max(max_){}

		/// \brief Get the description of the set of values allowed as string for the error log message
		string getDomainDescription() const
		{
			string rt( "[ ");
			rt.append( boost::lexical_cast<string>(m_min));
			rt.append( ", ");
			rt.append( boost::lexical_cast<string>(m_max));
			rt.append( " ]");
			return rt;
		}

		/// \brief Checks if the configuration value is in the expected domain
		/// \tparam ValueType type of the value to check
		/// \param[in] val value to check, if its in this domain
		/// \param[out] explanation part of the error message in case of error explaining why it failed
		template <typename ValueType>
		bool check( const ValueType& val, string& explanation) const
		{
			if (val < m_min || val > m_max)
			{
				explanation = "value out of range, expected value in the range of ";
				explanation.append( getDomainDescription());
				return false;
			}
			return true;
		}
	private:
		DomainValueType m_min;		//< minimal allowed value of the range
		DomainValueType m_max;		//< maximal allowed value of the range
	};

	/// \class RangeDomain
	/// \brief Describes a value domain for a value that must differ from its default (empty) initialization
	/// \tparam DomainValueType type of the value the range is formed of
	template <typename DomainValueType>
	struct NonEmptyDomain :public BaseTypeDomain
	{
		/// \brief constructor
		NonEmptyDomain(){}

		/// \brief Checks if the configuration value is in the expected domain, e.g. non empty
		/// \tparam ValueType type of the value to check
		/// \param[in] val value to check, if its in this domain
		/// \param[out] explanation part of the error message in case of error explaining why it failed
		template <typename ValueType>
		bool check( const ValueType& val, string& explanation) const
		{
			ValueType empty = boost::value_initialized<ValueType>();
			if (val == empty)
			{
				explanation = "value expected to be other than '";
				explanation.append( boost::lexical_cast<std::string>( empty));
				explanation.append( "'");
				return false;
			}
			return true;
		}
	};

	/// \class EnumDomain
	/// \brief Describes a value domain for an enumerable fixed set of values
	struct EnumDomain :public BaseTypeDomain
	{
		/// \brief Constructor
		/// \param nofelems number of elements in the enumeration
		/// \param elems string representation of the elements in the enumeration
		EnumDomain( unsigned int nofelems, const char** elems) :m_size(nofelems),m_ar(elems){}

		/// \brief Get the description of the set of values allowed as string for the error log message
		string getDomainDescription() const
		{
			string rt( "{");
			unsigned int ii;
			for (ii=0; ii<m_size; ii++)
			{
				if (ii>0) rt.append( ", ");
				rt.append( "'");
				rt.append( boost::lexical_cast<string>(m_ar[ii]));
				rt.append( "'");
			}
			rt.append( "}");
			return rt;
		}

		/// \brief Parses the token string and maps it to its value representation
		/// \tparam ValueType type of the value to get parsed
		/// \param[out] value parsed value returned
		/// \param[in] token token string to parse
		/// \param[out] explanation error string that will be part of the log message
		template <typename ValueType>
		bool parse( ValueType& value, const string& token, string& explanation) const
		{
			unsigned int ii;
			value = boost::value_initialized<ValueType>();

			for (ii=0; ii<m_size; ii++, value=traits::increment(value))
			{
				if (boost::algorithm::iequals( token, m_ar[ii]))
				{
					return true;
				}
			}
			explanation = "must be one of ";
			explanation.append( getDomainDescription());
			return false;
		}
	private:
		unsigned int m_size;	//< number of elements in the enumeration
		const char** m_ar;	//< string representation of the elements in the enumeration
	};

	/// \class BoolDomain
	/// \brief Describes the value domain of a boolean value as enumeration of all possible values
	struct BoolDomain :public EnumDomain
	{
	private:
		enum {NofBooleanEnum=10};

		/// \brief returns the accepted string representations of a boolean
		/// \return enumeration of valid boolean values
		static const char** booleanEnum()
		{
			static const char* ar[ NofBooleanEnum ] = {"false", "true", "0", "1", "off", "on", "no", "yes", "deny", "allow"};
			return ar;
		}
		/// \brief Returns the boolean value of an accepted token
		/// \param booleanEnumIdx index in booleanEnum()
		static bool getBooleanValue( unsigned int booleanEnumIdx)
		{
			return (booleanEnumIdx & 0x01);
		}

	public:
		/// \brief Constructor
		BoolDomain() :EnumDomain( NofBooleanEnum, booleanEnum()){}

		/// \brief Parses a boolean value
		/// \param[out] value parsed value returned
		/// \param[in] token token string to parse
		/// \param[out] explanation error string that will be part of the log message
		bool parse( bool& value, const string& token, string& explanation) const
		{
			unsigned int enumval = 0;
			bool rt = EnumDomain::parse( enumval, token, explanation);
			value = getBooleanValue( enumval);
			return rt;
		}
	};

	/// \brief Get the value of a configration token with a domain restriction that is checked
	/// \tparam Value type of the returned value
	/// \tparam Domain domain of the returned value to check
	/// \param[in] module identifier of the current configuration scope
	/// \param[in] name name of the element in the configuration
	/// \param[in] token string value of the element in the configuration
	/// \param[out] value returned value of the token
	/// \param[in] domain domain of the parsed value
	/// \param[in,out] isDefined (optional) flag that is set when the value is defined.
	//  If the flag is set when the method is called an error message is logged and the command fails.
	/// \return bool true, if success, else false
	template <typename Value, class Domain>
	static bool getValue( const char* module, const char* name, const string& token, Value& value, const Domain& domain, bool* isDefined=0)
	{
		using std::bad_alloc;
		using std::exception;
		try
		{
			if (isDefined)
			{
				if (*isDefined)
				{
					LOG_ERROR << loggingScope(module) << "duplicate definition of configuration element '" << name << "'";
					return false;
				}
			}
			string errorExplanation;
			if (domain.parse( value, token, errorExplanation) && domain.check( value, errorExplanation))
			{
				if (isDefined) *isDefined = true;
				return true;
			}
			LOG_ERROR << loggingScope(module) << "invalid value '" << token << "' for configuration element '" << name << "' (" << errorExplanation << ")";
			return false;
		}
		catch (bad_alloc&)
		{
			LOG_ERROR << loggingScope(module) << "out of memory when parsing configuration element '" << name << "'";
			return false;
		}
		catch (exception& e)
		{
			LOG_ERROR << loggingScope(module) << "illegal value for configuration element '" << name << "' (" << e.what() << ")";
			return false;
		}
	}

	/// \brief Get the value of a configration token without additional domain restriction
	/// \tparam Value type of the returned value
	/// \param[in] module identifier of the current configuration scope
	/// \param[in] name name of the element in the configuration
	/// \param[in] token string value of the element in the configuration
	/// \param[out] value returned value of the token
	/// \param[in,out] isDefined (optional) flag that is set when the value is defined.
	//  If the flag is set when the method is called an error message is logged and the command fails.
	/// \return bool true, if success, else false
	template <typename Value>
	static bool getValue( const char* module, const char* name, const string& token, Value& value, bool* isDefined=0)
	{
		return getValue<Value,BaseTypeDomain>( module, name, token, value, BaseTypeDomain(), isDefined);
	}

	/// \brief Get the value of a configration token with a domain restriction that is checked
	/// \tparam Value type of the returned value
	/// \tparam Domain domain of the returned value to check
	/// \param[in] module identifier of the current configuration scope
	/// \param[in] decl name,token tuple representing an element definition in the configuration
	//   See definition of getValue(const char*,const std::pair<const std::string,const std::string>&,Value&,const Domain&,bool*).
	//   Only difference is that we convert the decl structure from a property tree element and check if it is a name value assignement.
	/// \param[out] value returned value of the token
	/// \param[in] domain domain of the parsed value
	/// \param[in,out] isDefined (optional) flag that is set when the value is defined.
	//  If the flag is set when the method is called an error message is logged and the command fails.
	/// \return bool true, if success, else false
	template <typename Value, class Domain>
	static bool getValue( const char* module, const std::pair<const std::string, const types::PropertyTree::Node>& decl, Value& value, const Domain& domain, bool* isDefined=0)
	{
		try
		{
			if (decl.second.begin() != decl.second.end())
			{
				LOG_ERROR << loggingScope(module) << "atomic value expected for configuration element '" << decl.first.c_str() << "'";
				return false;
			}
			return getValue( module, decl.first.c_str(), decl.second.data(), value, domain, isDefined);
		}
		catch (std::bad_alloc&)
		{
			LOG_ERROR << loggingScope(module) << "out of memory when parsing configuration element '" << decl.first.c_str() << "'";
			return false;
		};
	}

	/// \brief Get the value of a configration token without additional domain restriction
	/// \tparam Value type of the returned value
	/// \param[in] module identifier of the current configuration scope
	/// \param[in] decl name,token tuple representing an element definition in the configuration
	//   See definition of getValue(const char*,const std::pair<const std::string,const std::string>&,Value&,bool*).
	//   Only difference is that we convert the decl structure from a property tree element and check if it is a name value assignement.
	/// \param[out] value returned value of the token
	/// \param[in,out] isDefined (optional) flag that is set when the value is defined.
	//  If the flag is set when the method is called an error message is logged and the command fails.
	/// \return bool true, if success, else false
	template <typename Value>
	static bool getValue( const char* module, const std::pair<const std::string, const types::PropertyTree::Node>& decl, Value& value, bool* isDefined=0)
	{
		return getValue<Value,BaseTypeDomain>( module, decl, value, BaseTypeDomain(), isDefined);
	}

private:
	/// \brief Return the header of a configuration parse error message issued, specifying the definition scope of the variable value parsed
	/// \param[in] scope scope identifier
	static const std::string loggingScope( const char* scope)
	{
		std::string rt;
		if (scope && *scope)
		{
			rt.append( scope);
			rt.append( ": ");
		}
		return rt;
	}
};

}} // namespace _Wolframe::config

#endif // _Wolframe_CONFIG_VALUE_PARSER_HPP_INCLUDED
