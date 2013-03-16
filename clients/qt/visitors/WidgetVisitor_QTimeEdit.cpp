#include "WidgetVisitor_QTimeEdit.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QTimeEdit::WidgetVisitorState_QTimeEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_timeEdit(qobject_cast<QTimeEdit*>( widget_))
{
}

bool WidgetVisitorState_QTimeEdit::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QTimeEdit::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QTimeEdit::clear()
{
}

QVariant WidgetVisitorState_QTimeEdit::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QTimeEdit::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QTimeEdit::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QTimeEdit::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QTimeEdit::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QTimeEdit::getState() const
{
	return QVariant();
}

