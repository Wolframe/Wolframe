using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

[ComVisible(true)]
[Guid("755efe8f-f125-494c-89a2-cf3cb03a0c8c")]
public struct IdPair
{
    public int a;
    public int b;
};

[ComVisible(true)]
[Guid("390E047F-36FD-4F23-8CE8-3A4C24B33AD3")]
public struct Address
{
    public string street;
    public string country;
};

[ComVisible(true)]
[Guid("50085256-73D2-443E-B22B-9BB1BBAAFCDD")]
public struct User
{
    public int id;
    public string name;
    public Address place;
};

[ComVisible(true)]
public interface FunctionInterface
{
    [ComVisible(true)]
    int Sub( int i, int j);
    [ComVisible(true)]
    int Add( int i, int j);
    [ComVisible(true)]
    User StoreUser( User usr);
    [ComVisible(true)]
    int MapAddress( Address adr);
    [ComVisible(true)]
    int AddIdPair( IdPair p);
    [ComVisible(true)]
    IdPair GetIdPair( int a);
    [ComVisible(true)]
    Address GetAddress_p( [MarshalAs(UnmanagedType.LPStr)] string street, [MarshalAs(UnmanagedType.LPStr)] string country);
    [ComVisible(true)]
    User GetUser_p( int id, [MarshalAs(UnmanagedType.LPStr)] string name, [MarshalAs(UnmanagedType.LPStr)] string street, [MarshalAs(UnmanagedType.LPStr)] string country);
}

[ComVisible(true)]
[ClassInterface(ClassInterfaceType.None)]
public class Functions : FunctionInterface
{
    public Functions()
    {
        //default constructor needed for COM
    }

    public int Add( int i, int j)
    {
        return i + j;
    }
    public int Sub( int i, int j)
    {
        return i - j;
    }
    public User StoreUser( User usr)
    {
        usr.id += 1;
        return usr;
    }
    public int MapAddress( Address adr)
    {
        return adr.country.Length;
    }
    public int AddIdPair( IdPair p)
    {
        return p.a + p.b;
    }
    public IdPair GetIdPair( int a)
    {
        IdPair rt;
        rt.a = a + 1;
        rt.b = a - 1;
        return rt;
    }
    public Address GetAddress_p( [MarshalAs(UnmanagedType.LPStr)] string street, [MarshalAs(UnmanagedType.LPStr)] string country)
    {
        Address rt;
        rt.street = street;
        rt.country = country;
        return rt;
    }
    public User GetUser_p( int id, [MarshalAs(UnmanagedType.LPStr)] string name, [MarshalAs(UnmanagedType.LPStr)] string street, [MarshalAs(UnmanagedType.LPStr)] string country)
    {
        User rt;
        rt.id = id;
        rt.name = name;
        rt.place.street = street;
        rt.place.country = country;
        return rt;
    }
}

