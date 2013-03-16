#include "WidgetVisitor_QSpinBox.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QSpinBox::WidgetVisitorState_QSpinBox( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_spinBox(qobject_cast<QSpinBox*>( widget_))
{
}

bool WidgetVisitorState_QSpinBox::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QSpinBox::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QSpinBox::clear()
{
}

QVariant WidgetVisitorState_QSpinBox::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QSpinBox::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QSpinBox::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QSpinBox::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QSpinBox::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QSpinBox::getState() const
{
	return QVariant();
}

