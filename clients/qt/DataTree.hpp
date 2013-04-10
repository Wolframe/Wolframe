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
#ifndef _WIDGET_DATATREE_HPP_INCLUDED
#define _WIDGET_DATATREE_HPP_INCLUDED
#include <QString>
#include <QVariant>
#include <QPair>
#include <QList>

///\class DataTree
///\brief Defines a tree of data items
class DataTree
{
public:
	enum ElementType {Invalid,Single,Array};

	explicit DataTree( ElementType elemtype_=Invalid)
		:m_elemtype(elemtype_),m_nofattributes(0){}
	DataTree( const DataTree& o)
		:m_elemtype(o.m_elemtype),m_value(o.m_value),m_nodear(o.m_nodear),m_nofattributes(o.m_nofattributes){}
	DataTree( const QVariant& value_)
		:m_elemtype(Single),m_value(value_),m_nofattributes(0){}

	void addNode( const QString& name_, const DataTree& value_);
	void addAttribute( const QString& name_, const DataTree& value_);
	void addArrayElement( const DataTree& value_);
	void setNode( const QString& name_, const DataTree& value_);
	const DataTree& node( const QString& name_) const;
	const QVariant& value() const;
	bool isValid() const;

	static DataTree fromString( const QString::const_iterator& begin, const QString::const_iterator& end);
	static DataTree fromString( const QString& str);
	QString toString() const;

private:
	void setNodeValue( const QVariant& nodevalue);
	static void skipBrk( QString::const_iterator& itr, const QString::const_iterator& end);
	static QString parseString( QString::const_iterator& itr, const QString::const_iterator& end);

private:
	struct Node
	{
		Node( const QString& name_, const DataTree& value_)
			:name(name_),value( new DataTree( value_)){}
		Node( const Node& o)
			:name(o.name),value( new DataTree( *o.value)){}
		Node(){}

		QString name;
		DataTree* value;
	};

	ElementType m_elemtype;
	QVariant m_value;
	QList<Node> m_nodear;
	int m_nofattributes;
};

#endif

