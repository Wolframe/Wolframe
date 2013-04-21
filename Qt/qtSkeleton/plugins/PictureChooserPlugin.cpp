#include "PictureChooser.hpp"
#include "PictureChooserPlugin.hpp"

#include <QtPlugin>

PictureChooserPlugin::PictureChooserPlugin(QObject *parent)
	: QObject(parent)
{
	m_initialized = false;
}

void PictureChooserPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
	if (m_initialized)
		return;

	// Add extension registrations, etc. here

	m_initialized = true;
}

bool PictureChooserPlugin::isInitialized() const
{
	return m_initialized;
}

QWidget *PictureChooserPlugin::createWidget(QWidget *parent)
{
	return new pictureChooser(parent);
}

QString PictureChooserPlugin::name() const
{
	return QLatin1String("pictureChooser");
}

QString PictureChooserPlugin::group() const
{
	return QLatin1String("");
}

QIcon PictureChooserPlugin::icon() const
{
	return QIcon();
}

QString PictureChooserPlugin::toolTip() const
{
	return QLatin1String("");
}

QString PictureChooserPlugin::whatsThis() const
{
	return QLatin1String("");
}

bool PictureChooserPlugin::isContainer() const
{
	return false;
}

QString PictureChooserPlugin::domXml() const
{
	return QLatin1String("<widget class=\"pictureChooser\" name=\"pictureChooser\">\n</widget>\n");
}

QString PictureChooserPlugin::includeFile() const
{
	return QLatin1String("PictureChooser.hpp");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PictureChooserPlugin, PictureChooserPlugin)
#endif // QT_VERSION < 0x050000
