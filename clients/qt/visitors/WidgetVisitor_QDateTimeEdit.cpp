#include "WidgetVisitor_QDateTimeEdit.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QDateTimeEdit::WidgetVisitorState_QDateTimeEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_dateTimeEdit(qobject_cast<QDateTimeEdit*>( widget_))
{
}

bool WidgetVisitorState_QDateTimeEdit::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QDateTimeEdit::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QDateTimeEdit::clear()
{
}

QVariant WidgetVisitorState_QDateTimeEdit::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QDateTimeEdit::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QDateTimeEdit::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QDateTimeEdit::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QDateTimeEdit::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QDateTimeEdit::getState() const
{
	return QVariant();
}

