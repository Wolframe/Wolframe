#include "qxtsignalwaiter.h"
#include <QCoreApplication>
#include <QTimerEvent>

QxtSignalWaiter::QxtSignalWaiter(const QObject* sender, const char* signal) : QObject(0) {
    Q_ASSERT(sender && signal);
    connect(sender, signal, this, SLOT(signalCaught()));
}   

QxtSignalWaiter::QxtSignalWaiter(const QObject* sender, const char* signal1, const char *signal2) : QObject(0) {
    Q_ASSERT(sender && signal1);
    Q_ASSERT(sender && signal2);
    connect(sender, signal1, this, SLOT(signalCaught()));
    connect(sender, signal2, this, SLOT(signalCaught()));
}   
    
// Returns true if the signal was caught, returns false if the wait timed out
bool QxtSignalWaiter::wait(const QObject* sender, const char* signal, int msec) {
    QxtSignalWaiter w(sender, signal);
    return w.wait(msec);
}

bool QxtSignalWaiter::wait(const QObject* sender, const char* signal1, const char* signal2, int msec) {
    QxtSignalWaiter w(sender, signal1, signal2);
    return w.wait(msec);
}

// Returns true if the signal was caught, returns false if the wait timed out
bool QxtSignalWaiter::wait(int msec) {
    // Check input parameters
    if(msec < -1) return false;
    
    // activate the timeout
    if(msec != -1) timerID = startTimer(msec);
 
    // Begin waiting   
    ready = timeout = false;
    while(!ready && !timeout)
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
 
    // Clean up and return status
    killTimer(timerID);
    return ready || !timeout;
}

void QxtSignalWaiter::signalCaught() {
    ready = true;
}

void QxtSignalWaiter::timerEvent(QTimerEvent* event) {
    killTimer(timerID);
    timeout = true;
}
