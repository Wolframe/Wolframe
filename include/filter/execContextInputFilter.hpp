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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file filter/execContextInputFilter.hpp
/// \brief Execution context elements as input filter
#ifndef _WOLFRAME_FILTER_EXEC_CONTEXT_INPUT_FILTER_HPP_INCLUDED
#define _WOLFRAME_FILTER_EXEC_CONTEXT_INPUT_FILTER_HPP_INCLUDED
#include "processor/execContext.hpp"
#include "filter/typedfilter.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

struct ExecContextElement
{
	enum Value
	{
		Const,
		UserName,
		SocketId,
		RemoteHost,
		ConnectionType,
		Authenticator,
		Resource
	};
	enum {MaxValue=(int)Resource};

	static const char* valueName( Value v)
	{
		static const char* ar[] = {"Const","UserName","SocketId","RemoteHost","ConnectionType","Authenticator","Resource"};
		return ar[v];
	}
	static Value valueFromId( const std::string& v);

	std::string name;		///< name of the parameter
	Value value;			///< value taken from execution context
	std::string const_value;	///< value in case of const
	char value_delim;		///< delimiter in case of array access (0 else)
	std::size_t value_idx;		///< index >= 1 in case of array access (0 else)

	ExecContextElement()
		:value(Const),value_delim(0),value_idx(0){}
	ExecContextElement( const ExecContextElement& o)
		:name(o.name),value(o.value),const_value(o.const_value),value_delim(o.value_delim),value_idx(o.value_idx){}
	ExecContextElement( const std::string& name_, const std::string& const_value_)
		:name(name_),value(Const),const_value(const_value_),value_delim(0),value_idx(0){}
	ExecContextElement( const std::string& name_, Value value_)
		:name(name_),value(value_),value_delim(0),value_idx(0){}
	ExecContextElement( const std::string& name_, Value value_, char value_delim_, std::size_t value_idx_)
		:name(name_),value(value_),value_delim(value_delim_),value_idx(value_idx_){}
};

class ExecContextInputFilter
	:public langbind::TypedInputFilter
{
public:
	explicit ExecContextInputFilter( const std::vector<ExecContextElement>& params_, const proc::ExecContext& ctx_, const std::string& authorizationResource_);
	ExecContextInputFilter( const ExecContextInputFilter& o);
	virtual ~ExecContextInputFilter(){}

	virtual langbind::TypedInputFilter* copy() const
	{
		return new ExecContextInputFilter(*this);
	}

	const std::string& getElement( const std::string& ar, char delim, std::size_t idx);

	virtual bool getNext( ElementType& type, types::VariantConst& element);

	virtual void resetIterator();

private:
	int m_state;
	std::size_t m_paramidx;
	const std::vector<ExecContextElement>* m_params;
	const proc::ExecContext* m_ctx;
	std::string m_authorizationResource;
	std::string m_elembuf;
};

}}
#endif

