#include "WidgetVisitor_PictureChooser.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_PictureChooser::WidgetVisitorState_PictureChooser( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_pictureChooser(qobject_cast<PictureChooser*>( widget_))
{
}

bool WidgetVisitorState_PictureChooser::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_PictureChooser::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_PictureChooser::clear()
{
}

QVariant WidgetVisitorState_PictureChooser::property( const QByteArray& /*name*/)
{
	return QVariant();
}

bool WidgetVisitorState_PictureChooser::setProperty( const QByteArray& /*name*/, const QVariant& /*data*/)
{
	return false;
}

const QList<QByteArray>& WidgetVisitorState_PictureChooser::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	return noDataElements;
}

bool WidgetVisitorState_PictureChooser::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_PictureChooser::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_PictureChooser::getState() const
{
	return QVariant();
}

