
#ifndef _MDI_CHILD_HPP_INCLUDED
#define _MDI_CHILD_HPP_INCLUDED

#include <QTextEdit>

class MdiChild : public QTextEdit
{
    Q_OBJECT

public:
    MdiChild();

    bool loadFile(const QString &fileName);

    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    void closeEvent( QCloseEvent *event );

private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;
};

#endif // _MDI_CHILD_HPP_INCLUDED
