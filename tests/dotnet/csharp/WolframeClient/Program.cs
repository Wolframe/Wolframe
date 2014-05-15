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
            switch (answer.msgtype)
            {
                case Session.Answer.MsgType.Error:
                    Console.WriteLine("Session error in answer {0}: #{1} {2}", answer.id, answer.number, (string)answer.obj);
                    break;
                case Session.Answer.MsgType.Failure:
                    Console.WriteLine("Request error in answer {0}: #{1} {2}", answer.id, answer.number, (string)answer.obj);
                    break;
                case Session.Answer.MsgType.Result:
                    switch ((AnswerId)answer.id)
                    {
                        case AnswerId.CustomerInsertedObj:
                            {
                                if (answer.obj == null)
                                {
                                    Console.WriteLine("Got unexpected empty answer. Expected InsertedCustomer object not null");
                                }
                                else
                                {
                                    CustomerInserted customer = (CustomerInserted)answer.obj;
                                    Console.WriteLine("Answer {0}: #{2} Id {1} Name {3} Address {4}", answer.id, answer.number, customer.Id, customer.Name, customer.Address);
                                }
                            }
                            break;
                        default:
                            Console.WriteLine("Request error: unknown aswer type: {0} #{1}", answer.id, answer.number);
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

                    int ii = 0;
                    for (ii = 0; ii < 100; ++ii)
                    {
                        Session.Request request = new Session.Request { id = answerid, command = "Insert", number = ii, doctype = "Customer", root = "customer", obj = customer, objtype = typeof(Customer), answertype = typeof(CustomerInserted) };
                        session.IssueRequest(request);
                    }
                    Console.WriteLine("End...");
                    Thread.Sleep(20000);
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

