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

WidgetVisitor::StateR createWidgetVisitorState( QWidget* widget)
{
	QString clazz = widget->metaObject()->className();
	if (clazz == "QComboBox")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QComboBox( widget));
	}
	else if (clazz == "QCheckBox")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QCheckBox( widget));
	}
	else if (clazz == "QDoubleSpinBox")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QDoubleSpinBox( widget));
	}
	else if (clazz == "QSpinBox")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QSpinBox( widget));
	}
	else if (clazz == "QGroupBox")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QGroupBox( widget));
	}
	else if (clazz == "QRadioButton")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QRadioButton( widget));
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
	else if (clazz == "QPlainTextEdit")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QPlainTextEdit( widget));
	}
	else if (clazz == "QDateEdit")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QDateEdit( widget));
	}
	else if (clazz == "QTimeEdit")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTimeEdit( widget));
	}
	else if (clazz == "QDateTimeEdit")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QDateTimeEdit( widget));
	}
	else if (clazz == "QTableWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTableWidget( widget));
	}
	else if (clazz == "QTreeWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTreeWidget( widget));
	}
	else if (clazz == "QSlider")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QSlider( widget));
	}
	else if (clazz == "FileChooser")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_FileChooser( widget));
	}
	else if (clazz == "PictureChooser")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_PictureChooser( widget));
	}
	else
	{
		return WidgetVisitor::StateR( new WidgetVisitor::State( widget));
	}
}

