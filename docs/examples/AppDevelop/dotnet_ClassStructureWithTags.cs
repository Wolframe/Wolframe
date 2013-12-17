using System;
using System.Runtime.InteropServices;

[ComVisible(true)]
[Guid("390E047F-36FD-4F23-8CE8-3A4C24B33AD3")]
public struct Address
{
    [MarshalAs(UnmanagedType.BStr)] public string street;
    [MarshalAs(UnmanagedType.BStr)] public string country;
};

[ComVisible(true)]
public interface FunctionInterface
{
    [ComVisible(true)]  Address GetAddress( [MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country);
}

[ComVisible(true)]
[ClassInterface(ClassInterfaceType.None)]
public class Functions : FunctionInterface
{
    public Address GetAddress([MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country)
    {
        Address rt = new Address();
        rt.street = street;
        rt.country = country;
        return rt;
    }
}

