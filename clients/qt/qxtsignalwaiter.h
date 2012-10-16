#ifndef QXTSIGNALWAITER_H
#define QXTSIGNALWAITER_H

#include <QObject>
class QTimerEvent;

class QxtSignalWaiter : public QObject {
Q_OBJECT
public:
    QxtSignalWaiter(const QObject* sender, const char* signal);
    QxtSignalWaiter(const QObject* sender, const char* signal1, const char *signal2);
    static bool wait(const QObject* sender, const char* signal, int msec = -1);
    static bool wait(const QObject* sender, const char* signal1, const char* signal2, int msec = -1);
    bool wait(int msec = -1);
protected:
    void timerEvent(QTimerEvent* event);
private slots:
    void signalCaught();
private:
    bool ready, timeout;
    int timerID;
};

#endif
