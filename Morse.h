#ifndef MORSE_H
#define MORSE_H

#include <QtCore/QString>
#include <QtMultimedia/QAudioOutput>
#include <QtCore/QMap>
#include <QtCore/QList>

#include "Generator.h"
#include "MorseStat.h"

class Morse : public QObject
{
public:
    Q_OBJECT

public:
    Morse();
    Morse(QAudioOutput *output);

    enum ditdah{ DIT, DAH };

    enum mode { PLAY, TRAIN, TEST };
    enum playingmode { STOPPED, PLAYING };

    void setSequence(const QString &sequence);
    void clearList();
    void add(Generator *nextsound);
    void add(QChar c, bool addpause = true);
    void addAndPlayIt(QChar c);
    void createTones(float ditSecs, int dahMult = 3, int pauseMult = 3, int spaceMult = 6);

    Generator *dit();
    Generator *dah();
    Generator *pause();
    Generator *space();

public slots:
    void playSequence();
    void maybePlaySequence();
    void nextSequence(QAudio::State state);
    void keyPressed(QString newtext);
    void switchMode(int newMode);

private:
    QAudioOutput                    *m_audioOutput;
    Generator                       *m_dit, *m_dah, *m_space, *m_pause;
    Generator                       *m_playBuffer;
    QMap<QChar, QList<ditdah> *>    code;
    playingmode                     m_playingMode;
    mode                            m_gameMode;
    int                             m_currentWPMGoal;
    QMap<QChar, MorseStat *>        m_stats;
};

#endif // MORSE_H
