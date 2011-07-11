#ifndef _MODULE_BASE_HPP_INCLUDED
#define _MODULE_BASE_HPP_INCLUDED

class ModuleBase
{
public:
	virtual ~ModuleBase()		{}

	virtual int doModuleStuff() = 0;
};

#endif // _MODULE_BASE_HPP_INCLUDED
