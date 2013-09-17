#include "pythonStructure.hpp"
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

static types::Variant getPythonObjValue( const PyObject* obj_)
{
	PyObject* obj = const_cast<PyObject*>(obj_);
	if (!obj) throw std::runtime_error( "accessing undefined object");
	if (PyLong_Check( obj))
	{
		return types::Variant( PyLong_AsLong( obj));
	}
	else if (PyFloat_Check( obj))
	{
		return types::Variant( PyFloat_AsDouble( obj));
	}
	else if (PyBool_Check( obj))
	{
		if (obj == Py_False)
		{
			return types::Variant( false);
		}
		return types::Variant( true);
	}
	else if (PyBytes_Check( obj))
	{
		const char* str = PyBytes_AsString( obj);
		Py_ssize_t size = PyBytes_Size( obj);
		return types::Variant( str, size);
	}
	else if (PyUnicode_Check( obj))
	{
		PyObject* strUTF8 = PyUnicode_AsUTF8String( obj);
		if (!strUTF8) throw std::runtime_error( "error in conversion to UTF-8");
		const char* str = PyBytes_AsString( strUTF8);
		if (!str)
		{
			Py_DECREF( strUTF8);
			throw std::runtime_error( "failed to convert atomic type");
		}
		Py_ssize_t size = PyBytes_Size( strUTF8);
		types::Variant rt = types::Variant( str, size);
		Py_DECREF( strUTF8);
		return rt;
	}
	else if (obj == Py_None)
	{
		return types::Variant();
	}
	else if (PyByteArray_Check( obj))
	{
		const char* str = PyByteArray_AsString( obj);
		Py_ssize_t size = PyByteArray_Size( obj);
		return types::Variant( str, size);
	}
	else
	{
		PyObject* serialization = PyObject_Str( obj);
		if (!serialization) throw std::runtime_error( "failed to convert atomic type (no serialization to string available)");
		const char* str = PyBytes_AsString( serialization);
		if (!str)
		{
			Py_DECREF( serialization);
			throw std::runtime_error( "failed to convert atomic type");
		}
		Py_ssize_t size = PyBytes_Size( serialization);
		types::Variant rt = types::Variant( str, size);
		Py_DECREF( serialization);
		return rt;
	}
}

static PyObject* getPythonObjFromValue( const types::Variant& val)
{
	switch (val.type())
	{
		case types::Variant::Null:
			Py_RETURN_NONE;
		break;
		case types::Variant::Int:
			return PyLong_FromLong( val.toint());
		break;
		case types::Variant::UInt:
			return PyLong_FromUnsignedLong( val.touint());
		break;
		case types::Variant::Bool:
			if (val.tobool())
			{
				Py_RETURN_TRUE;
			}
			else
			{
				Py_RETURN_FALSE;
			}
		break;
		case types::Variant::Double:
			return PyFloat_FromDouble( val.todouble());
		break;
		case types::Variant::String:
			return PyUnicode_FromStringAndSize( val.charptr(), val.charsize());
		break;
		default:
			throw std::runtime_error("try to get object for non atomic value");
	}
}

static Structure::Type getPythonObjType( const PyObject* obj_)
{
	PyObject* obj = const_cast<PyObject*>(obj_);
	Structure::Type rt = Structure::Nil;
	if (obj)
	{
		if (!PyDict_Check( obj))
		{
			rt = Structure::Map;
		}
		else if (PySequence_Check( obj))
		{
			rt = Structure::Array;
		}
		else
		{
			rt = Structure::Atomic;
		}
	}
	return rt;
}

Structure::Structure()
	:m_obj(0){}

Structure::Structure( PyObject* obj_)
	:m_obj(obj_)
{
	Py_INCREF( m_obj);
}

Structure::Structure( const Structure& o)
	:m_obj(o.m_obj)
{
	if (m_obj) Py_INCREF( m_obj);
}

void Structure::clear()
{
	if (m_obj)
	{
		Py_DECREF( m_obj);
		m_obj = 0;
	}
}

Structure::~Structure()
{
	clear();
}

bool Structure::const_iterator::Element::atomic() const
{
	return getPythonObjType( val) == Atomic;
}

bool Structure::const_iterator::Element::array() const
{
	return getPythonObjType( val) == Array;
}

types::Variant Structure::const_iterator::Element::getValue() const
{
	return getPythonObjValue( val);
}

Structure::const_iterator::const_iterator()
	:m_st(0),m_itr(0),m_pos(0)
{
	m_elem.key = 0;
	m_elem.val = 0;
}

Structure::const_iterator::const_iterator( const Structure* st_)
	:m_st(st_),m_itr(0),m_pos(0)
{
	m_elem.key = 0;
	m_elem.val = 0;

	if (PyIter_Check( m_st->m_obj))
	{
		m_itr = PyObject_GetIter( m_st->m_obj);
		fetch_next();
	}
}

Structure::const_iterator::const_iterator( const const_iterator& o)
	:m_st(o.m_st),m_itr(o.m_itr),m_pos(o.m_pos)
{
	if (m_itr) Py_INCREF( m_itr);
	m_elem.key = o.m_elem.key;
	m_elem.val = o.m_elem.val;
}

Structure::const_iterator::~const_iterator()
{
	if (m_itr) Py_DECREF( m_itr);
}

void Structure::const_iterator::fetch_next()
{
	switch (m_st->type())
	{
		case Nil: break;
		case Atomic: break;
		case Map:
		{
			if (!PyDict_Next( m_st->obj(), &m_pos, &m_elem.key, &m_elem.val))
			{
				m_elem.key = 0;
				m_elem.val = 0;
				m_pos = 0;
			}
			break;
		}
		case Array:
		{
			if (m_itr)
			{
				m_elem.val = PyIter_Next( m_itr);	// borrowed reference
				if (m_elem.val)
				{
					m_elem.key = PyLong_FromLong( m_pos++);
				}
				else
				{
					m_elem.key = 0;
					m_pos = 0;
				}
			}
			else
			{
				m_elem.key = 0;
				m_elem.val = 0;
			}
			break;
		}
	}
}

Structure::Type Structure::type() const
{
	return getPythonObjType( m_obj);
}

Structure* Structure::addStructElement( const std::string& elemid_)
{
}

Structure* Structure::addArrayElement()
{
}

void Structure::setValue( const types::Variant& value_)
{
	clear();
	m_obj = getPythonObjFromValue( value_);
}

const types::Variant& Structure::getValue() const
{
	return getPythonObjValue( m_obj);
}

unsigned int Structure::lastArrayIndex() const
{
}

static void print_newitem( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level)
{
	out << pformat->newitem;
	for (std::size_t ll=0; ll<level; ++ll) out << pformat->indent;
}

void Structure::print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const
{
}

std::string Structure::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream buf;
	print( buf, pformat, 0);
	return buf.str();
}

