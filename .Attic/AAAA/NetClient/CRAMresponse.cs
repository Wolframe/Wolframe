using System.Security.Cryptography;

namespace WolframeClient
{
	class CRAMresponse
	{
		private byte[]	digest;

		public CRAMresponse( string password, string challenge )
		{
			// process the challenge
			if ( challenge[0] != '$' )
				throw new System.ArgumentException( "Challenge does not begin with a \"$\"" );
			challenge.TrimStart( '$' );
			string[] parts = challenge.Split( '$' );
			if ( parts.Length != 3 )
				throw new System.ArgumentException( "Invalid challenge format" );
			byte[] salt = System.Convert.FromBase64String( parts[1] );
			byte[] chlng = System.Convert.FromBase64String( parts[2] );
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
				digest = sha.Hash;
			}
		}

		public string outString()
		{
			return System.Convert.ToBase64String( digest );
		}
	}
}
