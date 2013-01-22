using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

[ComVisible(true)]
[System.Runtime.InteropServices.StructLayoutAttribute(System.Runtime.InteropServices.LayoutKind.Sequential)]
[Guid("390E047F-36FD-4F23-8CE8-3A4C24B33AD3")]
public struct Address
{
    public string street;
    public string coutry;
};

[ComVisible(true)]
[System.Runtime.InteropServices.StructLayoutAttribute(System.Runtime.InteropServices.LayoutKind.Sequential)]
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
    int Add(int i, int j);
    [ComVisible(true)]
    User StoreUser(User usr);
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
    public int Mul(int i, int j)
    {
        return i * j;
    }
}




