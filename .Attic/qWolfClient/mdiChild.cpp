#include <QtGui>

#include "mdiChild.hpp"

MdiChild::MdiChild()
{
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}

bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
	QMessageBox::warning(this, tr("MDI"),
			     tr("Cannot read file %1:\n%2.")
			     .arg(fileName)
			     .arg(file.errorString()));
	return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), SIGNAL(contentsChanged()),
	    this, SLOT(documentWasModified()));

    return true;
}


QString MdiChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void MdiChild::closeEvent( QCloseEvent *event )
{
	if ( maybeSave())	{
		event->accept();
	}
	else	{
		event->ignore();
	}
}

void MdiChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

bool MdiChild::maybeSave()
{
    if (document()->isModified()) {
	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(this, tr("MDI"),
		     tr("'%1' has been modified.\n"
			"Do you want to save your changes?")
		     .arg(userFriendlyCurrentFile()),
		     QMessageBox::Save | QMessageBox::Discard
		     | QMessageBox::Cancel);
	if (ret == QMessageBox::Save)
		;
//	    return save();
	else if (ret == QMessageBox::Cancel)
	    return false;
    }
    return true;
}

void MdiChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
