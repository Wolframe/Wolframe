/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
/// \file appdevel/databaseModuleMacros.hpp
/// \brief Macros for a module for defining a database interface
#include "appdevel/module/authenticationConstructor.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <boost/lexical_cast.hpp>

/// \brief Defines a simple database (one database object per unit) interface
#define WF_DATABASE(NAME,DBCLASS,CONFIGCLASS) \
{\
	class Unit :public _Wolframe::db::DatabaseUnit \
	{\
	public:\
		Unit( const CONFIGCLASS& cfg)\
			:m_id(cfg.ID()),m_db(cfg){}\
		virtual const char* className() const\
		{\
			return #DBCLASS "Unit";\
		}\
		virtual const std::string& ID() const\
		{\
			return m_id;\
		}\
		virtual DBCLASS* database()\
		{\
			return &m_db;\
		}\
	private:\
		std::string m_id;\
		DBCLASS m_db;\
	};\
	class Constructor :public _Wolframe::ConfiguredObjectConstructor< _Wolframe::db::DatabaseUnit >\
	{\
	public:\
		virtual ~Constructor(){}\
		_Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return DATABASE_OBJECT;\
		}\
		const char* objectClassName() const\
		{\
			return NAME "Database";\
		}\
		_Wolframe::db::DatabaseUnit* object( const _Wolframe::config::NamedConfiguration& cfgi)\
		{\
			const CONFIGCLASS* cfg = dynamic_cast<const CONFIGCLASS*>(&cfgi);\
			return new Unit(*cfg);\
		}\
	};\
	class BuilderDescription : public _Wolframe::module::ConfiguredBuilder\
	{\
	public:\
		BuilderDescription( const char* title, const char* section,\
					const char* keyword, const char* className )\
			:_Wolframe::module::ConfiguredBuilder( title, section, keyword, className ){}\
		virtual ~BuilderDescription()\
		{}\
		virtual _Wolframe::config::NamedConfiguration* configuration( const char* logPrefix )\
		{\
			return new CONFIGCLASS( m_title, logPrefix);\
		}\
		virtual _Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return m_constructor.objectType();\
		}\
		virtual _Wolframe::ObjectConstructorBase* constructor()\
		{\
			return &m_constructor;\
		}\
	private:\
		Constructor m_constructor;\
	};\
	struct Builder \
	{\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			static BuilderDescription mod( NAME "Database", "Database", NAME, NAME "Database");\
			return &mod;\
		}\
	};\
	(*this)(&Builder::impl);\
}


/// \brief Defines a test database interface (simple database with initializer class with configuration as constructor argument that is constructed before the database and destructed after)
#define WF_TEST_DATABASE(NAME,DBCLASS,CONFIGCLASS,DBINITCLASS) \
{\
	class Unit :public _Wolframe::db::DatabaseUnit \
	{\
	public:\
		Unit( const CONFIGCLASS& cfg)\
			:m_dbinit(cfg),m_db(cfg){}\
		virtual const char* className() const\
		{\
			return #DBCLASS "Unit";\
		}\
		virtual const std::string& ID() const\
		{\
			return m_db.ID();\
		}\
		virtual DBCLASS* database()\
		{\
			return &m_db;\
		}\
	private:\
		DBINITCLASS m_dbinit;\
		DBCLASS m_db;\
	};\
	class Constructor :public _Wolframe::ConfiguredObjectConstructor< _Wolframe::db::DatabaseUnit >\
	{\
	public:\
		virtual ~Constructor(){}\
		_Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return DATABASE_OBJECT;\
		}\
		const char* objectClassName() const\
		{\
			return NAME "Database";\
		}\
		Unit* object( const _Wolframe::config::NamedConfiguration& cfgi)\
		{\
			const CONFIGCLASS* cfg = dynamic_cast<const CONFIGCLASS*>(&cfgi);\
			return new Unit(*cfg);\
		}\
	};\
	class BuilderDescription : public _Wolframe::module::ConfiguredBuilder\
	{\
	public:\
		BuilderDescription( const char* title, const char* section,\
					const char* keyword, const char* className )\
			:_Wolframe::module::ConfiguredBuilder( title, section, keyword, className ){}\
		virtual ~BuilderDescription()\
		{}\
		virtual _Wolframe::config::NamedConfiguration* configuration( const char* logPrefix )\
		{\
			return new CONFIGCLASS( m_title, logPrefix);\
		}\
		virtual _Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return m_constructor.objectType();\
		}\
		virtual _Wolframe::ObjectConstructorBase* constructor()\
		{\
			return &m_constructor;\
		}\
	private:\
		Constructor m_constructor;\
	};\
	struct Builder \
	{\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			static BuilderDescription mod( NAME "Database", "Database", NAME, NAME "Database");\
			return &mod;\
		}\
	};\
	(*this)(&Builder::impl);\
}




