#ifndef GENERATOR_H
#define GENERATOR_H

#include <QtCore/QIODevice>
#include <QtCore/QTime>
#include <QVector>

class Generator : public QIODevice
{
    Q_OBJECT
public:
    Generator(float secs = .5, int freq=500);
    Generator(Generator *copyFrom);
    ~Generator();

    void appendDataFrom(const Generator *copyFrom);
    void start();
    void stop();
    QTime timeLeft();

    void clearBuffer();
    void restartPauses();
    void setupPauses();

    int  trailing;
    int  pos;
    bool finished;
    QVector<qint16> buffer;
    int  m_freq;

public slots:
    void restartData();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);            

private:
    void fillData(int frequency, float seconds);
};



#endif // GENERATOR_H
