using System;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Text.RegularExpressions;

namespace WolframeClient
{
    public class Serializer
    {
        public static byte[] getRequestContent(string doctype, string root, Type type, object obj)
        {
            var xattribs = new XmlAttributes();
            var xroot = new XmlRootAttribute(root);
            xattribs.XmlRoot = xroot;
            var xoverrides = new XmlAttributeOverrides();
            //... have to use XmlAttributeOverrides because .NET insists on the object name as root element name otherwise ([XmlRoot(..)] has no effect)
            xoverrides.Add(type, xattribs);

            XmlSerializer serializer = new XmlSerializer(type, xoverrides);
            StringWriter sw = new StringWriter();
            XmlWriterSettings wsettings = new XmlWriterSettings();
            wsettings.OmitXmlDeclaration = false;
            wsettings.Encoding = new UTF8Encoding(false/*no BOM*/, true/*throw if input illegal*/);
            XmlWriter xw = XmlWriter.Create(sw, wsettings);
            xw.WriteProcessingInstruction("xml", "version='1.0' standalone='no'");
            //... have to write header by hand (OmitXmlDeclaration=false has no effect)
            xw.WriteDocType(root, null, doctype + ".sfrm", null);

            XmlSerializerNamespaces ns = new XmlSerializerNamespaces();
            ns.Add("", "");
            //... trick to avoid printing of xmlns:xsi xmlns:xsd attributes of the root element

            serializer.Serialize(xw, obj, ns);
            return wsettings.Encoding.GetBytes( sw.ToString());
        }

        public static object getResult(byte[] content, Type type)
        {
            string stringContent = System.Text.UTF8Encoding.UTF8.GetString(content);
            Regex doctypeDeclaration = new Regex("<!DOCTYPE[ ]*[^>]*[>]");
            string saveContent = doctypeDeclaration.Replace(stringContent, "");
            // ... !DOCTYPE is prohibited for security reasons by .NET, if not 
            //      explicitely enabled. We cut it out, because we do not need it.
            StringReader sr = new StringReader(saveContent);
            XmlReader xr = XmlReader.Create(sr);
            XmlSerializer xs = new XmlSerializer(type);
            return xs.Deserialize(xr);
        }
    }
}
