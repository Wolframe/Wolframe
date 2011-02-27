#ifndef _Wolframe_METHODTABLE_H_INCLUDED
#define _Wolframe_METHODTABLE_H_INCLUDED

typedef struct MethodData;

typedef struct MethodContext
{
   ObjectData* data;
   void* contentIterator;

} MethodContext;


//Method call
//@return 0, in case of success, errorcode for client in case of error
typedef int (*MethodCall)( MethodContext* context, unsigned int argc, const char** argv);

//method of the processor
typedef struct Method
{   
   const char* name;
   Call call;

} Method;


//current instance with data of the processor
typedef struct ObjectInstance
{
   const Method* mt;
   ObjectData* data;

} ObjectInstance;

#endif


