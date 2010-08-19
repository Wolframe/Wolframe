//
// connContext.hpp
//

#ifndef _CONNECTION_CONTEXT_HPP_INCLUDED
#define _CONNECTION_CONTEXT_HPP_INCLUDED

namespace _SMERP {

	/// The connection session context
	class connectionContext
	{
	public:
		/// The status of the parser.
		enum connexionStatus_t	{
			EMPTY,
			PARSING,
			READY
		};

	private:

	public:
		connexionStatus_t status();
	};

} // namespace _SMERP

#endif // _CONNECTION_CONTEXT_HPP_INCLUDED
