#include "WidgetVisitorStateConstructor.hpp"
#include "visitors/WidgetVisitor_QComboBox.hpp"
#include "visitors/WidgetVisitor_QListWidget.hpp"
#include "visitors/WidgetVisitor_QTreeWidget.hpp"
#include "visitors/WidgetVisitor_QLineEdit.hpp"
#include "visitors/WidgetVisitor_QTextEdit.hpp"
#include "visitors/WidgetVisitor_QTableWidget.hpp"
#include "visitors/WidgetVisitor_FileChooser.hpp"

WidgetVisitor::StateR createWidgetVisitorState( QWidget* widget)
{
	QString clazz = widget->metaObject()->className();
	if (clazz == "QComboBox")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QComboBox( widget));
	}
	else if (clazz == "QListWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QListWidget( widget));
	}
	else if (clazz == "QLineEdit")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QLineEdit( widget));
	}
	else if (clazz == "QTextEdit")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTextEdit( widget));
	}
	else if (clazz == "QTableWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTableWidget( widget));
	}
	else if (clazz == "QTreeWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTreeWidget( widget));
	}
	else if (clazz == "FileChooser")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_FileChooser( widget));
	}
	else
	{
		return WidgetVisitor::StateR( new WidgetVisitor::State( widget));
	}
}

