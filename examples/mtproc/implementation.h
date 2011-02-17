#ifndef _SMERP_MTPROC_IMPLEMENTATION_H_INCLUDED
#define _SMERP_MTPROC_IMPLEMENTATION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "methodtable.h"

//example mtprocHandler object implementation in C++ish form
int method_echo( MethodContext* context, unsigned int argc, const char** argv);
int method_printarg( MethodContext* context, unsigned int argc, const char** argv);

ObjectInstance* createObjectInstance();
void destroyObjectInstance( ObjectInstance*);

#ifdef __cplusplus
}//"C"
#endif

#endif


