#include "WidgetVisitor_QTimeEdit.hpp"
#include "WidgetVisitor.hpp"
#include "WidgetListener.hpp"
#include "WidgetEnabler.hpp"
#include <QSignalMapper>
#include <QWidget>
#include <QDebug>

WidgetVisitorState_QTimeEdit::WidgetVisitorState_QTimeEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_timeEdit(qobject_cast<QTimeEdit*>( widget_))
{
	m_timeEdit->setTime( QTime::currentTime());
}

bool WidgetVisitorState_QTimeEdit::enter( const QString& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QTimeEdit::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QTimeEdit::clear()
{
	m_timeEdit->setTime( QTime::currentTime());
}

QVariant WidgetVisitorState_QTimeEdit::property( const QString& name)
{
	if (name.isEmpty())
	{
		QVariant( m_timeEdit->time().toString( Qt::ISODate));
	}
	return QVariant();
}

bool WidgetVisitorState_QTimeEdit::setProperty( const QString& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_timeEdit->setTime( QTime::fromString( data.toString(), Qt::ISODate));
	}
	return false;
}

void WidgetVisitorState_QTimeEdit::setState( const QVariant& state)
{
	if (state.isValid()) m_timeEdit->setTime( state.toTime());
}

QVariant WidgetVisitorState_QTimeEdit::getState() const
{
	return QVariant( m_timeEdit->time());
}

void WidgetVisitorState_QTimeEdit::connectDataSignals( WidgetVisitor::DataSignalType dt, WidgetListener& listener)
{
	switch (dt)
	{
		case WidgetVisitor::SigChanged:
			QObject::connect( (const QObject*)m_timeEdit, SIGNAL( timeChanged( const QDate&)), (const QObject*)&listener, SLOT( changed()), Qt::UniqueConnection); break;
		case WidgetVisitor::SigActivated:
		case WidgetVisitor::SigEntered:
		case WidgetVisitor::SigPressed:
		case WidgetVisitor::SigClicked:
		case WidgetVisitor::SigDoubleClicked:
			qCritical() << "try to connect to signal not provided" << m_timeEdit->metaObject()->className() << WidgetVisitor::dataSignalTypeName(dt);
			break;
	}
}

void WidgetVisitorState_QTimeEdit::connectWidgetEnabler( WidgetEnabler& enabler)
{
	QObject::connect( (const QObject*)m_timeEdit, SIGNAL( timeChanged( const QDate&)), (const QObject*)&enabler, SLOT( changed()), Qt::UniqueConnection);
}


