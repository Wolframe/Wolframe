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
        static void ProcessAnswer(Answer answer)
        {
            switch (answer.msgtype)
            {
                case Answer.MsgType.Error:
                    Console.WriteLine("Session error in answer {0}: #{1} {2}", answer.id, answer.number, (string)answer.obj);
                    break;
                case Answer.MsgType.Failure:
                    Console.WriteLine("Request error in answer {0}: #{1} {2}", answer.id, answer.number, (string)answer.obj);
                    break;
                case Answer.MsgType.Result:
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
                var cfg = new Session.Configuration{host="localhost",port=7661,authmethod="NONE"};
                Session session = new Session( cfg, ProcessAnswer);
                if (!session.Connect())
                {
                    Console.WriteLine("Error in connect of session: {0}", session.GetLastError());
                }
                else
                {
                    Customer customer = new Customer { Name = "Ottmar Hitzfeld", Address = "Loerrach Germany" };
                    int answerid = (int)AnswerId.CustomerInsertedObj;

                    int ii = 0;
                    for (ii = 0; ii < 100; ++ii)
                    {
                        Request request = new Request { id = answerid, command = "Insert", number = ii, doctype = "Customer", root = "customer", obj = customer, objtype = typeof(Customer), answertype = typeof(CustomerInserted) };
                        session.IssueRequest(request);
                    }
                    while (session.NofOpenRequests() > 0)
                    {
                        Thread.Sleep(200);
                    }
                    Console.WriteLine("All done");
                    session.Shutdown();
                    session.Close();
                    string err = session.GetLastError();
                    if (err != null)
                    {
                        Console.WriteLine("Error in session: {0}", err);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception in session: {0}", e.Message);
            }
        }
    }
}

