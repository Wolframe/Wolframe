#ifndef _EXPORTABLE_H
#define _EXPORTABLE_H

#ifndef _WIN32

#define WOLFRAME_DLL_VISIBLE

#else

#ifdef BUILD_SHARED

#ifdef BUILDING_WOLFRAME_LIB
#define WOLFRAME_DLL_VISIBLE __declspec(dllexport)
#else
#define WOLFRAME_DLL_VISIBLE __declspec(dllimport)
#endif

#else // BUILD_SHARED

#define WOLFRAME_DLL_VISIBLE

#endif // BUILDING_WOLFRAME_LIB

#endif // _WIN32

#endif // _EXPORTABLE_H
