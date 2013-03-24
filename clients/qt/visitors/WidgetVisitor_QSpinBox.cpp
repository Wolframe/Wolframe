#include "WidgetVisitor_QSpinBox.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QSpinBox::WidgetVisitorState_QSpinBox( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_spinBox(qobject_cast<QSpinBox*>( widget_))
{
}

bool WidgetVisitorState_QSpinBox::enter( const QString& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QSpinBox::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QSpinBox::clear()
{
	// set to value between minimum and maximum
	int midval = (m_spinBox->minimum() + m_spinBox->maximum()) / 2;
	int stp = midval / m_spinBox->singleStep();
	double initval = m_spinBox->minimum() + stp * m_spinBox->singleStep();
	m_spinBox->setValue( initval);
}

QVariant WidgetVisitorState_QSpinBox::property( const QString& name)
{
	if (name.isEmpty())
	{
		return m_spinBox->value();
	}
	return QVariant();
}

bool WidgetVisitorState_QSpinBox::setProperty( const QString& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_spinBox->setValue( data.toInt());
	}
	return false;
}

const QList<QString>& WidgetVisitorState_QSpinBox::dataelements() const
{
	static const DataElements dataElements( "");
	return dataElements;
}

bool WidgetVisitorState_QSpinBox::isRepeatingDataElement( const QString& /*name*/)
{
	return false;
}

void WidgetVisitorState_QSpinBox::setState( const QVariant& state)
{
	if (state.isValid()) m_spinBox->setValue( state.toDouble());
}

QVariant WidgetVisitorState_QSpinBox::getState() const
{
	return QVariant( m_spinBox->value());
}

