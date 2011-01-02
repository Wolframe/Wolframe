//
// SMERPClient.hpp
//

#ifndef _SMERP_CLIENT_HPP_INCLUDED
#define _SMERP_CLIENT_HPP_INCLUDED

#include <QObject>

namespace _SMERP {
	namespace QtClient {

	class SMERPClient : public QObject
	{
	Q_OBJECT

	public:
		SMERPClient( );
		virtual ~SMERPClient( );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _SMERP_CLIENT_HPP_INCLUDED
