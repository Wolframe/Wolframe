#include "WidgetVisitor_QSlider.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QSlider::WidgetVisitorState_QSlider( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_slider(qobject_cast<QSlider*>( widget_))
{
}

bool WidgetVisitorState_QSlider::enter( const QString& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QSlider::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QSlider::clear()
{
	// set to value between minimum and maximum
	int midval = (m_slider->minimum() + m_slider->maximum()) / 2;
	int stp = midval / m_slider->singleStep();
	double initval = m_slider->minimum() + stp * m_slider->singleStep();
	m_slider->setValue( initval);
}

QVariant WidgetVisitorState_QSlider::property( const QString& name)
{
	if (name.isEmpty())
	{
		return m_slider->value();
	}
	return QVariant();
}

bool WidgetVisitorState_QSlider::setProperty( const QString& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_slider->setValue( data.toInt());
	}
	return false;
}

const QList<QString>& WidgetVisitorState_QSlider::dataelements() const
{
	static const DataElements dataElements( "");
	return dataElements;
}

bool WidgetVisitorState_QSlider::isRepeatingDataElement( const QString& /*name*/)
{
	return false;
}

void WidgetVisitorState_QSlider::setState( const QVariant& state)
{
	if (state.isValid()) m_slider->setValue( state.toDouble());
}

QVariant WidgetVisitorState_QSlider::getState() const
{
	return QVariant( m_slider->value());
}

