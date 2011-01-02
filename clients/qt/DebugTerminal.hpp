//
// DebugTerminal.hpp
//

#ifndef _DEBUG_TERMINAL_HPP_INCLUDED
#define _DEBUG_TERMINAL_HPP_INCLUDED

#include <QWidget>
#include <QTextEdit>

#include "HistoryLineEdit.hpp"
#include "SMERPClient.hpp"

namespace _SMERP {
	namespace QtClient {

	class DebugTerminal : public QWidget
	{
	Q_OBJECT

	private:
		QTextEdit *m_output;
		HistoryLineEdit *m_input;
		SMERPClient *m_smerpClient;

	public:
		DebugTerminal( SMERPClient *_smerpClient, QWidget *_parent = 0 );
		virtual ~DebugTerminal( );
	
	private:
		void initialize( );

	protected:
		virtual bool focusNextPrevChild( bool next );

	private slots:
		void lineEntered( QString line );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _DEBUG_TERMINAL_HPP_INCLUDED
