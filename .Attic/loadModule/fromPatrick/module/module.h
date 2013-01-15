/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file module.h
///\brief module object references loaded from a module

#ifdef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED
struct wolframe_ObjectStruct
{
	const char* type;		///< type of the object, determines what is 'reference' pointing to
	const char* name;		///< name of the object
	void* reference;		///< pointer to the object (objects with the same type MUST have here a pointer to an object of the same type)
	const char* description;	///< description of the object for help
};

struct wolframe_ModuleStruct
{
	char name[ 32];					///< name of the module
	char description[ 100];				///< description of the module for help
	void (*destroy)( wolframe_ModuleStruct* m);	///< destructor function (that has to be implemented in the module itself)
	char _private[ 32];				///< hidden, platform dependent data to avoid some ifdefs in the header
	unsigned int nof_objects;			///< number of elements in objects
	Object* objects;				///< array of objects exported
};

Module* create();

#endif //MODULE_H_INCLUDED
