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
#include "WidgetVisitor_QComboBox.hpp"
#include <QDebug>

WidgetVisitorState_QComboBox::WidgetVisitorState_QComboBox( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_mode(None)
	,m_comboBox(qobject_cast<QComboBox*>(widget_))
	,m_elementname(widget_->objectName().toAscii())
	,m_currentindex(0)
{
	m_dataelements.push_back( m_elementname);
}

void WidgetVisitorState_QComboBox::clear()
{
	m_comboBox->clear();
	m_currentindex = 0;
	m_mode = None;
}

bool WidgetVisitorState_QComboBox::enter( const QString& name, bool writemode)
{
	switch (m_mode)
	{
		case None:
			if (name == m_elementname)
			{
				if (writemode)
				{
					if (m_comboBox->count() < m_comboBox->maxCount())
					{
						m_comboBox->addItem( "");
						m_comboBox->setCurrentIndex( m_comboBox->count()-1);
						m_mode = Value;
					}
					else
					{
						return false;
					}
				}
				else
				{
					if (m_currentindex < m_comboBox->count())
					{
						m_comboBox->setCurrentIndex( m_currentindex++);
						m_mode = Select;
					}
					else
					{
						return false;
					}
				}
				return true;
			}
		case Value:
		case Select:
			break;
	}
	return false;
}

bool WidgetVisitorState_QComboBox::leave( bool /*writemode*/)
{
	switch (m_mode)
	{
		case None:	return false;
		case Value:	m_mode = None; m_comboBox->setCurrentIndex( m_currentindex=0); return true;
		case Select:	m_mode = None; return true;
	}
	return false;
}

QVariant WidgetVisitorState_QComboBox::property( const QString& name)
{
	switch (m_mode)
	{
		case None:
			if (name == "selected")
			{
				if (m_comboBox->currentIndex() >= 0)
				{
					return m_comboBox->itemData( m_comboBox->currentIndex(), Qt::UserRole);
				}
				else
				{
					return m_comboBox->property( "_w_selected");
				}
			}
			break;
		case Value:
			break;
		case Select:
			if (name.isEmpty())
			{
				return QVariant( m_comboBox->currentText());
			}
			if (name == "id")
			{
				if (m_comboBox->currentIndex() < 0) return QVariant();
				return m_comboBox->itemData( m_comboBox->currentIndex(), Qt::UserRole);
			}
	}
	return QVariant();
}

bool WidgetVisitorState_QComboBox::setProperty( const QString& name, const QVariant& data)
{
	/*[-]*/qDebug() << "WidgetVisitorState_QComboBox::setProperty" << name << data;
	switch (m_mode)
	{
		case None:
			if (name == "selected")
			{
				m_comboBox->setProperty( "_w_selected", data);
				endofDataFeed();
				return true;
			}
			return false;
		case Value:
			if (name.isEmpty())
			{
				if (m_comboBox->currentIndex() < 0) return false;
				m_comboBox->setItemText( m_comboBox->currentIndex(), data.toString());
				return true;
			}
			if (name == "id")
			{
				if (m_comboBox->currentIndex() < 0) return false;
				m_comboBox->setItemData( m_comboBox->currentIndex(), data, Qt::UserRole);
				return true;
			}
			return false;
		case Select:
			if (name.isEmpty())
			{
				int idx = m_comboBox->findText( data.toString(), Qt::MatchExactly);
				if (idx < 0) return false;
				m_comboBox->setCurrentIndex( idx);
				return true;
			}
			if (name == "id")
			{
				int idx = m_comboBox->findData( data, Qt::UserRole, Qt::MatchExactly);
				if (idx < 0) return false;
				m_comboBox->setCurrentIndex( idx);
				return true;
			}
			return false;
	}
	return false;
}

const QList<QString>& WidgetVisitorState_QComboBox::dataelements() const
{
	static const DataElements ar_Select( "id", 0);
	static const DataElements ar_Value( "id", 0);
	static const QList<QString> ar_Empty;

	switch (m_mode)
	{
		case None:	return m_dataelements;
		case Value:	return ar_Value;
		case Select:	return ar_Select;
	}
	return ar_Empty;
}

bool WidgetVisitorState_QComboBox::isRepeatingDataElement( const QString& name)
{
	return (name == m_elementname);
}

void WidgetVisitorState_QComboBox::setState( const QVariant& state)
{
	int idx = m_comboBox->findData( state, Qt::UserRole, Qt::MatchExactly);
	if (idx >= 0) m_comboBox->setCurrentIndex( idx);
}

QVariant WidgetVisitorState_QComboBox::getState() const
{
	return m_comboBox->itemData( m_comboBox->currentIndex(), Qt::UserRole);
}

void WidgetVisitorState_QComboBox::endofDataFeed()
{
	QVariant selected = m_comboBox->property( "_w_selected");
	if (selected.isValid())
	{
		int idx = m_comboBox->findData( selected, Qt::UserRole, Qt::MatchExactly);
		if (idx < 0) return;
		m_comboBox->setCurrentIndex( idx);
	}
	m_comboBox->setSizeAdjustPolicy( QComboBox::AdjustToContents);
}

