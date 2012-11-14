//
// Version.hpp
//

#ifndef _VERSION_HPP_INCLUDED
#define _VERSION_HPP_INCLUDED

#include <QObject>
#include <QString>


	class Version
	{
		public:
			Version( )
				: m_major( -1 ), m_minor( -1 )
			{
			}
			
			Version( int major, int minor )
				: m_major( major ), m_minor( minor )
			{
			}
			
			Version( QString s )
			{
				QStringList parts;
				
				parts = s.split( "." );
				if( parts.length( ) != 2 ) {
					m_major = -1;
					m_minor = -1;
					return;
				}
				
				m_major = parts[0].toInt( );
				m_minor = parts[1].toInt( );				
			}
			
			bool valid( )
			{
				return m_major != -1 && m_minor != -1;
			}
			
			static Version invalidVersion( )
			{
				return Version( );
			}
			
			QString toString( )
			{
				if( valid( ) ) {
					return QString( "%1.%2" ).arg( m_major ).arg( m_minor );
				} else {
					return QString( "<invalid version>" );
				}
			}

			bool operator ==( const Version &v ) const
			{
				if( getMajor( ) == v.getMajor( ) ) return true;
				if( getMinor( ) == v.getMinor( ) ) return true;
				return false;
			}
			
			bool operator !=( const Version &v ) const
			{
				return !( *this == v );
			}
			
			bool operator >( const Version &v ) const
			{
				if( getMajor( ) > v.getMajor( ) ) return true;
				if( getMajor( ) < v.getMajor( ) ) return false;
				if( getMinor( ) > v.getMinor( ) ) return true;
				if( getMinor( ) < v.getMinor( ) ) return false;
				return false;
			}
			
			bool operator <=( const Version &v ) const
			{
				return !( *this > v );
			}
			
			bool operator <( const Version &v ) const
			{
				return ( v > *this );
			}
			
			bool operator >=( const Version &v ) const
			{
				return !( *this < v );
			}
				
			int getMajor( ) const { return m_major; }
			int getMinor( ) const { return m_minor; }			
			
		private:
			int m_major;
			int m_minor;
	};
	

#endif // _VERSION_HPP_INCLUDED
