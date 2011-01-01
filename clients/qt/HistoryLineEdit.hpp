//
// HistoryLineEdit.hpp
//

#ifndef _HISTORY_LINE_EDIT_HPP_INCLUDED
#define _HISTORY_LINE_EDIT_HPP_INCLUDED

#include <QLineEdit>
#include <QStringList>

namespace _SMERP {
	namespace QtClient {

	class HistoryLineEdit : public QLineEdit
	{
	Q_OBJECT

	private:
		QStringList *m_history;
		QString m_historyFilename;

	private:
		void initialize( );
		void saveHistory( );
		void readHistory( );

	public:
		HistoryLineEdit( QWidget *_parent = 0 );
		virtual ~HistoryLineEdit( );

	signals:
		void lineEntered( QString line );

	private slots:
		void returnPressed( );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _HISTORY_LINE_EDIT_HPP_INCLUDED
