#include "WidgetVisitor_QGroupBox.hpp"
#include "WidgetVisitor.hpp"
#include "WidgetListener.hpp"
#include <QDebug>
#include <QAbstractButton>
#include <QRadioButton>
#include <QCheckBox>

WidgetVisitorState_QGroupBox::WidgetVisitorState_QGroupBox( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_groupBox(qobject_cast<QGroupBox*>( widget_))
	,m_mode(Init)
{}

bool WidgetVisitorState_QGroupBox::setChecked( const QString& name)
{
	if (m_childmap.size() == 0)
	{
		foreach (QAbstractButton* child, m_groupBox->findChildren<QAbstractButton*>())
		{
			m_childmap[ child->objectName().toAscii()] = child;
		}
	}
	QHash<QString,QWidget*>::const_iterator ci = m_childmap.find( name);
	if (ci == m_childmap.end()) return false;

	QRadioButton* radioButton = qobject_cast<QRadioButton*>( ci.value());
	if (radioButton)
	{
		radioButton->setChecked( true);
		return true;
	}
	QCheckBox* checkBox = qobject_cast<QCheckBox*>( ci.value());
	if (checkBox)
	{
		checkBox->setChecked( true);
		return true;
	}
	return false;
}

void WidgetVisitorState_QGroupBox::setAllUnchecked()
{
	foreach (QAbstractButton* child, m_groupBox->findChildren<QAbstractButton*>())
	{
		QRadioButton* radioButton = qobject_cast<QRadioButton*>( child);
		if (radioButton)
		{
			radioButton->setChecked( false);
		}
		QCheckBox* checkBox = qobject_cast<QCheckBox*>( child);
		if (checkBox)
		{
			checkBox->setChecked( false);
		}
	}
}

QList<QVariant> WidgetVisitorState_QGroupBox::checkedList() const
{
	QList<QVariant> rt;
	foreach (QAbstractButton* child, m_groupBox->findChildren<QAbstractButton*>())
	{
		if (child->isChecked()) rt.push_back( QVariant( child->objectName().toAscii()));
	}
	return rt;
}

bool WidgetVisitorState_QGroupBox::enter( const QString& name, bool writemode)
{
	if (writemode && m_mode == Init && name == "selected")
	{
		m_mode = Selected;
		return true;
	}
	return false;
}

bool WidgetVisitorState_QGroupBox::leave( bool writemode)
{
	if (writemode && m_mode == Selected)
	{
		m_mode = Init;
		return true;
	}
	return false;
}

void WidgetVisitorState_QGroupBox::clear()
{
	setAllUnchecked();
	m_childmap.clear();
}

QVariant WidgetVisitorState_QGroupBox::property( const QString& name)
{
	if (name == "selected")
	{
		return QVariant( checkedList());
	}
	return QVariant();
}

bool WidgetVisitorState_QGroupBox::setProperty( const QString& name, const QVariant& data)
{
	if (m_mode == Selected && name.isEmpty())
	{
		return setChecked( data.toByteArray());
	}
	return false;
}

const QList<QString>& WidgetVisitorState_QGroupBox::dataelements() const
{
	static const DataElements dataElements( "selected");
	return dataElements;
}

QList<QWidget*> WidgetVisitorState_QGroupBox::datachildren() const
{
	QList<QWidget*> rt;
	foreach (QWidget* ww, m_groupBox->findChildren<QWidget*>())
	{
		if (!qobject_cast<QAbstractButton*>( ww)) rt.push_back( ww);
	}
	return rt;
}

bool WidgetVisitorState_QGroupBox::isRepeatingDataElement( const QString& /*name*/)
{
	return false;
}

void WidgetVisitorState_QGroupBox::setState( const QVariant& state)
{
	foreach (const QVariant& name, state.toList())
	{
		if (!setChecked( name.toByteArray()))
		{
			qCritical() << "set checked failed for QGroupBox object" << name.toByteArray();
		}
	}
}

QVariant WidgetVisitorState_QGroupBox::getState() const
{
	return QVariant( checkedList());
}

