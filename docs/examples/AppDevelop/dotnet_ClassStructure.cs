
using System;
using System.Runtime.InteropServices;

public struct Address
{
    public string street;
    public string country;
};

public interface FunctionInterface
{
    Address GetAddress( string street, string country);
}

public class Functions : FunctionInterface
{
    public Address GetAddress( string street, string country)
    {
        Address rt = new Address();
        rt.street = street;
        rt.country = country;
        return rt;
    }
}

