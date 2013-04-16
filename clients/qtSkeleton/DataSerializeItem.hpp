/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
#ifndef _WIDGET_DATA_SERIALIZE_ITEM_HPP_INCLUDED
#define _WIDGET_DATA_SERIALIZE_ITEM_HPP_INCLUDED
#include <QVariant>
#include <QString>

///\class DataSerializeItem
///\brief DataSerializeItem of serialization of widget data (for constructing a server request out of the widget data)
class DataSerializeItem
{
public:
	enum Type {OpenTag,CloseTag,Attribute,Value};
	static const char* typeName( Type i)
	{
		static const char* ar[] = {"OpenTag","CloseTag","Attribute","Value"};
		return ar[i];
	}
	Type type() const		{return m_type;}
	const QVariant& value() const	{return m_value;}

	DataSerializeItem( Type type_, const QVariant& value_)
		:m_type(type_),m_value(value_){}
	DataSerializeItem( const DataSerializeItem& o)
		:m_type(o.m_type),m_value(o.m_value){}

	///\brief Get the element as string for debugging output
	QString toString() const
	{
		QString rt( typeName( m_type));
		rt.append( " '");
		rt.append( m_value.toString());
		rt.append( "'");
		return rt;
	}
private:
	Type m_type;
	QVariant m_value;
};

#endif
