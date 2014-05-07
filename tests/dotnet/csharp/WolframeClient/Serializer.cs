using System;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Text.RegularExpressions;

namespace WolframeClient
{
    class Serializer
    {
        public static string getRequestString(string doctype, string root, Type type, object obj)
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
            wsettings.Encoding = new UTF8Encoding();
            XmlWriter xw = XmlWriter.Create(sw, wsettings);
            xw.WriteProcessingInstruction("xml", "version='1.0' standalone='no'");
            //... have to write header by hand (OmitXmlDeclaration=false has no effect)
            xw.WriteDocType(root, null, doctype + ".sfrm", null);

            XmlSerializerNamespaces ns = new XmlSerializerNamespaces();
            ns.Add("", "");
            //... trick to avoid printing of xmlns:xsi xmlns:xsd attributes of the root element

            serializer.Serialize(xw, obj, ns);
            return sw.ToString();
        }

        public static object getResult(string content, Type type)
        {
            Regex doctypeDeclaration = new Regex("<!DOCTYPE[ ]*[^>]*[>]");
            string saveContent = doctypeDeclaration.Replace(content, "");
            // ... !DOCTYPE is prohibited for security reasons by .NET, if not 
            //      explicitely enabled. We cut it out, because we do not need it.
            StringReader sr = new StringReader(saveContent);
            XmlReader xr = XmlReader.Create(sr);
            XmlSerializer xs = new XmlSerializer(type);
            return xs.Deserialize(xr);
        }
    }
}
