namespace Microsoft.Tools.WixContrib.Extensions
{
    using System;
    using System.Reflection;
    using System.Xml;
    using System.Xml.Schema;
    using Microsoft.Tools.WindowsInstallerXml;

    /// <summary>
    /// The compiler for the Windows Installer XML Toolset Contrib Extension.
    /// </summary>
    public class WixContribCompiler : CompilerExtension
    {
        private XmlSchema schema;

        /// <summary>
        /// Creates a new WixContribCompiler.
        /// </summary>
        public WixContribCompiler()
        {
            this.schema = LoadXmlSchemaHelper(Assembly.GetExecutingAssembly(), "Microsoft.Tools.WixContrib.Extensions.Xsd.wixcontrib.xsd");
        }

        /// <summary>
        /// Gets the schema for this extension.
        /// </summary>
        /// <value>Schema for this extension.</value>
        public override XmlSchema Schema
        {
            get { return this.schema; }
        }

        /// <summary>
        /// Processes an element for the Compiler.
        /// </summary>
        /// <param name="sourceLineNumbers">Source line number for the parent element.</param>
        /// <param name="parentElement">Parent element of element to process.</param>
        /// <param name="element">Element to process.</param>
        /// <param name="contextValues">Extra information about the context in which this element is being parsed.</param>
	public override void ParseElement(SourceLineNumberCollection sourceLineNumbers, XmlElement parentElement, XmlElement element, params string[] contextValues)
	{
		switch( parentElement.LocalName ) {
			case "Property":
//				string propertyId = contextValues[0];
				string propertyId = "bla";
		    
				switch( element.LocalName ) {
					case "XmlSearch":
						this.ParseXmlSearchElement( element, propertyId );
						break;
						
					default:
						this.Core.UnexpectedElement(parentElement, element);
						break;
				}
				break;
		    
                case "Component":
                    string componentId = contextValues[0];
                    string directoryId = contextValues[1];

                    switch (element.LocalName)
                    {
                        case "HttpServerReservation":
                            this.ParseHttpServerReservationElement(element, componentId);
                            break;
                        case "RemoveFolderEx":
                            this.ParseRemoveFolderExElement(element, componentId, directoryId);
                            break;
                        default:
                            this.Core.UnexpectedElement(parentElement, element);
                            break;
                    }
                    break;
                default:
                    this.Core.UnexpectedElement(parentElement, element);
                    break;
            }
        }

	/// <summary>
	/// Parses a XmlSearch element.
	/// </summary>
	/// <param name="node">Element to parse.</param>
	/// <param name="componentId">Identifier of a property node.</param>
	private void ParseXmlSearchElement(XmlNode node, string propertyId)
	{
		SourceLineNumberCollection sourceLineNumbers = Preprocessor.GetSourceLineNumbers( node );
		string id = null;
		string file = null;
		string path = null;
		foreach( XmlAttribute attrib in node.Attributes ) {
			if( attrib.NamespaceURI.Length == 0 || attrib.NamespaceURI == this.schema.TargetNamespace ) {
				switch( attrib.LocalName ) {
					case "Id":
						id = this.Core.GetAttributeIdentifierValue( sourceLineNumbers, attrib );
						break;
						
					case "File":
						file = this.Core.GetAttributeValue( sourceLineNumbers, attrib );
						break;
						
					case "Path":
						path = this.Core.GetAttributeValue( sourceLineNumbers, attrib );
						break;
						
					default:
						this.Core.UnexpectedAttribute( sourceLineNumbers, attrib );
						break;
				}
			} else {
				this.Core.UnsupportedExtensionAttribute(sourceLineNumbers, attrib);
			}
		}

		if( id == null ) {
			this.Core.OnMessage(WixErrors.ExpectedAttribute( sourceLineNumbers, node.Name, "Id" ) );
		}
		
		if( file == null ) {
			this.Core.OnMessage(WixErrors.ExpectedAttribute( sourceLineNumbers, node.Name, "File" ) );
		}

		if( path == null ) {
			this.Core.OnMessage(WixErrors.ExpectedAttribute( sourceLineNumbers, node.Name, "Path" ) );
		}
		
		if( !this.Core.EncounteredError ) {
			Row row = this.Core.CreateRow( sourceLineNumbers, "XmlSearch" );
			row[0] = id;
			row[1] = file;
			row[2] = path;
			this.Core.CreateWixSimpleReferenceRow( sourceLineNumbers, "CustomAction", "XmlSearch" );
		}
	}
	
