#include "DataTreeSerialize.hpp"
#include "DataTree.hpp"
#include "WidgetVisitor.hpp"
#include <QDebug>

struct StackElement
{
	QSharedPointer<DataTree> tree;
	int arraysize;
	int arrayelemidx;
	int nodeidx;
	QHash<QString, QVariant> arrayValueMap;

	StackElement()
		:arraysize(-1),arrayelemidx(0),nodeidx(0){}
	StackElement( const DataTree& tree_)
		:tree( new DataTree( tree_)),arraysize(-1),arrayelemidx(0),nodeidx(0){}
	StackElement( const QSharedPointer<DataTree>& tree_)
		:tree(tree_),arraysize(-1),arrayelemidx(0),nodeidx(0){}
	StackElement( const StackElement& o)
		:tree(o.tree),arraysize(o.arraysize),arrayelemidx(o.arrayelemidx),nodeidx(o.nodeidx),arrayValueMap(o.arrayValueMap){}
};

static void mapValue( QList<DataSerializeItem>& rt, WidgetVisitor& visitor, QList<StackElement>& stk, int arrayidx)
{
	QString value = stk.back().tree->value().toString();
	if (value.size() > 1)
	{
		if (value.at(0) == '{' && value.at(value.size()-1) == '}')
		{
			QString propkey = value.mid( 1, value.size()-2);
			QVariant prop;
			if (arrayidx >= 0)
			{
				int ai = stk.at(arrayidx).arrayelemidx;
				if (ai == 0)
				{
					prop = stk[ arrayidx].arrayValueMap[ propkey] = visitor.property( propkey);
				}
				else
				{
					prop = stk[ arrayidx].arrayValueMap[ propkey];
				}
				if (prop.type() == QVariant::List)
				{
					if (prop.toList().size() > ai)
					{
						rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toList().at(ai)));
					}
					else
					{
						qCritical() << "accessing array with index out of range:" << propkey;
						rt.push_back( DataSerializeItem( DataSerializeItem::Value, ""));
					}
				}
				else if (prop.isValid())
				{
					rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toString()));
				}
				else
				{
					qCritical() << "accessing non existing property" << propkey;
				}
			}
			else
			{
				prop = visitor.property( propkey);
				if (prop.type() == QVariant::List)
				{
					qCritical() << "referencing list of element in a single element context:" << propkey;
					rt.push_back( DataSerializeItem( DataSerializeItem::Value, ""));
				}
				else if (prop.isValid())
				{
					rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toString()));
				}
				else
				{
					qCritical() << "accessing non existing property" << propkey;
				}
			}
		}
		else
		{
			QVariant resolved = visitor.resolve( value);
			rt.push_back( DataSerializeItem( DataSerializeItem::Value, resolved.toString()));
		}
	}
}

static int calcArraySize( const QSharedPointer<DataTree>& dt)
{
	if (dt->elemtype() != DataTree::Array) return 0;
	int rt = 0;

	QList<StackElement> stk;
	stk.push_back( StackElement( dt));
	while (!stk.isEmpty())
	{
		if (stk.back().tree->value().type() == QVariant::List)
		{
			int arsize = stk.back().tree->value().toList().size();
			if (rt < arsize) rt = arsize;
		}
		int ni = stk.back().nodeidx;
		if (ni >= stk.back().tree->size())
		{
			stk.pop_back();
		}
		else
		{
			stk.push_back( stk.back().tree->nodevalue( ni));
		}
	}
	return rt;
}

static int getLastArrayIdx( const QList<StackElement>& stk)
{
	int arrayidx = stk.size()-1;
	for (;arrayidx >= 0; --arrayidx)
	{
		if (stk.at(arrayidx).tree->elemtype() == DataTree::Array)
		{
			break;
		}
	}
	return arrayidx;
}

QList<DataSerializeItem> getWidgetDataSerialization( const DataTree& datatree, QWidget* widget)
{
	QList<DataSerializeItem> rt;
	WidgetVisitor visitor( widget);
	QList<StackElement> stk;
	stk.push_back( datatree);
	int arrayidx = -1;

	while (!stk.isEmpty())
	{
		if (stk.back().tree->value().isValid())
		{
			mapValue( rt, visitor, stk, arrayidx);
		}
		int ni = stk.back().nodeidx;
		if (ni >= stk.back().tree->size())
		{
			rt.push_back( DataSerializeItem( DataSerializeItem::CloseTag, ""));
			if (stk.back().tree->elemtype() == DataTree::Array && ++stk.back().arrayelemidx < stk.back().arraysize)
			{
				rt.push_back( DataSerializeItem( DataSerializeItem::OpenTag, stk.back().tree->nodename( ni)));
			}
			else
			{
				stk.pop_back();
				if (arrayidx >= stk.size()) arrayidx = getLastArrayIdx( stk);
				if (!stk.isEmpty()) stk.back().nodeidx++;
			}
			continue;
		}
		else if (stk.back().tree->isAttribute( ni))
		{
			rt.push_back( DataSerializeItem( DataSerializeItem::Attribute, stk.back().tree->nodename( ni)));
			stk.push_back( StackElement( stk.back().tree->nodevalue(ni)));
			mapValue( rt, visitor, stk, arrayidx);
			stk.pop_back();
			stk.back().nodeidx++;
		}
		else
		{
			rt.push_back( DataSerializeItem( DataSerializeItem::OpenTag, stk.back().tree->nodename( ni)));
			stk.push_back( StackElement( stk.back().tree->nodevalue(ni)));

			if (stk.back().tree->elemtype() == DataTree::Array)
			{
				arrayidx = stk.size()-1;
				stk.back().arraysize = calcArraySize( stk.back().tree);
			}
		}
	}
	return rt;
}



