#ifndef _Wolframe_METHODTABLE_H_INCLUDED
#define _Wolframe_METHODTABLE_H_INCLUDED

typedef struct MethodData;
typedef struct ContentIterator;

typedef enum
{
   Open,           //serving data
   EndOfMessage,   //EWOULDBLK -> have to yield
   Error           //an error occurred
}
ContentIteratorState;

typedef bool (*GetNext)( ContentIterator* this_, void* buffer, unsigned int buffersize);

typedef struct
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

typedef bool (*Print)( FormatOutput* this_, int type, void* element, unsigned int elementsize);

typedef struct
{
   void* m_ptr;
   unsigned int m_pos;
   unsigned int m_size;
   Print m_print;
}
FormatOutput;

typedef struct
{
   ObjectData* data;
   ContentIterator* contentIterator;
   FormatOutput* formatOutput;
}
MethodContext;

//Method call
//@return 0, in case of success, errorcode for client in case of error
typedef int (*MethodCall)( MethodContext* context, unsigned int argc, const char** argv);
typedef MethodData* (*CreateMethodData)();
typedef void (*DestroyMethodData)( MethodData*);

//method of the processor
typedef struct Method
{   
   const char* name;
   Call call;
   bool hasIO;
} Method;

//current instance with data of the processor
typedef struct Implementation
{
   const Method* mt;
   ObjectData* data;
   CreateMethodData createMethodData;
   DestroyMethodData destroyMethodData;

} Implementation;

#endif


