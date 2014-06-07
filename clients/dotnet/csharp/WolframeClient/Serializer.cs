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
        private string m_schemadir;
        private string m_schemaext;

        public Serializer(string schemadir_, string schemaext_)
        {
            m_schemadir = schemadir_;
            m_schemaext = schemaext_;
        }

        public byte[] getRequestContent( string doctype, string root, Type type, object obj)
        {
            XmlSerializer serializer = null;
            if (root == null)
            {
                //... root element will be the object type name
                serializer = new XmlSerializer(type);
            }
            else
            {
                //... root element set explicitely
                var xattribs = new XmlAttributes();
                var xroot = new XmlRootAttribute(root);
                xattribs.XmlRoot = xroot;
                var xoverrides = new XmlAttributeOverrides();
                xoverrides.Add(type, xattribs);
                serializer = new XmlSerializer(type, xoverrides);
            }
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Encoding = new UnicodeEncoding(true, false); 
            //... 1) true -> big endian (not respected) and 2) false -> no BOM in a .NET string
            settings.Indent = false;
            settings.OmitXmlDeclaration = false;

            XmlSerializerNamespaces xmlNameSpace = new XmlSerializerNamespaces();
            xmlNameSpace.Add("xsi", "http://www.w3.org/2001/XMLSchema-instance");    
            xmlNameSpace.Add("schemaLocation", m_schemadir + "/" + doctype + "." + m_schemaext);    

            using (StringWriter textWriter = new StringWriter())
            {
                using (XmlWriter xmlWriter = XmlWriter.Create(textWriter, settings))
                {
                    serializer.Serialize(xmlWriter, obj, xmlNameSpace);
                }
                string str = textWriter.ToString().Replace("xmlns:schemaLocation", "xsi:schemaLocation").Replace("encoding=\"utf-16\"", "encoding=\"utf-16le\"");
                //... PF:HACK: Sorry for this hack. Could not help myself. Tagging redundantly all structures is no option. Schmema collection locations should be a different aspect
                Console.WriteLine("XML serialized: {0}", str);
                byte[] bytes = new byte[str.Length * sizeof(char)];
                System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
                return bytes;
            }
        }

        public object getResult(byte[] content, Type type)
        {
            char[] chars = new char[content.Length / sizeof(char) + 1];
            System.Buffer.BlockCopy(content, 0, chars, 0, content.Length);
            string str = new string(chars);

            StringReader sr = new StringReader(str);
            XmlReader xr = XmlReader.Create(sr);
            XmlSerializer xs = new XmlSerializer(type);
            return xs.Deserialize(xr);
        }
    }
}