        /// <summary>
        /// Parses a RemoveFolderEx element.
        /// </summary>
        /// <param name="node">Element to parse.</param>
        /// <param name="componentId">Identifier of parent component.</param>
        private void ParseHttpServerReservationElement(XmlNode node, string componentId)
        {
            SourceLineNumberCollection sourceLineNumbers = Preprocessor.GetSourceLineNumbers(node);
            string id = null;
            string url = null;
            string sddl = null;

            foreach (XmlAttribute attrib in node.Attributes)
            {
                if (0 == attrib.NamespaceURI.Length || attrib.NamespaceURI == this.schema.TargetNamespace)
                {
                    switch (attrib.LocalName)
                    {
                        case "Id":
                            id = this.Core.GetAttributeIdentifierValue(sourceLineNumbers, attrib);
                            break;
                        case "Url":
                            url = this.Core.GetAttributeValue(sourceLineNumbers, attrib);
                            break;
                        case "Sddl":
                            sddl = this.Core.GetAttributeValue(sourceLineNumbers, attrib);
                            break;
                        default:
                            this.Core.UnexpectedAttribute(sourceLineNumbers, attrib);
                            break;
                    }
                }
                else
                {
                    this.Core.UnsupportedExtensionAttribute(sourceLineNumbers, attrib);
                }
            }

            if (null == id)
            {
                this.Core.OnMessage(WixErrors.ExpectedAttribute(sourceLineNumbers, node.Name, "Id"));
            }

            if (null == url)
            {
                this.Core.OnMessage(WixErrors.ExpectedAttribute(sourceLineNumbers, node.Name, "Url"));
            }

            if (null == sddl)
            {
                this.Core.OnMessage(WixErrors.ExpectedAttribute(sourceLineNumbers, node.Name, "Sddl"));
            }

            foreach (XmlNode child in node.ChildNodes)
            {
                if (XmlNodeType.Element == child.NodeType)
                {
                    if (child.NamespaceURI == this.schema.TargetNamespace)
                    {
                        this.Core.UnexpectedElement(node, child);
                    }
                    else
                    {
                        this.Core.UnsupportedExtensionElement(node, child);
                    }
                }
            }

            if (!this.Core.EncounteredError)
            {
                Row row = this.Core.CreateRow(sourceLineNumbers, "HttpServerUrl");
                row[0] = id;
                row[1] = componentId;
                row[2] = url;
                row[3] = sddl;

                this.Core.CreateWixSimpleReferenceRow(sourceLineNumbers, "CustomAction", "InstallHttpServerUrls");
            }
        }

        /// <summary>
        /// Parses a RemoveFolderEx element.
        /// </summary>
        /// <param name="node">Element to parse.</param>
        /// <param name="componentId">Identifier of parent component.</param>
        /// <param name="parentDirectory">Identifier of parent component's directory.</param>
        private void ParseRemoveFolderExElement(XmlNode node, string componentId, string parentDirectory)
        {
            SourceLineNumberCollection sourceLineNumbers = Preprocessor.GetSourceLineNumbers(node);
            string id = null;
            string directory = null;
            int on = CompilerCore.IntegerNotSet;
            string property = null;
            string dirProperty = parentDirectory; // assume the parent directory will be used as the "DirProperty" column.

            foreach (XmlAttribute attrib in node.Attributes)
            {
                if (0 == attrib.NamespaceURI.Length || attrib.NamespaceURI == this.schema.TargetNamespace)
                {
                    switch (attrib.LocalName)
                    {
                        case "Id":
                            id = this.Core.GetAttributeIdentifierValue(sourceLineNumbers, attrib);
                            break;
                        case "Directory":
                            directory = this.Core.GetAttributeIdentifierValue(sourceLineNumbers, attrib);
                            this.Core.CreateWixSimpleReferenceRow(sourceLineNumbers, "Directory", directory);
                            break;
                        case "On":
                            string onValue = this.Core.GetAttributeValue(sourceLineNumbers, attrib);
                            if (onValue.Length == 0)
                            {
                                on = CompilerCore.IllegalInteger;
                            }
                            else
                            {
                                switch (onValue)
                                {
                                    case "install":
                                        on = 1;
                                        break;
                                    case "uninstall":
                                        on = 2;
                                        break;
                                    case "both":
                                        on = 3;
                                        break;
                                    default:
                                        this.Core.OnMessage(WixErrors.IllegalAttributeValue(sourceLineNumbers, node.Name, "On", onValue, "install", "uninstall", "both"));
                                        on = CompilerCore.IllegalInteger;
                                        break;
                                }
                            }
                            break;
                        case "Property":
                            property = this.Core.GetAttributeValue(sourceLineNumbers, attrib);
                            break;
                        default:
                            this.Core.UnexpectedAttribute(sourceLineNumbers, attrib);
                            break;
                    }
                }
                else
                {
                    this.Core.UnsupportedExtensionAttribute(sourceLineNumbers, attrib);
                }
            }

            if (CompilerCore.IntegerNotSet == on)
            {
                this.Core.OnMessage(WixErrors.ExpectedAttribute(sourceLineNumbers, node.Name, "On"));
                on = CompilerCore.IllegalInteger;
            }

            if (null != directory && null != property)
            {
                this.Core.OnMessage(WixErrors.IllegalAttributeWithOtherAttribute(sourceLineNumbers, node.Name, "Property", "Directory", directory));
            }
            else
            {
                if (null != directory)
                {
                    dirProperty = directory;
                }
                else if (null != property)
                {
                    dirProperty = property;
                }
            }

            if (null == id)
            {
                id = String.Concat("Remove", dirProperty);
            }

            foreach (XmlNode child in node.ChildNodes)
            {
                if (XmlNodeType.Element == child.NodeType)
                {
                    if (child.NamespaceURI == this.schema.TargetNamespace)
                    {
                        this.Core.UnexpectedElement(node, child);
                    }
                    else
                    {
                        this.Core.UnsupportedExtensionElement(node, child);
                    }
                }
            }

            if (!this.Core.EncounteredError)
            {
                Row row = this.Core.CreateRow(sourceLineNumbers, "RemoveFoldersEx");
                row[0] = id;
                row[1] = componentId;
                row[2] = dirProperty;
                row[3] = on;

                this.Core.CreateWixSimpleReferenceRow(sourceLineNumbers, "CustomAction", "RemoveFoldersEx");
            }
        }
    }
}
