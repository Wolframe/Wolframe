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

#ifndef _FORM_CALL_HPP_INCLUDED
#define _FORM_CALL_HPP_INCLUDED
#include <QString>
#include <QByteArray>
#include <QPair>
#include <QVariant>
#include <QList>

///\class FormCall
///\brief Form call interpreted as string "formname?param1=...&param2=..."
//	commas ',' in the parameter definition are interpreted as list element separator for a variant list
//	single quotes "'" are marking string content that can contain the special characters '&' and '?' and comma ','
//	single quotes in strings are escaped as double single quotes
class FormCall
{
public:
	///\brief Constructor from definition
	explicit FormCall( const QString& callstr);
	///\brief Default constructor
	FormCall();

	void init( const QString& callstr);

	///\brief Form name to call
	const QString& name() const					{return m_name;}
	static QString name( const QString& callstr);

	typedef QPair<QString,QVariant> Parameter;
	///\brief List of parameters to pass to the form called
	const QList<Parameter>& parameter() const			{return m_parameter;}

private:
	QString m_name;				//< name of the form to load
	QList<Parameter> m_parameter;		//< list of parameters to set in the loaded form before initializing it with data
};

QList<QString> getFormCallProperties( const QString& callstr);

#endif


