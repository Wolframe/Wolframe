using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Serialization;

public class Link
{
    [XmlElement("originalUrl")]
    public string OriginalUrl { get; set; }
    // You other props here much like the above
}

[XmlRoot("profileSite")]
public class ProfileSite
{
    [XmlElement("profileId")]
    public int ProfileId { get; set; }

    [XmlElement("siteId")]
    public int SiteId { get; set; }

    [XmlArray("links"), XmlArrayItem("link")]
    public Link[] Links { get; set; }

}

namespace WolframeClient
{
    class Program
    {
        static string getRequestString( string doctype, string root, Type type, object obj)
        {
            var xattribs = new XmlAttributes();
            var xroot = new XmlRootAttribute( root);
            xattribs.XmlRoot = xroot;
            var xoverrides = new XmlAttributeOverrides();
            //... have to use XmlAttributeOverrides because .NET insists on the object name as root element name otherwise ([XmlRoot(..)] has no effect)
            xoverrides.Add( type, xattribs);

            XmlSerializer serializer = new XmlSerializer( type, xoverrides);
            StringWriter sw = new StringWriter();
            XmlWriterSettings wsettings = new XmlWriterSettings();
            wsettings.OmitXmlDeclaration = false;
            wsettings.Encoding = new UTF8Encoding();
            XmlWriter xw = XmlWriter.Create( sw, wsettings);
            xw.WriteProcessingInstruction( "xml", "version='1.0' standalone='no'");
            //... have to write header by hand (OmitXmlDeclaration=false has no effect)
            xw.WriteDocType( root, null, doctype + ".sfrm", null);

            XmlSerializerNamespaces ns = new XmlSerializerNamespaces();
            ns.Add("", "");
            //... trick to avoid printing of xmlns:xsi xmlns:xsd attributes of the root element

            serializer.Serialize( xw, obj, ns);
            return sw.ToString();
        }

        static void Main(string[] args)
        {
            ProfileSite obj = new ProfileSite();
            obj.ProfileId = 1;
            obj.SiteId = 2;
            Link[] lnk1 = new Link[2];
            lnk1[0] = new Link();
            lnk1[1] = new Link();
            lnk1[0].OriginalUrl = "wolframe.net";
            lnk1[1].OriginalUrl = "test.net";
            obj.Links = lnk1;

            var res = getRequestString("profileSite", "profileSite", typeof(ProfileSite), obj);
            Console.WriteLine("\nDocument:\n\n{0}", res);
        }
    }
}
