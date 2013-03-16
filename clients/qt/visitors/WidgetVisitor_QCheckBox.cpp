#include "WidgetVisitor_QCheckBox.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QCheckBox::WidgetVisitorState_QCheckBox( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_checkBox(qobject_cast<QCheckBox*>( widget_))
{
}

bool WidgetVisitorState_QCheckBox::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QCheckBox::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QCheckBox::clear()
{
}

QVariant WidgetVisitorState_QCheckBox::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QCheckBox::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QCheckBox::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QCheckBox::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QCheckBox::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QCheckBox::getState() const
{
	return QVariant();
}

