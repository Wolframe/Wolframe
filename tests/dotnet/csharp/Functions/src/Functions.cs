using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Wolframe;

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
    [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_I4)] public int[] relation;
    [MarshalAs(UnmanagedType.BStr)] public string name;
    public Address place;
};

[ComVisible(true)]
[Guid("B3BFB013-B9A5-49B9-9C4D-93ADE58063FE")]
public struct UserGroup
{
    [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_RECORD)]  public User[] usr;
    [MarshalAs(UnmanagedType.BStr)] public string name;
};

[ComVisible(true)]
public interface FunctionInterface
{
    [ComVisible(true)]  int Sub( int i, int j);
    [ComVisible(true)]  int Add( int i, int j);
    [ComVisible(true)]  int Sum( int[] aa);
    [ComVisible(true)]  User StoreUser( User usr);
    [ComVisible(true)]  User[] StoreUsers( [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_RECORD)] User[] usr);
    [ComVisible(true)]  UserGroup StoreUserGroup( UserGroup usr);
    [ComVisible(true)]  int MapAddress( Address adr);
    [ComVisible(true)]  int AddIdPair( IdPair p);
    [ComVisible(true)]  int AddIdPairs( [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_RECORD)] IdPair[] p);
    [ComVisible(true)]  IdPair GetIdPair( int a);
    [ComVisible(true)]  IdPair[] GetIdPairs( int len);
    [ComVisible(true)]  string ConcatStrings( [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_BSTR)] string[] sar);
    [ComVisible(true)]  Address GetAddress_p( [MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country);
    [ComVisible(true)]  User GetUser_p(int id, [MarshalAs(UnmanagedType.BStr)] string name, [MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country);
    [ComVisible(true)]  Address GetUserAddress( [In, MarshalAs(UnmanagedType.IDispatch)] Wolframe.ProcProvider provider, User usr);
    [ComVisible(true)]  int GetUserXYZ([In, MarshalAs(UnmanagedType.IDispatch)] Wolframe.ProcProvider provider);
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

    public int Sum( int[] aa)
    {
        int rt = 0;
        for (int ii = 0; ii < aa.Length; ++ii) rt += aa[ii];
        return rt;
    }

    public User StoreUser( User usr)
    {
        usr.id += 1;
        usr.name = usr.name.ToLower();
        usr.place.street = usr.place.street.ToUpper();
        usr.place.country = usr.place.country.ToUpper();
        return usr;
    }

    public User[] StoreUsers( User[] usr)
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

    public UserGroup StoreUserGroup( UserGroup usr)
    {
        usr.name = usr.name.ToUpper();
        for (int ii = 0; ii < usr.usr.Length; ++ii)
        {
            usr.usr[ii].id += 1;
            usr.usr[ii].name = usr.usr[ii].name.ToLower();
            usr.usr[ii].place.street = usr.usr[ii].place.street.ToUpper();
            usr.usr[ii].place.country = usr.usr[ii].place.country.ToUpper();
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

    public int AddIdPairs( [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_RECORD)] IdPair[] p)
    {
        int rt = 0;
        for (int ii = 0; ii < p.Length; ++ii)
        {
            rt += p[ii].a + p[ii].b;
        }
        return rt;
    }

    public string ConcatStrings( [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_BSTR)] string[] sar)
    {
        string rt = "";
        for (int ii = 0; ii < sar.Length; ++ii)
        {
            rt = rt + sar[ii];
        }
        return rt;
    }

    public IdPair GetIdPair(int a)
    {
        IdPair rt;
        rt.a = a + 1;
        rt.b = a - 1;
        return rt;
    }

    public IdPair[] GetIdPairs( int len)
    {
        IdPair[] rt = new IdPair[ len];
        for (int ii = 0; ii < len; ++ii)
        {
            rt[ ii].a = 10+ii;
            rt[ ii].b = 10+ii+1;
        }
        return rt;
    }

    public Address GetAddress_p([MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country)
    {
        Address rt = new Address();
        rt.street = street;
        rt.country = country;
        return rt;
    }

    public User GetUser_p(int id, [MarshalAs(UnmanagedType.BStr)] string name, [MarshalAs(UnmanagedType.BStr)] string street, [MarshalAs(UnmanagedType.BStr)] string country)
    {
        User rt = new User();
        rt.id = id;
        rt.name = name;
        rt.place.street = street;
        rt.place.country = country;
        return rt;
    }

    public Address GetUserAddress( [In, MarshalAs(UnmanagedType.IDispatch)] Wolframe.ProcProvider provider, User usr)
    {
        Address rt = (Address)provider.call("GetAddress", usr, typeof(Address).GUID);
        Console.WriteLine("street='{0}', country='{1}'", rt.street, rt.country);
        return rt;
    }

    public int GetUserXYZ([In, MarshalAs(UnmanagedType.IDispatch)] Wolframe.ProcProvider provider)
    {
        User usr = new User();
        Address rt = (Address)provider.call( "GetAddress", usr, typeof(Address).GUID);
        Console.WriteLine("street='{0}', country='{1}'", rt.street, rt.country);
        return 1;
    }
}

