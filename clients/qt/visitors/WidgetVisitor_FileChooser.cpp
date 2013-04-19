#include "WidgetVisitor_FileChooser.hpp"
#include "WidgetVisitor.hpp"
#include <QFile>

WidgetVisitorState_FileChooser::WidgetVisitorState_FileChooser( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_mode(Init)
	,m_fileChooser(qobject_cast<FileChooser*>( widget_))
	,m_fileidx(-1)
{}

bool WidgetVisitorState_FileChooser::enter( const QString& name, bool writemode)
{
	switch (m_mode)
	{
		case Init:
			if (name == "file")
			{
				m_mode = File;
				if (!writemode)
				{
					if (m_fileidx < 0)
					{
						m_filenames = m_fileChooser->fileNames();
						m_fileidx = 0;
					}
					if (m_fileidx >= m_filenames.size())
					{
						m_filecontent.clear();
						m_filecontent_encoded.clear();
						m_mode = Init;
						return false;
					}
					else
					{
						QFile file( m_filenames.at( m_fileidx));
						file.open( QFile::ReadOnly);
						m_filecontent = file.readAll();
						m_filecontent_encoded =  m_filecontent.toBase64();
						file.close();
					}
				}
				return true;
			}
			break;
		case File:
			break;
	}
	return false;
}

bool WidgetVisitorState_FileChooser::leave( bool /*writemode*/)
{
	if (m_mode == File)
	{
		m_mode = Init;
		m_fileidx++;
		return true;
	}
	return false;
}

void WidgetVisitorState_FileChooser::clear()
{
	m_filenames.clear();
	m_fileChooser->setFileName( "" );
	m_filecontent.clear();
	m_filecontent_encoded.clear();
	m_fileidx = -1;
	m_mode = Init;
}

QVariant WidgetVisitorState_FileChooser::property( const QString& name)
{
	switch (m_mode)
	{
		case Init:
			break;
		case File:
			if (name == "name")
			{
				return QVariant( m_filenames.at( m_fileidx));
			}
			if (name == "size")
			{
				return QVariant( m_filecontent.size());
			}
			if (name.isEmpty())
			{
				return QVariant( m_filecontent_encoded);
			}
			break;
	}
	return QVariant();
}

bool WidgetVisitorState_FileChooser::setProperty( const QString& /*name*/, const QVariant& /*data*/)
{
	return false;
}

bool WidgetVisitorState_FileChooser::isArrayElement( const QString& name)
{
	if (name == "file") return true;
	return false;
}

void WidgetVisitorState_FileChooser::setState( const QVariant& /*state*/)
{
}

QVariant WidgetVisitorState_FileChooser::getState() const
{
	return QVariant();
}

