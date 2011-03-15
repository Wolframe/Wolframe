#ifndef _Wolframe_LUATYPES_HPP_INCLUDED
#define _Wolframe_LUATYPES_HPP_INCLUDED
#include "methodtable.hpp"
#include "protocol/generator.hpp"

namespace _Wolframe {
namespace mtproc {

class Interpreter
{
	struct State;
	Method::Context context;
	State* state;

	Interpreter();
	~Interpreter();

	bool passConfig( const char* method, protocol::Generator* config);
	int call( unsigned int argc, const char** argv);
};

}}//namespace
#endif

