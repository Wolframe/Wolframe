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

/** Implementation Documentation:

1 Introduction
The WidgetVisitor iterates on a tree of widget nodes and their internal states.
It uses a stack of WidgetState references and a counter for the internal states
of widgets visited for keeping track of its position in the widget tree and its state.

2 Widget Stack
The widget stack grows with one element every time a widget references something across its own borders.
Such an element can be a child widget or a widget referenced by a symbolic link to another widget.

2.1 Widget stack example
For resolving a property
         'customertable.customer.address.name'
where address refers to a customer subwidget and name defines a symbolic link, we
have the following elements on the widget visitor stack (a very artificial example :-):

    [customertable 1]   (1 is the internal state of customertable after selection of customer)
    [address 0]
    [name 0]

We get to this state calling:
    WidgetVisitor visitor( <customertable widget>* )
    visitor.enter( "customer");
    visitor.enter( "address");

We can access the property name with:
    QVariant name = visitor.property( "name");

If we are just interested in the property only, we can also call instead:
    QVariant name = WidgetVisitor( <customertable widget>* ).property( "customer.address.name");

Internally happens the same.


3 Symbol Resolving Algorithm

3.1 Property Name Resolving
 [A] Check if a synonym is referenced and return its redirected property if yes
 [B] Check if an internal property of the widget is referenced return it if yes
 [C] Check if an multipart property is referenced and try to step into the substructures to return the property if yes (see "2.2")
 [D] Check if a dynamic property is referenced and return it if yes

3.2 Step Into Substructures
 [A] Check if name is a synonym and follow it if yes
 [B] Check if name refers to a widget internal item and follow it if yes
 [C] Check if name refers to a symbolic link and follow the link if yes
 [D] On top level check if name refers to an ancessor or an ancessor child and follow it if yes
 [E] Check if name refers to a child and follow it if yes


4. Dynamic Properties Used

4.1 Used Dynamic Property Prefixes of widgets
 'synonym:'              Rewrite rule for property "synonym:IDENTIFIER": look for the value if IDENTIFIER is selected with enter)
 'link:'                 Rewrite rule for property "link:IDENTIFIER": push the widget referenced as property value of "link:IDENTIFIER" if IDENTIFIER is selected with enter)
 'datasignal:IDENTIFIER' Defines a signal of type IDENTIFIER (domainchange,onload) with the destination slot identifer defined as property value of "datasignal:IDENTIFIER"
 'dataslot:IDENTIFIER'   Defines a slot for the signal of type IDENTIFIER (domainchange,onload) with the identifer defined as property value of "dataslot:IDENTIFIER"
 'assign:PROP'           Defines an assingment of property PROP to the property defined as value "assign:PROP" on an avent. Used to update linked values on signal
 '_w_'                   Prefix for a Wolframe internal property not of interest for the user

 4.2 Used Dynamic Properties of widgets
 'widgetid'              Unique identifier of the widget used for identifying it (resolving symbolic links, address of a request aswer)


5. Mapping XML to and from Widgets
The module WidgetRequest uses the visitors defined here to iterate on the widget structure
to set and get the elements in the XML of a request/answer or some other representation
of the widget data.

**/


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
		///\brief Sets the current node to the root widget or child with name 'name' of the root widget
		bool enter_root( const QByteArray& name);

		///\brief Set the current node to the parent that called enter to this node.
		void leave( bool writemode);

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

		///\brief Find all sub widgets matching a condition and return them as their visitor context
		typedef bool (*NodeProperty)( const QWidget* widget, const QByteArray& cond);
		QList<QWidget*> findSubNodes( NodeProperty prop, const QByteArray& cond=QByteArray()) const;

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

		///\enum DataSignalType
		///\brief Data signal type
		enum DataSignalType
		{
			OnLoad,
			DomainChange
		};
		enum {NofDataSignalTypes=(int)DomainChange+1};

		///\class State
		///\brief State on WidgetVisitor stack. Implemented for every widget type supported
		struct State
		{
			///\brief Constructor
			explicit State( QWidget* widget_);

			///\brief Copy constructor
			State( const State& o);

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
			///\brief Get all children widgets that have data elements not handled (owned) by this. default is all children
			virtual QList<QWidget*> datachildren() const				{return m_widget->findChildren<QWidget*>();}
			///\brief Evaluate if a dataelements is possibly addressing a list of elements, thus appearing more than once
			virtual bool isRepeatingDataElement( const QByteArray&/*name*/)		{return false;}
			///\brief Restore the widget state from a variable
			virtual void setState( const QVariant& /*state*/){}
			///\brief Get the current the widget state
			virtual QVariant getState()						{return QVariant();}

		public://Common methods:
			const QByteArray& getSynonym( const QByteArray& name) const;
			QByteArray getLink( const QByteArray& name) const;
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
			struct DataSignals
			{
				QList<QByteArray> id[(int)NofDataSignalTypes];
			};
			struct DataSlots
			{
				QList<QByteArray> id[(int)NofDataSignalTypes];
			};
			friend class WidgetVisitorStackElement;
			friend class WidgetVisitor;
			typedef QPair< QByteArray,QByteArray> LinkDef;
			typedef QPair< QByteArray,QByteArray> Assignment;

			QWidget* m_widget;							//< widget reference
			QHash<QByteArray,QByteArray> m_synonyms;				//< synonym name map
			QList<LinkDef> m_links;							//< symbolic links to other objects
			QList<Assignment> m_assignments;					//< assignment done at initialization and destruction
			DataSignals m_datasignals;						//< datasignals to emit on certain state changes
			DataSlots m_dataslots;							//< dataslot to declare a receiver by name for being informed on certain state changes
			QHash<QByteArray,QVariant> m_dynamicProperties;				//< map of defined dynamic properties
			int m_synonym_entercnt;							//< counter for how many stack elements to pop on a leave (for multipart synonyms)
			int m_internal_entercnt;						//< counter for calling State::leave() before removing stack elements
		};
		typedef QSharedPointer<State> StateR;

		///\brief Get a serialization of all visible widget elements in the current state
		QList<Element> elements();

		///\brief Get a serialization of a selecte list of widget elements in the current state
		///\param[in] selected_dataelements data elements selected by name
		QList<Element> elements( const QList<QByteArray>& selected_dataelements);

		///\brief Eval if id describes a widget id of an identifier
		///\param[in] id identifier to check
		///\return true if 'id' is a widget id
		static bool is_widgetid( const QByteArray& id);

		///\brief Get the unique identifier of the widget for server requests
		QByteArray widgetid() const;
		///\brief Get the widget of the current state
		QWidget* widget() const								{return m_stk.isEmpty()?0:m_stk.top()->m_widget;}
		///\brief Get the object name of the widget of the current state
		QByteArray objectName() const;
		///\brief Get the class name of the widget of the current state
		QByteArray className() const;

		///\brief Resolve reference to a variable
		///\param[in] value to resolve
		QVariant resolve( const QVariant& value);

		///\brief Get the UI root widget
		QWidget* uirootwidget() const;
		///\brief Get the UI root widget
		QWidget* predecessor( const QByteArray& name) const;

		///\brief Resolve a symbolic link to a widget
		///\param[in] link name of symbolic link to resolve
		QWidget* resolveLink( const QByteArray& link);

		///\brief Backup state description in a dynamic property and reset widget state
		void resetState();
		///\brief Restore the state from its description backup (resetState)
		void restoreState();

		///\brief Clear widget data
		void clear();

		///\brief Get all receivers of a datasignal (type)
		QList<QWidget*> get_datasignal_receivers( DataSignalType type);

		void readAssignments();
		void writeAssignments();
		///\brief Do assignments to form widgets based on assign: declarations
		void do_initInititalizations();
		///\brief Do assignments from form widgets based on assign: declarations
		void do_closeInititalizations();

	private:
		///\brief Internal property get using 'level' to check property resolving step (B).
		///\param[in] name name of the property
		///\param[in] level element index in path (element is first element of a path <=> level == 0)
		QVariant property( const QByteArray& name, int level);

		///\brief Internal property set using 'level' to check property resolving step (B).
		///\param[in] name name of the property
		///\param[in] value property value as variant (any type)
		///\param[in] level element index in path (element is first element of a path <=> level == 0)
		///\return true on success
		bool setProperty( const QByteArray& name, const QVariant& value, int level);

		///\brief Sets the current node to the child with name 'name'
		///\param[in] name name of the subnode
		///\param[in] writemode true if in write/create access and not in read access
		///\param[in] level element index in path (element is first element of a path <=> level == 0)
		bool enter( const QByteArray& name, bool writemode, int level);

		///\brief Internal implementation of 'elements(...)'
		QList<Element> elements( const QList<QByteArray>* selected_dataelements);

		///\brief Constructor internal
		WidgetVisitor( const StateR& state);
		///\brief Constructor internal
		WidgetVisitor( const QStack<StateR>& stk_);

		void ERROR( const char* msg, const QString& arg=QString()) const;
		void ERROR( const char* msg, const QByteArray& arg) const;

	private:
		QStack<StateR> m_stk;				//< stack of visited widget nodes (first) with their select state (second). The current node is the top element
};

#endif

