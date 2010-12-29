//
// DebugTerminal.hpp
//

#ifndef _DEBUG_TERMINAL_HPP_INCLUDED
#define _DEBUG_TERMINAL_HPP_INCLUDED

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>

namespace _SMERP {
	namespace QtClient {

	class DebugTerminal : public QWidget
	{
	Q_OBJECT

	private:
		QTextEdit *m_output;
		QLineEdit *m_input;

	public:
		DebugTerminal( QWidget *_parent = 0 );
		~DebugTerminal( );
	
	private:
		void initialize( );

	private Q_SLOTS:
		void returnPressed( );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _DEBUG_TERMINAL_HPP_INCLUDED
