#include "WidgetVisitor_QSlider.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QSlider::WidgetVisitorState_QSlider( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_slider(qobject_cast<QSlider*>( widget_))
{
}

bool WidgetVisitorState_QSlider::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QSlider::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QSlider::clear()
{
}

QVariant WidgetVisitorState_QSlider::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QSlider::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QSlider::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QSlider::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QSlider::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QSlider::getState() const
{
	return QVariant();
}

