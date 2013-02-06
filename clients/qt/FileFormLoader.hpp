/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

#ifndef _FILE_FORM_LOADER_HPP_INCLUDED
#define _FILE_FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>

#include "FormLoader.hpp"


	class FileFormLoader : public FormLoader
	{
		// intentionally omitting Q_OBJECT here, is done in FormLoader!
		
		public:
			FileFormLoader( QString formDir, QString localeDir, QString resourcesDir );
			virtual ~FileFormLoader( ) {};
			virtual void initiateListLoad( );
			virtual void initiateFormLoad( QString &name );
			virtual void initiateFormLocalizationLoad( QString &name, QLocale locale );
			virtual void initiateGetLanguageCodes( );

			virtual void initiateFormSave( QString name, QByteArray form );
			virtual void initiateFormLocalizationSave( QString name, QLocale locale, QByteArray localizationSrc, QByteArray localizationBin );
			
			virtual void initiateFormDelete( QString name );
			virtual void initiateFormLocalizationDelete( QString name, QLocale locale );
		
		private:
			void initialize( );
			
		private:
			QString m_formDir;
			QString m_localeDir;
			QString m_resourcesDir;
			
			QByteArray readFile( QString fileName );

	};

#endif // _FILE_FORM_LOADER_HPP_INCLUDED
