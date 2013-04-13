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
#include <QSharedPointer>

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

	const QString nodename( int idx) const			{return m_nodear.at(idx).name;}
	const QSharedPointer<DataTree>& nodevalue(int i) const	{return m_nodear.at(i).value;}
	bool isAttributeNode( int idx) const			{return (idx >= 0 && idx < m_nofattributes);}
	int size() const					{return m_nodear.size();}
	const QVariant& value() const				{return m_value;}
	ElementType elemtype() const				{return m_elemtype;}
	bool isValid() const					{return m_elemtype != Invalid;}
	bool isAttribute( int idx) const			{return idx<m_nofattributes;}

	static DataTree fromString( const QString::const_iterator& begin, const QString::const_iterator& end);
	static DataTree fromString( const QString& str);
	QString toString() const;

private:
	void setNodeValue( const QVariant& nodevalue);
	static QString parseString( QString::const_iterator& itr, const QString::const_iterator& end);
	static bool mapDataTreeToString( const DataTree& dt, QString& str);
	static bool mapDataValueToString( const QVariant& val, QString& str);
	static ElementType parseNodeHeader( QString& nodename, QString::const_iterator& itr, const QString::const_iterator& end);

private:
	struct Node
	{
		Node( const QString& name_, const DataTree& value_)
			:name(name_),value( new DataTree( value_)){}
		Node( const Node& o)
			:name(o.name),value(o.value){}
		Node(){}

		QString name;
		QSharedPointer<DataTree> value;
	};

	ElementType m_elemtype;
	QVariant m_value;
	QList<Node> m_nodear;
	int m_nofattributes;
};

class ActionDefinition
{
public:
	ActionDefinition( const QString& content);
	ActionDefinition( const ActionDefinition& o)
		:m_doctype(o.m_doctype),m_rootelement(o.m_rootelement),m_structure(o.m_structure){}
	QString toString() const;

	const QString& doctype() const		{return m_doctype;}
	const QString& rootelement() const	{return m_rootelement;}
	const DataTree& structure() const	{return m_structure;}
	bool isValid() const			{return m_structure.isValid();}

private:
	QString m_doctype;
	QString m_rootelement;
	DataTree m_structure;
};

#endif

