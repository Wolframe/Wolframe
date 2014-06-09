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
            settings.Indent = false;
            settings.OmitXmlDeclaration = false;
            settings.Encoding = new UTF8Encoding(false/*no BOM*/, true/*throw if input illegal*/);

            XmlSerializerNamespaces xmlNameSpace = new XmlSerializerNamespaces();
            xmlNameSpace.Add("xsi", "http://www.w3.org/2001/XMLSchema-instance");    
            xmlNameSpace.Add("noNamespaceSchemaLocation", m_schemadir + "/" + doctype + "." + m_schemaext);

            StringWriter sw = new StringWriter();
            XmlWriter xw = XmlWriter.Create( sw, settings);
            xw.WriteProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");

            serializer.Serialize(xw, obj, xmlNameSpace);

            return settings.Encoding.GetBytes( sw.ToString());
        }

        public object getResult(byte[] content, Type type)
        {
            string str = System.Text.UTF8Encoding.UTF8.GetString(content);

            StringReader sr = new StringReader(str);
            XmlReader xr = XmlReader.Create(sr);
            XmlSerializer xs = new XmlSerializer(type);
            try
            {
                return xs.Deserialize(xr);
            }
            catch (System.InvalidOperationException e)
            {
                return null;
            }
        }
    }
}
