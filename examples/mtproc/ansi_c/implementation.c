#include "implementation.h"

typedef struct ObjectData
{
   int bla; 
} ObjectData;


int method_echo( MethodContext*, unsigned int, const char**)
{
   return 0;
}

int method_printarg( MethodContext*, unsigned int, const char**)
{
   return 0;
}



static const Method methodTable[3] = {{"echo",&method_echo},{"parg",&method_printarg},{0,0}};

Instance* createObjectInstance()
{
   Instance* rt = (ObjectInstance*)calloc( sizeof(ObjectInstance) + sizeof(ObjectData), 1);
   if (rt == NULL) return NULL;
   rt->data = (ObjectData*)(rt+1);
   rt->mt = methodTable;
   return rt;
}

void destroyObjectInstance( ObjectInstance* inst)
{
   free( inst->data);
   free( inst);   
}


