/// \file docs/cpp/namespaces.hpp
/// \brief File used by doxygen to generate the documentation of namespaces

/// Toplevel Wolframe namespace
namespace _Wolframe {

	/// Interfaces for authentication, authorization, auditing and accounting
	namespace AAAA {}

	/// Development interfaces for writing Wolframe application extension modules
	namespace appdevel {}

	/// Base64 encoding/decoding functions (implemented in the core library libwolframe)
	namespace base64 {}

	/// Interfaces to command handlers and some basic command handlers (implemented in the library libwolframe_langbind)
	namespace cmdbind {}

	/// Interfaces and templates for configuration structures and parsers
	namespace config {
		/// templates for type dependend characteristics in configuration
		namespace traits {}
	}

	/// Cryprographic structures and functions (implemented in the core library libwolframe)
	namespace crypto {}

	/// Database related classes (implemented in the library libwolframe_database)
	namespace db {
		/// Classes implementing the virtual machine for executing database transactions
		namespace vm {}
	}

	/// Glueing classes for language bindings and filters (implemented in the library libwolframe_langbind)
	namespace langbind {}

	/// Logger structures (implemented in the core library libwfsingleton)
	namespace log {}

	/// Module base classes and object builder structures
	namespace module {}

	/// Network and connection handling
	namespace net {}

	/// Interfaces to programs and the program library
	namespace prgbind {}

	/// Processor provider and execution context
	namespace proc {}

	/// Helper classes for implementing the protocol and handling buffers
	namespace protocol {}

	/// Serialization library (implemented in the library libwolframe_serialize)
	namespace serialize {
		/// templates for type dependend characteristics in serialization
		namespace traits {}
	}

	/// Basic data types (implemented in the core library libwolframe)
	namespace types {
		/// templates for type dependend characteristics in basic types
		namespace traits {}
	}

	/// Utility functions for parsing, handling files, allocating memory, etc. (implemented in the core library libwolframe)
	namespace utils {}
}

/// STL namespace
namespace std {}
