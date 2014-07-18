/// \file docs/cpp/namespaces.hpp
/// \brief File used by doxygen to generate the documentation of namespaces

/// Toplevel Wolframe namespace
namespace _Wolframe {
/// Authentication, authorization, auditing and accounting
namespace AAAA {}
/// Development interfaces for writing Wolframe application extension modules
namespace appdevel {}
/// Base64 encoding/decoding functions
namespace base64 {}
/// Command handlers and programs
namespace cmdbind {}
/// Configuration structures and parsers
namespace config {
/// templates for type dependend characteristics in configuration
namespace traits {}
}
/// Cryprographic structures and functions
namespace crypto {}
/// Database related classes
namespace db {
/// Classes implementing the virtual machine for executing database transactions
namespace vm {}
}
/// Glueing classes for language bindings and filters
namespace langbind {}
/// Logger structures
namespace log {}
/// Module base objects and object builder structures
namespace module {}
/// Network and connection handling
namespace net {}
/// Program library structures
namespace prgbind {}
/// Processor provider and execution context
namespace proc {}
/// Classes for implementing the protocol
namespace protocol {}
/// Serialization library
namespace serialize {
/// templates for type dependend characteristics in serialization
namespace traits {}
}
/// Basic data types
namespace types {
/// templates for type dependend characteristics in basic types
namespace traits {}
}
/// Utility functions
namespace utils {}
}
/// STL namespace
namespace std {}

