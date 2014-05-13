

namespace Wolframe
{
	class SSLClient
	{
		void  SendDataToServer( string message_)
		{
			byte [] buffer =  Encoding .UTF8.GetBytes( message_);
			m_sslStream.Write(buffer, 0, buffer.Length);
		} 
	};
}

