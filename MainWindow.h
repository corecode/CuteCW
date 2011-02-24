#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioOutput>
#include <QtCore/QSignalMapper>
#include <QtGui/QKeyEvent>
#include "Generator.h"
#include "Morse.h"

namespace Ui {
    class MainWindow;
    class Prefs;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QAudioOutput *createAudioOutput();
    void keyPressEvent(QKeyEvent *);
    bool event(QEvent *event);

public slots:
    void startIt();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    Ui::Prefs      *prefs;
    QAudioOutput   *m_audioOutput;
    Morse          *m_morse;
    QSignalMapper  *m_signalMapper;
};

#endif // MAINWINDOW_H
