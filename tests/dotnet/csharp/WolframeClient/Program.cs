using System;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Text.RegularExpressions;
using System.Threading;
using WolframeClient;

[XmlRoot("customer")]
public class Customer
{
    [XmlElement("name")]
    public string Name { get; set; }
    [XmlElement("address")]
    public string Address { get; set; }
};

[XmlRoot("customer")]
public class CustomerInserted
{
    [XmlElement("id")]
    public int Id { get; set; }
    [XmlElement("name")]
    public string Name { get; set; }
    [XmlElement("address")]
    public string Address { get; set; }
};

enum AnswerId { CustomerInsertedObj = 1 };

namespace WolframeClient
{
    class Program
    {
        static void ProcessAnswer(Session.Answer answer)
        {
            Console.WriteLine("Answer id {0}", answer.id);
            switch (answer.msgtype)
            {
                case Session.Answer.MsgType.Error:
                    Console.WriteLine("Session error: {0}", (string)answer.obj);
                    break;
                case Session.Answer.MsgType.Failure:
                    Console.WriteLine("Request error: {0}", (string)answer.obj);
                    break;
                case Session.Answer.MsgType.Result:
                    switch ((AnswerId)answer.id)
                    {
                        case AnswerId.CustomerInsertedObj:
                            {
                                CustomerInserted customer = (CustomerInserted)answer.obj;
                                Console.WriteLine("Id {0}", customer.Id);
                                Console.WriteLine("Name {0}", customer.Name);
                                Console.WriteLine("Address {0}", customer.Address);
                            }
                            break;
                        default:
                            Console.WriteLine("Request error: unknown aswer type: {0}", answer.id);
                            break;
                    }
                    break;
            }
        }

        static void Main(string[] args)
        {
            try
            {
                Session session = new Session("127.0.0.1", 7661, "NONE", ProcessAnswer);
                if (!session.Connect())
                {
                    Console.WriteLine("Error in connect of session: {0}", session.getLastError());
                }
                else
                {
                    Customer customer = new Customer { Name = "Ottmar Hitzfeld", Address = "Loerrach Germany" };
                    int answerid = (int)AnswerId.CustomerInsertedObj;
                    Session.Request request = new Session.Request { id = answerid, command = "Insert", doctype = "Customer", root = "customer", obj = customer, objtype = typeof(Customer), answertype = typeof(CustomerInserted) };

                    session.IssueRequest(request);
                    Thread.Sleep( 20000);
                    session.Shutdown();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception in session: {0}", e.Message);
            }
        }
    }
}

