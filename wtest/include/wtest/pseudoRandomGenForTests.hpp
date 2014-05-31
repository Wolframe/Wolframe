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
/// \file pseudoRandomGenForTests.hpp
/// \brief Pseudo random number generator depending on current day (for reproducability)
#ifndef _Wolframe_wtest_PSEUDO_RANDOM_GENERATOR_FOR_TESTS_HPP_INCLUDED
#define _Wolframe_wtest_PSEUDO_RANDOM_GENERATOR_FOR_TESTS_HPP_INCLUDED
#include <time.h>

namespace _Wolframe {
namespace wtest {

class Random
{
public:
	Random()
	{
		time_t nowtime;
		struct tm* now;

		::time ( &nowtime);	//init time now
		now = ::localtime( &nowtime);

		m_cur = ((now->tm_year+1) * (now->tm_mon+100) * (now->tm_mday+1)) * KnuthIntegerHashFactor;
	}

	unsigned int get( unsigned int min_, unsigned int max_)
	{
		m_cur = (m_cur+123) * KnuthIntegerHashFactor;
		unsigned int iv = max_ - min_;
		return iv?((m_cur % iv) + min_):min_;
	}

	unsigned int seed() const
	{
		return m_cur;
	}

	void setSeed( unsigned int v)
	{
		m_cur = v;
	}

private:
	enum {KnuthIntegerHashFactor=2654435761U};
	unsigned int m_cur;
};

}}//namespace
#endif

