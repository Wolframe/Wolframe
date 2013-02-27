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

#ifndef _WIDGET_VISIOR_HPP_INCLUDED
#define _WIDGET_VISIOR_HPP_INCLUDED
#include <QWidget>
#include <QStack>
#include <QHash>
#include <QString>

typedef void* WidgetState;

///\class WidgetVisitor
///\brief Tree to access to (read/write) of widget data
class WidgetVisitor
{
	public:
		///\brief Constructor
		///\param[in] root Root of widget tree visited
		///\param[in] globals_ Reference to global variables
		WidgetVisitor( QWidget* root, QHash<QByteArray, QString>* globals_);

		///\brief Sets the current node to the child with name 'name'
		bool enter( const QString& name);
		///\brief Sets the current node to the child with name 'name'
		bool enter( const QByteArray& name);

		///\brief Set the current node to the parent that called enter to this node.
		void leave();

		/** Property name resolving process:
		*	(A) Split name to SELECT.REST and try to enter SELECT from the current node and eveluate there REST
		*	(B) Otherwise if first element of a path try to enter SELECT from the root node of the widget tree
		*	(C) Otherwise try to evaluate name as widget property of the current node
		*	(D) Otherwise try to evaluate it as a a dynamic property
		*	(E) Otherwise find a dynamic property synonym:name and redo evaluation with the synonym:name value as name
		*/
		/** Used dynamic property prefixes:
		*	'synonym:' Rewrite rule for property "synonym:IDENTIFIER": look for the value if IDENTIFIER is searched
		*	'global.' Global variable: Resolve 'global.IDENTIFIER' as lookup in the globals table passed as reference in the constructor
		*			Global variable references '{global.NAME}' are resolved once at widget initialization.
		*			Other variable references '{name}' are resolved whenever a property is read
		*/
		/** Rules:
		*	Structured widget property names (customer.id) refer allways to the last accessed main property (customer)
		*/
		///\brief Get the property of the current node by 'name'
		///\param[in] name name of the property
		///\return Property variant (any type)
		QVariant property( const QString& name);
		///\brief Get the property of the current node by 'name'
		///\param[in] name name of the property
		///\return property variant (any type)
		QVariant property( const QByteArray& name);

		///\brief Set the property of the current node
		///\param[in] name name of the property
		///\param[in] value property value as variant (any type)
		///\return true on success
		bool setProperty( const QByteArray& name, const QVariant& value);

		///\brief Set the property of the current node
		///\param[in] name name of the property
		///\param[in] value property value as variant (any type)
		///\return true on success
		bool setProperty( const QString& name, const QVariant& value);

	private:
		///\brief Internal property get using 'level' to check property resolving step (B).
		///\param[in] name name of the property
		///\param[in] level element index in path (element is first element of a path <=> level == 0)
		QVariant property( const QByteArray& name, int level);

		///\brief Internal property set using 'level' to check property resolving step (B).
		///\param[in] name name of the property
		///\param[in] value property value as variant (any type)
		///\return true on success
		bool setProperty( const QByteArray& name, const QVariant& value, int level);

		WidgetVisitor( QWidget* root, WidgetState state, QHash<QByteArray, QString>* globals_);

		enum {MaxIdentifierSize=0xFF};			//< internal maximum identifier length
		QStack<QPair<QWidget*,WidgetState> > m_stk;	//< stack of visited widget nodes (first) with their select state (second). The current node is the top element
		QHash<QByteArray, QString>* m_globals;		//< global variables
};

#endif

