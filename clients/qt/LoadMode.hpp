//
// LoadMode.hpp
//

#ifndef _LOADMODE_HPP
#define _LOADMODE_HPP

#include <QString>

enum LoadMode {
	Unknown,
	Network,
	LocalFile
};

QString LoadModeToStr( LoadMode m );

LoadMode LoadModeFromStr( const QString s );

#endif // _LOADMODE_HPP
