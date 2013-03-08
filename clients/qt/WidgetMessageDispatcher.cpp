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
#include "WidgetMessageDispatcher.hpp"
#include "WidgetRequest.hpp"
#include <QDebug>
#include <QAbstractButton>

static bool nodeProperty_hasRequestId( const QWidget* widget, const QByteArray& cond)
{
	QVariant requestid = widget->property( "_w_requestid");
	return (requestid.isValid() && requestid.toByteArray() == cond);
}

///\brief Return true if the widget is not an action widget.
//	in an action widget the doctype is associated with the request on action and not on domain load
static bool nodeProperty_isEnabledNonActionWidgetWithDoctype( const QWidget* widget, const QByteArray&)
{
	if (!widget->isEnabled()) return false;
	if (qobject_cast<const QAbstractButton*>( widget)) return false;
	QVariant property = widget->property( "doctype");
	return (property.isValid());
}

QList<WidgetMessageDispatcher::Request> WidgetMessageDispatcher::getDomainLoadRequests( bool debugmode)
{
	QList<Request> rt;
	foreach (WidgetVisitor visitor, m_visitor.findNodes( nodeProperty_isEnabledNonActionWidgetWithDoctype))
	{
		rt.push_back( Request( visitor.requestUID(), getWigdetRequest( visitor, debugmode)));
	}
	return rt;
}

bool WidgetMessageDispatcher::feedResult( const QByteArray& tag, const QByteArray& data)
{
	bool found = false;
	bool rt = true;
	foreach (WidgetVisitor visitor, m_visitor.findNodes( nodeProperty_hasRequestId, tag))
	{
		found = true;
		if (!setWidgetAnswer( visitor, data))
		{
			qCritical() << "Failed assign request answer tag:" << tag << "data:" << data;
			rt = false;
		}
	}
	if (!found)
	{
		qCritical() << "Request not found: tag=" << tag;
	}
	return rt;
}


