using System;

namespace WolframeClient
{
	class Program
	{
		static void Main( string[] args )
		{
			UsernameHash hash = new UsernameHash( "q9QPp8YscSmkIpDyVMNsyw==", "Admin" );
			Console.WriteLine( "Username hash: " + hash.outString() );
			CRAMresponse response = new CRAMresponse( "Good Password",
				"$w5Ca0wRXmJfcFYaliE3fiw==$z2OIj/bKF61bO6UKUyrWEAW61pWnldXOOzvkJnEVYGx/eCbj9Ei0ozPFMzMJT5RrcEJw152JnNW0p8aEyVJcew==" );
			Console.WriteLine( "CRAM response: " + response.outString() );
		}
	}
}
