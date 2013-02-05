using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

[ComVisible(true)]
[Guid("755efe8f-f125-494c-89a2-cf3cb03a0c8c")]
public struct IdPair
{
    [MarshalAs(UnmanagedType.I4)] public int a;
    [MarshalAs(UnmanagedType.I4)] public int b;
};

[ComVisible(true)]
[Guid("390E047F-36FD-4F23-8CE8-3A4C24B33AD3")]
public struct Address
{
    [MarshalAs(UnmanagedType.BStr)] public string street;
    [MarshalAs(UnmanagedType.BStr)] public string country;
};

[ComVisible(true)]
[Guid("50085256-73D2-443E-B22B-9BB1BBAAFCDD")]
public struct User
{
    [MarshalAs(UnmanagedType.I4)] public int id;
    [MarshalAs(UnmanagedType.BStr)] public string name;
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
    User[] StoreUsers(User[] usr);
    [ComVisible(true)]
    int MapAddress( Address adr);
    [ComVisible(true)]
    int AddIdPair( IdPair p);
    [ComVisible(true)]
    IdPair GetIdPair( int a);
    [ComVisible(true)]
    Address GetAddress_p([MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country);
    [ComVisible(true)]
    User GetUser_p(int id, [MarshalAs(UnmanagedType.BStr)] string name, [MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country);
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
    public User StoreUser(User usr)
    {
        usr.id += 1;
        usr.name = usr.name.ToLower();
        usr.place.street = usr.place.street.ToUpper();
        usr.place.country = usr.place.country.ToUpper();
        return usr;
    }
    public User[] StoreUsers(User[] usr)
    {
        for (int ii = 0; ii < usr.Length; ++ii)
        {
            usr[ii].id += 1;
            usr[ii].name = usr[ii].name.ToLower();
            usr[ii].place.street = usr[ii].place.street.ToUpper();
            usr[ii].place.country = usr[ii].place.country.ToUpper();
        }
        return usr;
    }
    public int MapAddress(Address adr)
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
    public Address GetAddress_p([MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country)
    {
        Address rt;
        rt.street = street;
        rt.country = country;
        return rt;
    }
    public User GetUser_p(int id, [MarshalAs(UnmanagedType.BStr)] string name, [MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country)
    {
        User rt;
        rt.id = id;
        rt.name = name;
        rt.place.street = street;
        rt.place.country = country;
        return rt;
    }
}

