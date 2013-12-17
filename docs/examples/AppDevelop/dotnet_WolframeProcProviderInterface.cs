
public interface ProcProvider
{
    object call(
    [In] string funcname,
    [In] object argument,
    [In] Guid resulttype);
}

