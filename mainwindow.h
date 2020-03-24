#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include "h264player.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int InitMenuBar();
    bool InitFileMenu(QMenuBar *pMenuBar);

    int ExtracH264(QString filename);

public slots:
    QString openFile();

private:
    Ui::MainWindow *ui;
    bool MakeAction(QAction *&action, QString text, int key);

    QString m_filename;

    H264Player *m_pH264Player;
};
#endif // MAINWINDOW_H
