using System;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Text.RegularExpressions;
using WolframeClient;

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

            var res = Serializer.getRequestContent("profileSite", "profileSite", typeof(ProfileSite), obj);
            Console.WriteLine("\nDocument:\n{0}", System.Text.UTF8Encoding.UTF8.GetString(res));
            ProfileSite inv = (ProfileSite)Serializer.getResult(res, typeof(ProfileSite));
            Console.WriteLine("Object ProfileId: {0}", inv.ProfileId);
            Console.WriteLine("Object SiteId: {0}", inv.SiteId);
            foreach (Link lnk in inv.Links)
            {
                Console.WriteLine("Object Link: {0}", lnk.OriginalUrl);
            }
        }
    }
}

