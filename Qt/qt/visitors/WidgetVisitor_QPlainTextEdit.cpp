#include "WidgetVisitor_QPlainTextEdit.hpp"
#include "WidgetVisitor.hpp"
#include "WidgetListener.hpp"
#include "WidgetEnabler.hpp"
#include <QDebug>

WidgetVisitorState_QPlainTextEdit::WidgetVisitorState_QPlainTextEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_plainTextEdit(qobject_cast<QPlainTextEdit*>(widget_)){}

void WidgetVisitorState_QPlainTextEdit::clear()
{
	m_plainTextEdit->clear();
}

QVariant WidgetVisitorState_QPlainTextEdit::property( const QString& name)
{
	if (name.isEmpty())
	{
		return QVariant( m_plainTextEdit->toPlainText());
	}
	return QVariant();
}

bool WidgetVisitorState_QPlainTextEdit::setProperty( const QString& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_plainTextEdit->setPlainText( data.toString());
		return true;
	}
	return false;
}

void WidgetVisitorState_QPlainTextEdit::setState( const QVariant& state)
{
	if (state.isValid()) m_plainTextEdit->setPlainText( state.toString());
}

QVariant WidgetVisitorState_QPlainTextEdit::getState() const
{
	return QVariant( m_plainTextEdit->toPlainText());
}

void WidgetVisitorState_QPlainTextEdit::connectDataSignals( WidgetVisitor::DataSignalType dt, WidgetListener& listener)
{
	switch (dt)
	{
		case WidgetVisitor::SigChanged:
			QObject::connect( m_plainTextEdit, SIGNAL( blockCountChanged( int)), &listener, SLOT( changed()), Qt::UniqueConnection);
			QObject::connect( m_plainTextEdit, SIGNAL( cursorPositionChanged()), &listener, SLOT( changed()), Qt::UniqueConnection);
			QObject::connect( m_plainTextEdit, SIGNAL( modificationChanged( bool)), &listener, SLOT( changed()), Qt::UniqueConnection);
			QObject::connect( m_plainTextEdit, SIGNAL( selectionChanged()), &listener, SLOT( changed()), Qt::UniqueConnection);
			QObject::connect( m_plainTextEdit, SIGNAL( textChanged()), &listener, SLOT( changed()), Qt::UniqueConnection);
			break;
		case WidgetVisitor::SigActivated:
		case WidgetVisitor::SigEntered:
		case WidgetVisitor::SigPressed:
		case WidgetVisitor::SigClicked:
		case WidgetVisitor::SigDoubleClicked:
			qCritical() << "try to connect to signal not provided" << m_plainTextEdit->metaObject()->className() << WidgetVisitor::dataSignalTypeName(dt);
	}
}

void WidgetVisitorState_QPlainTextEdit::connectWidgetEnabler( WidgetEnabler& enabler)
{
	QObject::connect( m_plainTextEdit, SIGNAL( blockCountChanged( int)), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_plainTextEdit, SIGNAL( cursorPositionChanged()), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_plainTextEdit, SIGNAL( modificationChanged( bool)), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_plainTextEdit, SIGNAL( selectionChanged()), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_plainTextEdit, SIGNAL( textChanged()), &enabler, SLOT( changed()), Qt::UniqueConnection);
}


