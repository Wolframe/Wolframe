#include "WidgetVisitor_QCheckBox.hpp"
#include "WidgetVisitor.hpp"
#include "WidgetListener.hpp"
#include <QDebug>

WidgetVisitorState_QCheckBox::WidgetVisitorState_QCheckBox( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_checkBox(qobject_cast<QCheckBox*>( widget_))
{
}

bool WidgetVisitorState_QCheckBox::enter( const QString& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QCheckBox::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QCheckBox::clear()
{
	m_checkBox->setChecked( false);
}

QVariant WidgetVisitorState_QCheckBox::property( const QString& name)
{
	if (name.isEmpty())
	{
		return QVariant( m_checkBox->isChecked());
	}
	return QVariant();
}

bool WidgetVisitorState_QCheckBox::setProperty( const QString& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_checkBox->setChecked( data.toBool());
	}
	return false;
}

const QList<QString>& WidgetVisitorState_QCheckBox::dataelements() const
{
	static const DataElements dataElements( "", 0);
	return dataElements;
}

bool WidgetVisitorState_QCheckBox::isRepeatingDataElement( const QString& /*name*/)
{
	return false;
}

void WidgetVisitorState_QCheckBox::setState( const QVariant& state)
{
	if (state.isValid()) m_checkBox->setCheckState( (Qt::CheckState)state.toInt());
}

QVariant WidgetVisitorState_QCheckBox::getState() const
{
	return QVariant( (int)m_checkBox->checkState());
}

void WidgetVisitorState_QCheckBox::connectDataSignals( WidgetVisitor::DataSignalType dt, WidgetListener& listener)
{
	switch (dt)
	{
		case WidgetVisitor::SigChanged:
			QObject::connect( m_checkBox, SIGNAL( clicked( bool)), &listener, SLOT( changed()));
			QObject::connect( m_checkBox, SIGNAL( released()), &listener, SLOT( changed()));
			QObject::connect( m_checkBox, SIGNAL( toggled( bool)), &listener, SLOT( changed()));
			break;
		case WidgetVisitor::SigPressed:
			QObject::connect( m_checkBox, SIGNAL( pressed()), &listener, SLOT( pressed())); break;
		case WidgetVisitor::SigActivated:
		case WidgetVisitor::SigEntered:
		case WidgetVisitor::SigClicked:
		case WidgetVisitor::SigDoubleClicked:
			qCritical() << "try to connect to signal not provided" << m_checkBox->metaObject()->className() << WidgetVisitor::dataSignalTypeName(dt);
	}
}


