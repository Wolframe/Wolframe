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

#ifndef _WOLFRAME_WIDGET_MESSAGE_DISPATCHER_HPP_INCLUDED
#define _WOLFRAME_WIDGET_MESSAGE_DISPATCHER_HPP_INCLUDED
#include "WidgetVisitor.hpp"

///\class WidgetMessageDispatcher
///\brief Structure to initialize widgets of a form and issue commands as client/server requests
class WidgetMessageDispatcher
{
	public:
		///\brief Constructor
		///\param[in] root Root of widget tree visited
		WidgetMessageDispatcher( QWidget* formwidget)
			:m_visitor( formwidget){}
		WidgetMessageDispatcher( const WidgetVisitor& visitor_)
			:m_visitor( visitor_){}

		///\brief Copy constructor
		///\param[in] o object to copy
		WidgetMessageDispatcher( const WidgetMessageDispatcher& o)
			:m_visitor(o.m_visitor){}

		struct Request
		{
			QString tag;
			QByteArray content;

			Request( QString tag_, QByteArray content_)
				:tag(tag_),content(content_){}
			Request( const Request& o)
				:tag(o.tag),content(o.content){}
		};

		QList<Request> getDomainLoadRequests( bool debugmode=false);
		Request getDomainLoadRequest( bool debugmode=false);
		Request getFormActionRequest( bool debugmode=false);
		QList<QWidget*> findRecipients( const QString& tag) const;

	private:
		WidgetVisitor m_visitor;			//< visitor of elements
};

#endif

