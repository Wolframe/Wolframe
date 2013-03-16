#include "WidgetVisitor_QPlainTextEdit.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QPlainTextEdit::WidgetVisitorState_QPlainTextEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_plainTextEdit(qobject_cast<QPlainTextEdit*>( widget_))
{
}

bool WidgetVisitorState_QPlainTextEdit::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QPlainTextEdit::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QPlainTextEdit::clear()
{
}

QVariant WidgetVisitorState_QPlainTextEdit::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QPlainTextEdit::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QPlainTextEdit::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QPlainTextEdit::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QPlainTextEdit::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QPlainTextEdit::getState() const
{
	return QVariant();
}

