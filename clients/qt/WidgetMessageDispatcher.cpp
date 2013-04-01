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

static bool nodeProperty_hasWidgetId( const QWidget* widget, const QVariant& cond)
{
	QVariant widgetid = widget->property( "widgetid");
	return (widgetid.isValid() && widgetid == cond);
}

///\brief Return true if the widget is not an action widget with a doctype defined.
//	in an action widget the doctype is associated with the request on action and not on domain load
static bool nodeProperty_isEnabledNonActionWidgetWithDoctype( const QWidget* widget, const QVariant&)
{
	if (!widget->isEnabled()) return false;
	if (qobject_cast<const QAbstractButton*>( widget)) return false;
	QVariant property = widget->property( "doctype");
	return (property.isValid());
}

QList<WidgetMessageDispatcher::Request> WidgetMessageDispatcher::getDomainLoadRequests( bool debugmode)
{
	QList<Request> rt;
	foreach (QWidget* widget, m_visitor.findSubNodes( nodeProperty_isEnabledNonActionWidgetWithDoctype))
	{
		WidgetVisitor visitor( widget);
		rt.push_back( Request( visitor.widgetid(), getWigdetRequest( visitor, debugmode)));
	}
	int nn = rt.size()/2;
	for (int kk = 0; kk < nn; kk++) rt.swap( kk, rt.size()-(1+kk));
	// ... requests are issued in reverse order (elements in sub nodes first)
	return rt;
}

WidgetMessageDispatcher::Request WidgetMessageDispatcher::getDomainLoadRequest( bool debugmode)
{
	return Request( m_visitor.widgetid(), getWigdetRequest( m_visitor, debugmode));
}

WidgetMessageDispatcher::Request WidgetMessageDispatcher::getFormActionRequest( bool debugmode)
{
	QPair<QString,QByteArray> actionrequest = getActionRequest( m_visitor, debugmode);
	QString actiontag;
	actiontag.push_back( '-');
	actiontag.append( actionrequest.first);
	return Request( actiontag, actionrequest.second);
}

QString WidgetMessageDispatcher::getActionId( const QString& tag)
{
	if (!tag.isEmpty() && tag.at(0) == '-')
	{
		return tag.mid( 1, tag.size()-1);
	}
	return QString();
}

QList<QWidget*> WidgetMessageDispatcher::findRecipients( const QString& tag) const
{
	return m_visitor.findSubNodes( nodeProperty_hasWidgetId, tag);
}

bool WidgetMessageDispatcher::feedResult( const QString& tag, const QByteArray& data)
{
	if (!tag.isEmpty() && tag.at(0) == '-') return true;

	bool found = false;
	bool rt = true;
	foreach (QWidget* widget, m_visitor.findSubNodes( nodeProperty_hasWidgetId, tag))
	{
		found = true;
		WidgetVisitor visitor( widget);
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

