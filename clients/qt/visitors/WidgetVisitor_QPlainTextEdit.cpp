#include "WidgetVisitor_QPlainTextEdit.hpp"
#include "WidgetVisitor.hpp"
#include "WidgetListener.hpp"
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

const QList<QString>& WidgetVisitorState_QPlainTextEdit::dataelements() const
{
	static const DataElements ar( "", 0);
	return ar;
}

void WidgetVisitorState_QPlainTextEdit::setState( const QVariant& state)
{
	if (state.isValid()) m_plainTextEdit->setPlainText( state.toString());
}

QVariant WidgetVisitorState_QPlainTextEdit::getState() const
{
	return QVariant( m_plainTextEdit->toPlainText());
}

