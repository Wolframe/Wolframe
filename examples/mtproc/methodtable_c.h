#ifndef _Wolframe_METHODTABLE_C_H_INCLUDED
#define _Wolframe_METHODTABLE_C_H_INCLUDED
#include <stdbool.h> 

/* Parallel definitions for the POD data structures usable in plain C defined in 
*    - include/protocol/generator.hpp
*    - example/mtproc/methodtable.hpp
*/

typedef struct MethodDataT* MethodDataP;

typedef enum
{
   Open,           //serving data
   EndOfMessage,   //EWOULDBLK -> have to yield
   Error           //an error occurred
}
ContentIteratorState;

typedef struct ContentIterator* ContentIteratorP;

typedef bool (*GetNext)( ContentIteratorP this_, void* buffer, unsigned int buffersize);

typedef struct ContentIterator
{
   void* m_ptr;
   unsigned int m_pos;
   unsigned int m_size;
   bool m_gotEoD;
   ContentIteratorState m_state;
   int m_errorCode;
   GetNext m_getNext;
}
ContentIterator;

typedef struct FormatOutput* FormatOutputP;

typedef bool (*Print)( FormatOutputP this_, int type, void* element, unsigned int elementsize);

typedef struct FormatOutput
{
   void* m_ptr;
   unsigned int m_pos;
   unsigned int m_size;
   Print m_print;
}
FormatOutput;

typedef struct
{
   MethodDataP data;
   ContentIterator* contentIterator;
   FormatOutput* formatOutput;
}
MethodContext;

//Method call
//@return 0, in case of success, errorcode for client in case of error
typedef int (*MethodCall)( MethodContext* context, unsigned int argc, const char** argv);
typedef MethodDataP (*CreateMethodData)(void);
typedef void (*DestroyMethodData)( MethodDataP);

//method of the processor
typedef struct Method
{   
   const char* name;
   MethodCall call;
   bool hasIO;
} Method;

//current instance with data of the processor
typedef struct Implementation
{
   const Method* mt;
   MethodDataP data;
   CreateMethodData createMethodData;
   DestroyMethodData destroyMethodData;

} Implementation;

#endif


