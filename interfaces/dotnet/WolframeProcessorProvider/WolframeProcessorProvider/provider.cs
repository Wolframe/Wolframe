using System;
using System.Runtime.InteropServices;

// Include this module for Wolframe .NET modules (COM/.NET interop callback to processor provider)
namespace Wolframe
{

    // Declare processor provider as COM interface
    [ComVisible(true)]
    [Guid("CE320901-8DBD-459A-821F-423E6C14D661")]
    [InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    public interface ProcProvider
    {
        //\param[in] funcname Name of Wolframe function to call
        //\param[in] argument Structure to pass to function as input
        //\param[out] result Result structure to get from function as output
        [DispId(1)]
        object call(
            [In, MarshalAs(UnmanagedType.BStr)] string funcname,
            [In] object argument,
            [In] Guid resulttype);

        //\param[in] funcname Name of Wolframe function to call
        //\param[in] argument Structure to pass to function as input
        [DispId(2)]
        void call(
            [In, MarshalAs(UnmanagedType.BStr)] string funcname,
            [In] object argument);
    }
} //namespace


