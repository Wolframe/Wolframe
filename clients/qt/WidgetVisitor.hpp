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
#include <QSharedPointer>
#include <QVariant>

///\class WidgetVisitor
///\brief Tree to access to (read/write) of widget data
class WidgetVisitor
{
	public:
		///\brief Default constructor
		WidgetVisitor(){}

		///\brief Constructor
		///\param[in] root Root of widget tree visited
		explicit WidgetVisitor( QWidget* root);

		///\brief Copy constructor
		///\param[in] o object to copy
		WidgetVisitor( const WidgetVisitor& o)
			:m_stk(o.m_stk){}

		///\brief Sets the current node to the child with name 'name'
		bool enter( const QString& name, bool writemode);
		///\brief Sets the current node to the child with name 'name'
		bool enter( const QByteArray& name, bool writemode);
		///\brief Return the addressed visitor of this with name as root element
		WidgetVisitor getRootElement( const QByteArray& name);

		///\brief Set the current node to the parent that called enter to this node.
		void leave( bool writemode);

		/** Property name resolving process:
		*	(A) Split name to SELECT.REST and try to enter SELECT from the current node and eveluate there REST
		*	(B) Otherwise if first element of a path try to enter SELECT from the root node of the widget tree
		*	(C) Otherwise try to evaluate name as widget property of the current node
		*	(D) Otherwise try to evaluate it as a a dynamic property
		*	(E) Otherwise find a dynamic property synonym:name and redo evaluation with the synonym:name value as name
		*/
		/** Used dynamic property prefixes:
		*	'synonym:' Rewrite rule for property "synonym:IDENTIFIER": look for the value if IDENTIFIER is searched
		*/
		///\brief Get the property of the current node by 'name'
		///\param[in] name name of the property
		///\return Property variant (any type)
		QVariant property( const QString& name);
		///\brief Get the property of the current node by 'name'
		///\param[in] name name of the property
		///\return property variant (any type)
		QVariant property( const QByteArray& name);
		///\brief Get the property of the current node by 'name'
		///\param[in] name name of the property
		///\return property variant (any type)
		QVariant property( const char* name);

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
		///\brief Set the property of the current node
		///\param[in] name name of the property
		///\param[in] value property value as variant (any type)
		///\return true on success
		bool setProperty( const char* name, const QVariant& value);

		///\brief Find all widgets matching a condition and return them as their visitor context
		typedef bool (*NodeProperty)( const QWidget* widget, const QByteArray& cond);
		QList<WidgetVisitor> findNodes( NodeProperty prop, const QByteArray& cond=QByteArray());

		///\brief Find a dedicated widget and return its visitor object relative to this
		WidgetVisitor getSubWidgetVisitor( const QWidget* subwidget) const;

		///\class Element
		///\brief Element of serialization of widget data (for constructing a server request out of the widget data)
		class Element
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

			Element( Type type_, const QVariant& value_)
				:m_type(type_),m_value(value_){}
			Element( const Element& o)
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

		///\class State
		///\brief State on WidgetVisitor stack. Implemented for every widget type supported
		struct State
		{
			///\brief Constructor
			explicit State( QWidget* widget_);

			///\brief Copy constructor
			State( const State& o)
				:m_widget(o.m_widget),m_synonyms(o.m_synonyms),m_dynamicProperties(o.m_dynamicProperties),m_entercnt(o.m_entercnt){}
			///\brief Destructor
			virtual ~State(){}

		public://Interface methods implemented for different widget types:
			///\brief Clear contents of the widget
			virtual void clear(){}
			///\brief Switch to a substructure context inside the widget but not to a child widget
			virtual bool enter( const QByteArray& /*name*/, bool /*writemode*/)	{return false;}
			///\brief Leave the current substructure context an switch to ist (enter) ancessor
			virtual bool leave( bool /*writemode*/)					{return false;}
			///\brief Get a property or set of properties of the current substructure context addressed by name
			virtual QVariant property( const QByteArray&)				{return QVariant();}
			///\brief Set a property of the current substructure context addressed by name
			virtual bool setProperty( const QByteArray&, const QVariant&)		{return false;}
			///\brief Get all dataelements readable in the current substructure context
			virtual const QList<QByteArray>& dataelements() const			{static const QList<QByteArray> ar; return ar;}
			///\brief Evaluate if a dataelements is possibly addressing a list of elements, thus appearing more than once
			virtual bool isRepeatingDataElement( const QByteArray&/*name*/)		{return false;}
			///\brief Restore the widget state from a variable
			virtual void setState( const QVariant& /*state*/){}
			///\brief Get the current the widget state
			virtual QVariant getState()						{return QVariant();}

		public://Common methods:
			const QByteArray& getSynonym( const QByteArray& name) const;
			QVariant dynamicProperty( const QByteArray& name) const;
			bool setDynamicProperty( const QByteArray&, const QVariant& value);

		protected:
			///\class DataElements
			///\brief Constructor helper for State::dataelements()
			struct DataElements :public QList<QByteArray>
			{
				DataElements( const char* initializer, ...);			//< constructor from 0 terminated vararg initializer list
			};

		private:
			friend class WidgetVisitorStackElement;
			friend class WidgetVisitor;
			QWidget* m_widget;							//< widget reference
			QHash<QByteArray,QByteArray> m_synonyms;				//< synonym name map
			QHash<QByteArray,QVariant> m_dynamicProperties;				//< map of defined dynamic properties
			int m_entercnt;								//< counter for leaving multipart synonyms
		};
		typedef QSharedPointer<State> StateR;

		///\brief Get a serialization of all visible widget elements in the current state
		QList<Element> elements();

		///\brief Get a serialization of a selecte list of widget elements in the current state
		///\param[in] selected_dataelements data elements selected by name
		QList<Element> elements( const QList<QByteArray>& selected_dataelements);

		///\brief Get the unique identifier of the widget for server requests (construct one if not defined as dynamic property yet)
		QByteArray requestUID();
		///\brief Get the widget of the current state
		QWidget* widget() const								{return m_stk.isEmpty()?0:m_stk.top()->m_widget;}
		///\brief Get the object name of the widget of the current state
		QByteArray objectName() const;
		///\brief Get the class name of the widget of the current state
		QByteArray className() const;

		///\brief Resolve reference to a variable
		///\param[in] value to resolve
		QVariant resolve( const QVariant& value);

		///\brief Backup state description in a dynamic property and reset widget state
		void resetState();
		///\brief Restore the state from its description backup (resetState)
		void restoreState();

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

		///\brief Internal implementation of 'elements(...)'
		QList<Element> elements( const QList<QByteArray>* selected_dataelements);

		///\brief Constructor internal
		WidgetVisitor( const StateR& state);
		///\brief Constructor internal
		WidgetVisitor( const QStack<StateR>& stk_);

	private:
		QStack<StateR> m_stk;				//< stack of visited widget nodes (first) with their select state (second). The current node is the top element
};

#endif

