using System.Security.Cryptography;
using System;
using System.Text;

namespace WolframeClient
{
	class UsernameHash
	{
		private string hash;

		public UsernameHash( string username )
		{
			byte[] seed = new byte[16];
			using ( RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider() ) {
				rng.GetBytes( seed );
			}
			byte[] uname = Encoding.ASCII.GetBytes( username );
			using ( var hmacSHA256 = new HMACSHA256( seed ) ) {
				hmacSHA256.ComputeHash( uname );

				hash = "$" + System.Convert.ToBase64String( seed ) +
					"$" + System.Convert.ToBase64String( hmacSHA256.Hash );
			}
		}

		public UsernameHash( string seed, string username )
		{
			byte[] key = System.Convert.FromBase64String( seed );
			byte[] uname = Encoding.ASCII.GetBytes( username );
			using ( var hmacSHA256 = new HMACSHA256( key ) ) {
				hmacSHA256.ComputeHash( uname );

				hash = "$" + System.Convert.ToBase64String( key ) +
					"$" + System.Convert.ToBase64String( hmacSHA256.Hash );
			}
		}

		public string outString()
		{
			return hash;
		}

	}
}
