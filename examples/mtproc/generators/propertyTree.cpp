#include "propertyTree.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <list>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>


using namespace _Wolframe;
using namespace mtproc;

struct PropertyTreeGenerator::Closure
{
	typedef std::string Key;
	typedef std::string Data;
	typedef boost::property_tree::basic_ptree<Key,Data> Tree;
	typedef std::pair<Key,Tree> Node;
	typedef std::list<Node> NodeList;
	typedef protocol::Generator::size_type size_type;
	typedef protocol::Generator::ElementType Type;

	enum Error
	{
		Ok,
		ErrBufferTooSmall,
		ErrOutOfMem,
		ErrUnexpected
	};

	struct Element
	{
		int state;
		Data data;
		Tree::const_iterator itr;
		Tree::const_iterator end;

		Element( const Data& data_, const Tree::const_iterator& itr_, const Tree::const_iterator& end_)
				:state(0),data(data_),itr(itr_),end(end_) {};
	};
	typedef std::vector<Element> ElementStack;


	Closure( const Tree* pt) :m_type(Value),m_error(Ok)
	{
		m_stack.push_back( Element( pt->data(), pt->begin(), pt->end()));
	}

	bool getData( void* buf, size_type bufsize, size_type* bufpos)
	{
		if (m_stack.back().data.size() + *bufpos > bufsize)
		{
			m_error = ErrBufferTooSmall;
			return false;
		}
		else
		{
			std::memcpy( (char*)buf+*bufpos, m_stack.back().data.c_str(), m_stack.back().data.size());
			*bufpos += m_stack.back().data.size();
			return true;
		}
	}

	bool getNext( Type* type, void* buf, size_type bufsize, size_type* bufpos)
	{
		if (m_stack.empty()) return false;
		try
		{
			for (;;)
			{
				if (m_stack.back().data.size())
				{
					if (!getData( buf, bufsize, bufpos)) return false;
					if (m_stack.back().itr == m_stack.back().end)
					{
						*type = Value;
						m_stack.pop_back();
					}
					else if (m_stack.back().state == 0 && m_stack.back().itr->second.begin() == m_stack.back().itr->second.end())
					{
						*type = Attribute;
						m_stack.push_back(
							Element(
								m_stack.back().itr->first,
								m_stack.back().itr->second.begin(),
								m_stack.back().itr->second.end()
							));
					}
					else
					{
						m_stack.back().state = 1;
						*type = OpenTag;
						m_stack.push_back(
							Element(
								m_stack.back().itr->first,
								m_stack.back().itr->second.begin(),
								m_stack.back().itr->second.end()
							));
					}
					return true;
				}
				else if (m_stack.back().itr == m_stack.back().end)
				{
					m_stack.pop_back();
					*type = CloseTag;
					return true;
				}
				else
				{
					m_stack.back().itr++;
				}
			}
		}
		catch (std::bad_alloc)
		{
			m_error = ErrOutOfMem;
			return false;
		}
		catch (...)
		{
			m_error = ErrUnexpected;
			return false;
		}
	}

	int getError() const
	{
		return m_error;
	}
private:
	ElementStack m_stack;
	Type m_type;
	Error m_error;
};


bool PropertyTreeGenerator::get( Generator* this_, Generator::ElementType* type, void* buf, Generator::size_type bufsize, Generator::size_type* bufpos)
{
	PropertyTreeGenerator* self = (PropertyTreeGenerator*) this_;
	if (!self->m_closure->getNext( type, buf, bufsize, bufpos))
	{
		int err = self->m_closure->getError();
		this_->setState( err?Error:Open, err);
		return false;
	}
	return true;
}


PropertyTreeGenerator::PropertyTreeGenerator( const boost::property_tree::ptree* pt)
				:protocol::Generator(&get)
{
	m_closure = new Closure( pt);
}



