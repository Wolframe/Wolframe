#include "WidgetVisitor_FileChooser.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_FileChooser::WidgetVisitorState_FileChooser( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_fileChooser(qobject_cast<FileChooser*>( widget_))
{
}

bool WidgetVisitorState_FileChooser::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_FileChooser::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_FileChooser::clear()
{
}

QVariant WidgetVisitorState_FileChooser::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_FileChooser::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_FileChooser::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_FileChooser::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_FileChooser::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_FileChooser::getState() const
{
	return QVariant();
}

