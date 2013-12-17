
public Address GetUserAddress( Wolframe.ProcProvider provider, User usr)
{
    Address rt = (Address)provider.call( "GetAddress", usr, typeof(Address).GUID);
    return rt;
}
