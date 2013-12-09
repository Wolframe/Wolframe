using System.Runtime.InteropServices;

// Include this module for Wolframe .NET modules (COM/.NET interop callback to processor provider)
namespace Wolframe
{

// Declare processor provider as a COM only (AutoDispatch) interface which 
[ComVisible(true)]
[InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
public interface ProcProvider
{
	//\param[in] funcname Name of Wolframe function to call
	//\param[in] argument Structure to pass to function as input
	//\param[out] result Result structure to get from function as output
    object call(
		[In, MarshalAs(UnmanagedType.BStr)] string funcname,
		[In, MarshalAs(UnmanagedType.Struct)] object argument);
}

} //namespace


