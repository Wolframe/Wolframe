#include "FormCall.hpp"
#include <QDebug>

QByteArray FormCall::name( const QByteArray& callstr)
{
	int idx=callstr.indexOf('?');
	return idx>=0?callstr.mid(0,idx):callstr;
}

QString FormCall::name( const QString& callstr)
{
	int idx=callstr.indexOf('?');
	return idx>=0?callstr.mid(0,idx):callstr;
}

static QVariant parseValue( const QString& callstr, int& idx)
{
	QVariant rt;
	QList<QVariant> rtlist;
	QString::const_iterator itr = callstr.begin() + idx, end = callstr.end();
	QString val;
	for (;itr != end && *itr != '&'; ++itr)
	{
		if (*itr == '\'')
		{
			while (itr != end)
			{
				if (*itr == '\'')
				{
					++itr;
					if (*itr == '\'')
					{
						//... escaped quote
						val.push_back( '\'');
						++itr;
					}
					else
					{
						//... end quote
						break;
					}
				}
			}
		}
		else if (*itr == ',')
		{
			rtlist.push_back( QVariant(val));
			val.clear();
		}
		else
		{
			val.push_back( *itr);
		}
	}
	idx = itr - callstr.begin();
	if (!rtlist.isEmpty())
	{
		rtlist.push_back( QVariant(val));
		rt = QVariant( rtlist);
	}
	else
	{
		rt = QVariant(val);
	}
	return rt;
}

FormCall::FormCall( const QString& callstr)
{
	int paramidx = callstr.indexOf( '?');
	if (paramidx >= 0)
	{
		m_name = callstr.mid( 0, paramidx).toAscii();
		while (paramidx < callstr.size())
		{
			int eqidx = callstr.indexOf( '=', paramidx);
			int amidx = callstr.indexOf( '&', paramidx);
			if (eqidx < 0 || (eqidx > amidx && amidx >= 0))
			{
				qCritical() << "Syntax error in form parameter list. missing assignment '=' in declaration";
				return;
			}
			else
			{
				QByteArray paramname( callstr.mid( paramidx+1, eqidx-paramidx-1).toAscii());
				paramidx = eqidx + 1;
				QVariant paramvalue( parseValue( callstr, paramidx));
				m_parameter.push_back( Parameter( paramname, paramvalue));
			}
		}
	}
	else
	{
		m_name = callstr.toAscii();
	}
}


