#include "WidgetVisitor_PictureChooser.hpp"
#include "WidgetVisitor.hpp"
#include "WidgetEnabler.hpp"

WidgetVisitorState_PictureChooser::WidgetVisitorState_PictureChooser( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_pictureChooser(qobject_cast<PictureChooser*>( widget_))
{
}

bool WidgetVisitorState_PictureChooser::enter( const QString& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_PictureChooser::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_PictureChooser::clear()
{
	m_pictureChooser->setFileName( "");
}

QVariant WidgetVisitorState_PictureChooser::property( const QString& name)
{
	if (name == "filename")
	{
		return QVariant( m_pictureChooser->fileName());
	}
	if (name == "size")
	{
		return QVariant( m_pictureChooser->picture().size());
	}
	if (name == "base64")
	{
		return QVariant( m_pictureChooser->picture().toBase64());
	}
	return QVariant();
}

bool WidgetVisitorState_PictureChooser::setProperty( const QString& name, const QVariant& data)
{
	if (name == "filename")
	{
		m_pictureChooser->setFileName( data.toString());
	}
	else if (name == "base64")
	{
		m_pictureChooser->setPicture( QByteArray::fromBase64( data.toByteArray()));
	}
	return false;
}

void WidgetVisitorState_PictureChooser::setState( const QVariant& state)
{
	if (state.isValid())
	{
		m_pictureChooser->setFileName( state.toString());
	}
}

QVariant WidgetVisitorState_PictureChooser::getState() const
{
	return QVariant( QVariant( m_pictureChooser->fileName()));
}

void WidgetVisitorState_PictureChooser::connectWidgetEnabler( WidgetEnabler& enabler)
{
	QObject::connect( m_pictureChooser, SIGNAL( changed()), &enabler, SLOT( changed()), Qt::UniqueConnection);
}

