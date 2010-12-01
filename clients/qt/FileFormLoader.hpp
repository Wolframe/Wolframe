 //
// FileFormLoader.hpp
//

#ifndef _FILE_FORM_LOADER_HPP_INCLUDED
#define _FILE_FORM_LOADER_HPP_INCLUDED

#include <QObject>

#include "AbstractFormLoader.hpp"

namespace _SMERP {
	namespace QtClient {

	class FileFormLoader : public AbstractFormLoader
	{
	Q_OBJECT
	
		public:
			FileFormLoader( );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _FILE_FORM_LOADER_HPP_INCLUDED
