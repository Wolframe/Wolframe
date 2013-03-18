#include "WidgetVisitor_QDateTimeEdit.hpp"
#include "WidgetVisitor.hpp"

WidgetVisitorState_QDateTimeEdit::WidgetVisitorState_QDateTimeEdit( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_dateTimeEdit(qobject_cast<QDateTimeEdit*>( widget_))
{
	m_dateTimeEdit->setDateTime( QDateTime::currentDateTime());
}

bool WidgetVisitorState_QDateTimeEdit::enter( const QByteArray& /*name*/, bool /*writemode*/)
{
	return false;
}

bool WidgetVisitorState_QDateTimeEdit::leave( bool /*writemode*/)
{
	return false;
}

void WidgetVisitorState_QDateTimeEdit::clear()
{
	m_dateTimeEdit->setDateTime( QDateTime::currentDateTime());
}

QVariant WidgetVisitorState_QDateTimeEdit::property( const QByteArray& name)
{
	if (name.isEmpty())
	{
		QVariant( m_dateTimeEdit->dateTime().toString( Qt::ISODate));
	}
	return QVariant();
}

bool WidgetVisitorState_QDateTimeEdit::setProperty( const QByteArray& name, const QVariant& data)
{
	if (name.isEmpty())
	{
		m_dateTimeEdit->setDateTime( QDateTime::fromString( data.toString(), Qt::ISODate));
	}
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QDateTimeEdit::dataelements() const
{
	static const DataElements dataElements( "");
	return dataElements;
}

bool WidgetVisitorState_QDateTimeEdit::isRepeatingDataElement( const QByteArray& /*name*/)
{
	return false;
}

void WidgetVisitorState_QDateTimeEdit::setState( const QVariant& state)
{
	if (state.isValid()) m_dateTimeEdit->setDateTime( state.toDateTime());
}

QVariant WidgetVisitorState_QDateTimeEdit::getState() const
{
	return QVariant( m_dateTimeEdit->dateTime());
}


