#include "WidgetVisitor_QDateEdit.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QDateEdit::WidgetVisitorState_QDateEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_dateEdit(qobject_cast<QDateEdit*>( widget_))
{
	m_dateEdit->setDate( QDate::currentDate());
}

bool WidgetVisitorState_QDateEdit::enter( const QString& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QDateEdit::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QDateEdit::clear()
{
	m_dateEdit->setDate( QDate::currentDate());
}

QVariant WidgetVisitorState_QDateEdit::property( const QString& name)
{
	if (name.isEmpty())
	{
		QVariant( m_dateEdit->date().toString( Qt::ISODate));
	}
	return QVariant();
}

bool WidgetVisitorState_QDateEdit::setProperty( const QString& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_dateEdit->setDate( QDate::fromString( data.toString(), Qt::ISODate));
	}
	return false;
}

const QList<QString>& WidgetVisitorState_QDateEdit::dataelements() const
{
	static const DataElements dataElements( "");
	return dataElements;
}

bool WidgetVisitorState_QDateEdit::isRepeatingDataElement( const QString& /*name*/)
{
	return false;
}

void WidgetVisitorState_QDateEdit::setState( const QVariant& state)
{
	if (state.isValid()) m_dateEdit->setDate( state.toDate());
}

QVariant WidgetVisitorState_QDateEdit::getState() const
{
	return QVariant( m_dateEdit->date());
}


