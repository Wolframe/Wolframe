/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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

#ifndef _PREFERENCES_DIALOG_HPP_INCLUDED
#define _PREFERENCES_DIALOG_HPP_INCLUDED

#include <QDialog>

#include "settings.hpp"

#include "ui_PreferencesDialog.h"
#include "ui_PreferencesDialogDeveloper.h"
#include "ui_PreferencesDialogInterface.h"

class PreferencesDialogInterface : public QWidget, public Ui::PreferencesDialogInterface
{
	Q_OBJECT
	
	public:
		PreferencesDialogInterface( QWidget *_parent = 0 );
};

class PreferencesDialogDeveloper : public QWidget, public Ui::PreferencesDialogDeveloper
{
	Q_OBJECT
	
	public:
		PreferencesDialogDeveloper( QWidget *_parent = 0 );
};

class PreferencesDialog : public QDialog, public Ui::PreferencesDialog
{
	Q_OBJECT
		
	public:
		PreferencesDialog( ApplicationSettings &settings, QStringList _languages, QWidget *_parent = 0 );
	
	private:
		ApplicationSettings &m_settings;
		QStringList m_languages;
		PreferencesDialogInterface *m_interface;
		PreferencesDialogDeveloper *m_developer;
		
	private:
		void initialize( );
		void loadSettings( );
		
	private slots:
		void toggleLoadMode( bool checked );
		void toggleLocale( bool checked );
		void restoreDefaults( );
		void apply( );
		void cancel( );
};


#endif // _PREFERENCES_DIALOG_HPP_INCLUDED
