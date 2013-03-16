#include "WidgetVisitor_QGroupBox.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QGroupBox::WidgetVisitorState_QGroupBox( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_groupBox(qobject_cast<QGroupBox*>( widget_))
{
}

bool WidgetVisitorState_QGroupBox::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QGroupBox::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QGroupBox::clear()
{
}

QVariant WidgetVisitorState_QGroupBox::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_QGroupBox::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QGroupBox::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_QGroupBox::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QGroupBox::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_QGroupBox::getState() const
{
	return QVariant();
}

