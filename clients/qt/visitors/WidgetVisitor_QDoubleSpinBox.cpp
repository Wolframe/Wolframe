#include "WidgetVisitor_QDoubleSpinBox.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QDoubleSpinBox::WidgetVisitorState_QDoubleSpinBox( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_doubleSpinBox(qobject_cast<QDoubleSpinBox*>( widget_))
{
}

bool WidgetVisitorState_QDoubleSpinBox::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QDoubleSpinBox::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QDoubleSpinBox::clear()
{
}

QVariant WidgetVisitorState_QDoubleSpinBox::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QDoubleSpinBox::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QDoubleSpinBox::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QDoubleSpinBox::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QDoubleSpinBox::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QDoubleSpinBox::getState() const
{
	return QVariant();
}

