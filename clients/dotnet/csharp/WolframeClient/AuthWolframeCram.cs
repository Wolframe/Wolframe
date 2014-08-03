using System.Security.Cryptography;
using System;
using System.Text;

namespace WolframeClient
{
	class AuthWolframeCram
	{
		public static string UsernameHash( string username )
		{
			byte[] seed = new byte[16];
			using ( RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider() ) {
				rng.GetBytes( seed );
			}
			byte[] uname = Encoding.ASCII.GetBytes( username );
			using ( var hmacSHA256 = new HMACSHA256( seed ) ) {
				hmacSHA256.ComputeHash( uname );

				return "$" + System.Convert.ToBase64String( seed ) +
					"$" + System.Convert.ToBase64String( hmacSHA256.Hash );
			}
		}

		public static string UsernameHash( string seed, string username )
		{
			byte[] key = System.Convert.FromBase64String( seed );
			byte[] uname = Encoding.ASCII.GetBytes( username );
			using ( var hmacSHA256 = new HMACSHA256( key ) ) {
				hmacSHA256.ComputeHash( uname );

				return "$" + System.Convert.ToBase64String( key ) +
					"$" + System.Convert.ToBase64String( hmacSHA256.Hash );
			}
		}

        public static string CRAMresponse( string password, string challenge )
		{
			// process the challenge
			if ( challenge[0] != '$' )
				throw new System.ArgumentException( "Challenge does not begin with a \"$\"" );
			challenge.TrimStart( '$' );
			string[] parts = challenge.Split( '$' );
			if ( parts.Length != 3 )
				throw new System.ArgumentException( "Invalid challenge format" );

            string saltBase64 = parts[1];
            while (saltBase64.Length % 4 != 0) saltBase64 += "=";
            byte[] salt = System.Convert.FromBase64String(saltBase64);

            string chlngBase64 = parts[2];
            while (chlngBase64.Length % 4 != 0) chlngBase64 += "=";
            byte[] chlng = System.Convert.FromBase64String(chlngBase64);

			if( chlng.Length != 64 )
				throw new System.ArgumentException( "Invalid challenge length" );
			Rfc2898DeriveBytes pwdHash = new Rfc2898DeriveBytes( password, salt, 10589 );
			byte[] passwd = pwdHash.GetBytes( 48 );

			byte[] response = new byte[64];
			for ( int i = 0; i < passwd.Length; i++ )
				response[i] = passwd[i];
			for ( int i = passwd.Length; i < 64; i++ )
				response[i] = 0x3c;
			for ( int i = 0; i < 64; i++ )
				response[i] ^= chlng[i];
			using ( SHA256 sha = SHA256.Create() ) {
				sha.ComputeHash( response );
                byte[] digest = sha.Hash;
                return System.Convert.ToBase64String(digest);
			}
		}
	}
}
