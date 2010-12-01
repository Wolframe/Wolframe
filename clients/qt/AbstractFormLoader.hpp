//
// AbstractFormLoader.hpp
//

#ifndef _ABSTRACT_FORM_LOADER_HPP_INCLUDED
#define _ABSTRACT_FORM_LOADER_HPP_INCLUDED

#include <QObject>

namespace _SMERP {
	namespace QtClient {

	class AbstractFormLoader : public QObject
	{
		public:
			virtual ~AbstractFormLoader( ) {};
			virtual void initiateListLoad( ) = 0;
			virtual void initiateFormLoad( QString &name ) = 0;
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _ABSTRACT_FORM_LOADER_HPP_INCLUDED
