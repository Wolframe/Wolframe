#include "WidgetVisitor_QPlainTextEdit.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QPlainTextEdit::WidgetVisitorState_QPlainTextEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_plainTextEdit(qobject_cast<QPlainTextEdit*>(widget_)){}

void WidgetVisitorState_QPlainTextEdit::clear()
{
	m_plainTextEdit->clear();
}

QVariant WidgetVisitorState_QPlainTextEdit::property( const QByteArray& name)
{
	if (strcmp( name, "") == 0)
	{
		return QVariant( m_plainTextEdit->toPlainText());
	}
	return QVariant();
}

bool WidgetVisitorState_QPlainTextEdit::setProperty( const QByteArray& name, const QVariant& data)
{
	if (strcmp( name, "") == 0)
	{
		m_plainTextEdit->setPlainText( data.toString());
		return true;
	}
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QPlainTextEdit::dataelements() const
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

