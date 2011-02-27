#ifndef _Wolframe_MTPROC_IMPLEMENTATION_H_INCLUDED
#define _Wolframe_MTPROC_IMPLEMENTATION_H_INCLUDED
extern "C" {

#include "methodtable.h"

//example mtprocHandler object implementation in C++ish form
int method_echo( MethodContext* context, unsigned int argc, const char** argv);
int method_printarg( MethodContext* context, unsigned int argc, const char** argv);

ObjectInstance* createObjectInstance();
void destroyObjectInstance( ObjectInstance*);

}//"C"
#endif


