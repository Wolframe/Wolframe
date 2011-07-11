#if !defined WIN32
   #include <dlfcn.h>
   #include <sys/types.h>
#else
   #include <windows.h>
#endif


#if defined WIN32
    HINSTANCE hplugin=LoadLibrary(file);
    if (hplugin != NULL) {
            PConst pinconstruct = (PConst)GetProcAddress(hplugin,"construct");
#else
    void * hplugin=dlopen(file,RTLD_NOW);
    if (hplugin != NULL) {
            PConst pinconstruct = (PConst)dlsym(hplugin,"construct");
#endif

#if !defined WIN32
            perror(dlerror());
#endif
