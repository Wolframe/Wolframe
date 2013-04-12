#include "DataTreeSerialize.hpp"
#include "DataTree.hpp"
#include "WidgetVisitor.hpp"
#include <QDebug>

struct StackElement
{
	QSharedPointer<DataTree> tree;
	int nodeidx;
	int elemidx;
	QHash<QString, QVariant> arrayValueMap;

	StackElement()
		:nodeidx(0),elemidx(0){}
	StackElement( const DataTree& tree_)
		:tree( new DataTree( tree_)),nodeidx(0),elemidx(0){}
	StackElement( const QSharedPointer<DataTree>& tree_)
		:tree(tree_),nodeidx(0),elemidx(0){}
	StackElement( const StackElement& o)
		:tree(o.tree),nodeidx(o.nodeidx),elemidx(o.elemidx),arrayValueMap(o.arrayValueMap){}
};


static void mapValue( QList<DataSerializeItem> rt, WidgetVisitor& visitor, QList<StackElement>& stk, int arrayidx)
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
				int ni = stk.at(arrayidx).nodeidx;
				if (ni == 0)
				{
					prop = stk[ arrayidx].arrayValueMap[ propkey] = visitor.property( propkey);
				}
				else
				{
					prop = stk[ arrayidx].arrayValueMap[ propkey];
				}
				if (prop.type() == QVariant::List)
				{
					if (prop.toList().size() > ni)
					{
						rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toList().at(ni)));
					}
					else
					{
						qCritical() << "accessing array with index out of range:" << propkey;
						rt.push_back( DataSerializeItem( DataSerializeItem::Value, ""));
					}
				}
				else
				{
					rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toString()));
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
				else
				{
					rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toString()));
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

			QString value = stk.back().tree->value().toString();
			if (value.size() > 1)
			{
				if (value.at(0) == '{' && value.at(value.size()-1) == '}')
				{
					QString propkey = value.mid( 1, value.size()-2);
					QVariant prop;
					if (arrayidx >= 0)
					{
						int ni = stk.at(arrayidx).nodeidx;
						if (ni == 0)
						{
							prop = stk[ arrayidx].arrayValueMap[ propkey] = visitor.property( propkey);
						}
						else
						{
							prop = stk[ arrayidx].arrayValueMap[ propkey];
						}
						if (prop.type() == QVariant::List)
						{
							if (prop.toList().size() > ni)
							{
								rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toList().at(ni)));
							}
							else
							{
								qCritical() << "accessing array with index out of range:" << propkey;
								rt.push_back( DataSerializeItem( DataSerializeItem::Value, ""));
							}
						}
						else
						{
							rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toString()));
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
						else
						{
							rt.push_back( DataSerializeItem( DataSerializeItem::Value, prop.toString()));
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
		if (stk.back().nodeidx >= stk.back().tree->size())
		{
			rt.push_back( DataSerializeItem( DataSerializeItem::CloseTag, ""));
			stk.pop_back();
			if (arrayidx >= stk.size())
			{
				arrayidx = stk.size()-1;
				while (arrayidx >= 0 && stk.at(arrayidx).tree->elemtype() != DataTree::Array)
				{
					--arrayidx;
				}
			}
			continue;
		}
		else
		{
			int ni = stk.back().nodeidx;
			rt.push_back( DataSerializeItem( DataSerializeItem::OpenTag, stk.back().tree->nodename( ni)));
			stk.push_back( StackElement( stk.back().tree->nodevalue(ni)));
		}
		if (stk.back().tree->elemtype() == DataTree::Array)
		{
			arrayidx = stk.size()-1;
		}
	}
	return rt;
}

