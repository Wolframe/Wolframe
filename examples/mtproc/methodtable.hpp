/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
#ifndef _Wolframe_METHODTABLE_HPP_INCLUDED
#define _Wolframe_METHODTABLE_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace mtproc {

//method of the processor
struct Method
{
   //defined by the processor instance 
   struct Data;
   
   struct Context
   {
      Data* data;
      boost::shared_ptr<protocol::InputFilter> contentIterator;
      boost::shared_ptr<protocol::FormatOutput> output;
      
      Context()                   :data(0){}
      void init( Data* d=0)       {data=d;contentIterator.reset();output.reset();}

      typedef Data* (*DataConstructor)();
      typedef void (*DataDestructor)( Data* context);
   };
   //Method call
   //@return 0, in case of success, errorcode for client in case of error
   typedef int (*Call)( Context* context, unsigned int argc, const char** argv);
   
   const char* name;
   Call call;
   bool hasIO;
};


//current instance with data of the processor
struct Instance
{
   const Method* m_mt;
   Method::Data* m_data;
   Method::Context::DataConstructor m_createData;
   Method::Context::DataDestructor m_destroyData;

   Instance()                     :m_mt(0),m_data(0),m_createData(0),m_destroyData(0){}
   Instance( const Instance& o)   :m_mt(o.m_mt),m_data(o.m_data),m_createData(o.m_createData),m_destroyData(o.m_destroyData){}
};

}}//namespace
#endif


