//
// DebugTerminal.hpp
//

#ifndef _DEBUG_TERMINAL_HPP_INCLUDED
#define _DEBUG_TERMINAL_HPP_INCLUDED

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QStringList>

namespace _SMERP {
	namespace QtClient {

	class DebugTerminal : public QWidget
	{
	Q_OBJECT

	private:
		QTextEdit *m_output;
		QLineEdit *m_input;
		QStringList *m_inputHistory;
		QString m_historyFilename;

	public:
		DebugTerminal( QWidget *_parent = 0 );
		~DebugTerminal( );
	
	private:
		void initialize( );
		void saveHistory( );
		void readHistory( );

	private Q_SLOTS:
		void returnPressed( );

	Q_SIGNALS:
			void formListLoaded( );
			void formLoaded( QString form_name, QByteArray form );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _DEBUG_TERMINAL_HPP_INCLUDED
