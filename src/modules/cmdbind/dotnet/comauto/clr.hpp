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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#ifndef _Wolframe_COM_AUTOMATION_CLR_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_CLR_HPP_INCLUDED
#include <cstring>
#include <string>
#include <vector>

struct tagVARIANT;

namespace _Wolframe {
namespace comauto {

///\remark This object cann only be created and destroyed once (because runtime host cannot be created again in the same process after a stop)
class CommonLanguageRuntime
{
public:
	explicit CommonLanguageRuntime( const std::string& version);
	virtual ~CommonLanguageRuntime();

	void call( tagVARIANT* res, const std::wstring& assembly_, const std::wstring& class_, const std::wstring& method_, unsigned int argc, const tagVARIANT* argv, unsigned int lcid=0x400/*LOCALE_USER_DEFAULT*/) const;
	void call( tagVARIANT* res, const std::string& assembly_utf8_, const std::string& class_utf8_, const std::string& method_utf8_, unsigned int argc, const tagVARIANT* argv, unsigned int lcid=0x400/*LOCALE_USER_DEFAULT*/) const;

private:
	class Impl;
	Impl* m_impl;
};

}} //namespace
#endif
