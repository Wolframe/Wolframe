#if defined( _MSC_VER )
	#define WOLFRAME_EXPORT __declspec( dllexport )
#else
	#define WOLFRAME_EXPORT
#endif

WOLFRAME_EXPORT void bar( ) {
}
