#include "WidgetVisitorStateConstructor.hpp"
#include "visitors/WidgetVisitor_QComboBox.hpp"
#include "visitors/WidgetVisitor_QCheckBox.hpp"
#include "visitors/WidgetVisitor_QGroupBox.hpp"
#include "visitors/WidgetVisitor_QRadioButton.hpp"
#include "visitors/WidgetVisitor_QSpinBox.hpp"
#include "visitors/WidgetVisitor_QDoubleSpinBox.hpp"
#include "visitors/WidgetVisitor_QListWidget.hpp"
#include "visitors/WidgetVisitor_QTreeWidget.hpp"
#include "visitors/WidgetVisitor_QLineEdit.hpp"
#include "visitors/WidgetVisitor_QTextEdit.hpp"
#include "visitors/WidgetVisitor_QPlainTextEdit.hpp"
#include "visitors/WidgetVisitor_QDateEdit.hpp"
#include "visitors/WidgetVisitor_QTimeEdit.hpp"
#include "visitors/WidgetVisitor_QDateTimeEdit.hpp"
#include "visitors/WidgetVisitor_QTableWidget.hpp"
#include "visitors/WidgetVisitor_QSlider.hpp"
#include "visitors/WidgetVisitor_FileChooser.hpp"
#include "visitors/WidgetVisitor_PictureChooser.hpp"

typedef WidgetVisitor::StateR (*StateConstructor)( QWidget* widget);

struct WidgetVisitorTypeMap :QHash<QString,StateConstructor>
{
	template <class VisitorStateClass>
	static WidgetVisitor::StateR stateConstructor( QWidget* widget)
	{
		return WidgetVisitor::StateR( new VisitorStateClass( widget));
	}
	template <class VisitorStateClass>
	void addClass( const char* name)
	{
		(*this)[ QString( name)] = &stateConstructor<VisitorStateClass>;
	}
	#define ADD_WidgetVisitorType( NAME ) { addClass<WidgetVisitorState_ ## NAME>( #NAME); }
	WidgetVisitorTypeMap()
	{
		ADD_WidgetVisitorType( QComboBox)
		ADD_WidgetVisitorType( QCheckBox)
		ADD_WidgetVisitorType( QGroupBox)
		ADD_WidgetVisitorType( QRadioButton)
		ADD_WidgetVisitorType( QSpinBox)
		ADD_WidgetVisitorType( QDoubleSpinBox)
		ADD_WidgetVisitorType( QListWidget)
		ADD_WidgetVisitorType( QTreeWidget)
		ADD_WidgetVisitorType( QLineEdit)
		ADD_WidgetVisitorType( QTextEdit)
		ADD_WidgetVisitorType( QPlainTextEdit)
		ADD_WidgetVisitorType( QDateEdit)
		ADD_WidgetVisitorType( QTimeEdit)
		ADD_WidgetVisitorType( QDateTimeEdit)
		ADD_WidgetVisitorType( QTableWidget)
		ADD_WidgetVisitorType( QSlider)
		ADD_WidgetVisitorType( FileChooser)
		ADD_WidgetVisitorType( PictureChooser)
	}
};

WidgetVisitor::StateR createWidgetVisitorState( QWidget* widget)
{
	static WidgetVisitorTypeMap widgetVisitorTypeMap;
	QHash<QString,StateConstructor>::const_iterator wi = widgetVisitorTypeMap.find( widget->metaObject()->className());
	if (wi == widgetVisitorTypeMap.end())
	{
		return WidgetVisitor::StateR( new WidgetVisitor::State( widget));
	}
	else
	{
		return wi.value()( widget);
	}
}


