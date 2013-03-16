#include "WidgetVisitor_QRadioButton.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QRadioButton::WidgetVisitorState_QRadioButton( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_radioButton(qobject_cast<QRadioButton*>( widget_))
{
}

bool WidgetVisitorState_QRadioButton::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QRadioButton::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QRadioButton::clear()
{
}

QVariant WidgetVisitorState_QRadioButton::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QRadioButton::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QRadioButton::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QRadioButton::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QRadioButton::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QRadioButton::getState() const
{
	return QVariant();
}

