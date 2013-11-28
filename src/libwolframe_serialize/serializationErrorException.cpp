#include "serialize/serializationErrorException.hpp"

using namespace _Wolframe;
using namespace serialize;

SerializationErrorException::SerializationErrorException( const char* title, const std::string& element, const std::string& tag, const std::string& comment)
	:std::runtime_error( std::string() + (title?title:"unknown error") + " (" + element + ")" + (tag.size()?" at ":"") + tag + "-" + comment){}

SerializationErrorException::SerializationErrorException( const char* title, const std::string& element, const std::string& tag)
	:std::runtime_error( std::string() + (title?title:"unknown error") + " (" + element + ")" + (tag.size()?" at ":"") + tag){}

SerializationErrorException::SerializationErrorException( const char* title, const std::string& tag)
	:std::runtime_error( std::string() + (title?title:"unknown error") + (tag.size()?" at ":"") + tag){}

SerializationErrorException::SerializationErrorException( const char* title)
	:std::runtime_error( title?title:"unknown error"){}


