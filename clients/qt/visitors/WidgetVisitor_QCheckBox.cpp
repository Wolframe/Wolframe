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
	m_checkBox->setChecked( false);
}

QVariant WidgetVisitorState_QCheckBox::property( const QByteArray& name)
{
	if (name.isEmpty())
	{
		return QVariant( m_checkBox->isChecked());
	}
	return QVariant();
}

bool WidgetVisitorState_QCheckBox::setProperty( const QByteArray& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_checkBox->setChecked( data.toBool());
	}
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QCheckBox::dataelements() const
{
	static const DataElements dataElements( "");
	return dataElements;
}

bool WidgetVisitorState_QCheckBox::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QCheckBox::setState( const QVariant& state)
{
	if (state.isValid()) m_checkBox->setCheckState( (Qt::CheckState)state.toInt());
}

QVariant WidgetVisitorState_QCheckBox::getState() const
{
	return QVariant( (int)m_checkBox->checkState());
}


