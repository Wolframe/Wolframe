#ifndef _Wolframe_METHODTABLE_C_H_INCLUDED
#define _Wolframe_METHODTABLE_C_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

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

typedef enum
{
   OpenTag,         //Open new hierarchy level
   Attribute,       //Attribute Name
   Value,           //Content or attribute Value
   CloseTag         //Close current hierarchy level
}
ElementType;

typedef struct ContentIterator* ContentIteratorP;

typedef bool (*GetNext)( ContentIteratorP this_, ElementType* type, void* buffer, size_t buffersize, size_t* bufferpos);

typedef struct ContentIterator
{
   void* m_ptr;
   size_t m_pos;
   size_t m_size;
   bool m_gotEoD;
   ContentIteratorState m_state;
   int m_errorCode;
   GetNext m_getNext;
}
ContentIterator;

typedef struct FormatOutput* FormatOutputP;

typedef bool (*Print)( FormatOutputP this_, int type, void* element, size_t elementsize);

typedef struct FormatOutput
{
   void* m_ptr;
   size_t m_pos;
   size_t m_size;
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

#ifdef __cplusplus
}
#endif

#endif /* _Wolframe_METHODTABLE_C_H_INCLUDED */


