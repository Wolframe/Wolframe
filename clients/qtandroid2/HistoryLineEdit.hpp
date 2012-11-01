//
// HistoryLineEdit.hpp
//

#ifndef _HISTORY_LINE_EDIT_HPP_INCLUDED
#define _HISTORY_LINE_EDIT_HPP_INCLUDED

#include <QLineEdit>
#include <QStringList>
#include <QKeyEvent>

namespace _Wolframe {
	namespace QtClient {

	class HistoryLineEdit : public QLineEdit
	{
	Q_OBJECT

	private:
		QStringList *m_history;
		QString m_historyFilename;
		int m_pos;

	private:
		void initialize( );
		void saveHistory( );
		void readHistory( );

	public:
		HistoryLineEdit( QWidget *_parent = 0 );
		virtual ~HistoryLineEdit( );
		void complete( );

	signals:
		void lineEntered( QString line );

	private slots:
		void returnPressed( );

	protected:
		virtual void keyPressEvent( QKeyEvent *event );
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _HISTORY_LINE_EDIT_HPP_INCLUDED
