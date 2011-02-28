//
// DebugTerminal.hpp
//

#ifndef _DEBUG_TERMINAL_HPP_INCLUDED
#define _DEBUG_TERMINAL_HPP_INCLUDED

#include <QWidget>
#include <QTextEdit>

#include "HistoryLineEdit.hpp"
#include "WolframeClient.hpp"

namespace _Wolframe {
	namespace QtClient {

	class DebugTerminal : public QWidget
	{
	Q_OBJECT

	private:
		QTextEdit *m_output;
		HistoryLineEdit *m_input;
		WolframeClient *m_wolframeClient;

	public:
		DebugTerminal( WolframeClient *_wolframeClient, QWidget *_parent = 0 );
		virtual ~DebugTerminal( );
		void bringToFront( );
	
	private:
		void initialize( );

	protected:
		virtual bool focusNextPrevChild( bool next );

	private slots:
		void lineEntered( QString line );
		void networkError( QString error );
		void lineReceived( QString line );

	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _DEBUG_TERMINAL_HPP_INCLUDED
