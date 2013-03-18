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
	m_radioButton->setChecked( false);
}

QVariant WidgetVisitorState_QRadioButton::property( const QByteArray& name)
{
	if (name.isEmpty())
	{
		return QVariant( m_radioButton->isChecked());
	}
	return QVariant();
}

bool WidgetVisitorState_QRadioButton::setProperty( const QByteArray& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_radioButton->setChecked( data.toBool());
	}
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QRadioButton::dataelements() const
{
	static const DataElements dataElements( "");
	return dataElements;
}

bool WidgetVisitorState_QRadioButton::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QRadioButton::setState( const QVariant& state)
{
	if (state.isValid()) m_radioButton->setChecked( state.toBool());
}

QVariant WidgetVisitorState_QRadioButton::getState() const
{
	return QVariant( m_radioButton->isChecked());
}


