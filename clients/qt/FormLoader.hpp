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

#ifndef _FORM_LOADER_HPP_INCLUDED
#define _FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QLocale>


	class FormLoader : public QObject
	{	
		Q_OBJECT
		
		public:
			virtual ~FormLoader( ) {};
			virtual void initiateListLoad( ) = 0;
			virtual void initiateFormLoad( QString &name ) = 0;
			virtual void initiateFormLocalizationLoad( QString &name, QLocale locale ) = 0;
			virtual void initiateGetLanguageCodes( ) = 0;
			
			virtual void initiateFormSave( QString name, QByteArray form ) = 0;
			virtual void initiateFormLocalizationSave( QString name, QLocale locale, QByteArray localizationSrc, QByteArray localizationBin ) = 0;
			
			virtual void initiateFormDelete( QString name ) = 0;
			virtual void initiateFormLocalizationDelete( QString name, QLocale locale ) = 0;

		Q_SIGNALS:
			void formListLoaded( QStringList forms );
			void formLoaded( QString form_name, QByteArray form );
			void formLocalizationLoaded( QString form_name, QByteArray localization );
			void languageCodesLoaded( QStringList languages );
			
			void formSaved( QString name );
			void formLocalizationSaved( QString name );
			
			void formDeleted( QString name );
			void formLocalizationDeleted( QString name );
	};

#endif // _FORM_LOADER_HPP_INCLUDED
