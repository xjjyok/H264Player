#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pH264Player = new H264Player;
    m_pH264Player->Init();

    //InitMenuBar();
    QMenuBar *mBar = menuBar();
    // add menu
    QMenu *pFileMenu = mBar->addMenu("File");
    // add action
    QAction *pOpenAct = pFileMenu->addAction("Open");
    //connect(pOpenAct, &QAction::triggered, )
    connect(pOpenAct, SIGNAL(triggered()), this, SLOT(openFile()));

    pFileMenu->addAction(pOpenAct);
    mBar->addMenu(pFileMenu);

    // add toolbar
    QToolBar *toolBar = addToolBar("toolBar");
    // add shorcut
    toolBar->addAction(pOpenAct);
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_pH264Player) {
        delete m_pH264Player;
        m_pH264Player = nullptr;
    }
}

int MainWindow::InitMenuBar()
{
    bool nRet = true;

    QMenuBar *pMenuBar = this->menuBar();
    if (!pMenuBar) {
        return -1;
    }

    InitFileMenu(pMenuBar);

    return 0;
}

int MainWindow::ExtracH264(QString filename)
{
    FILE *outH264File = fopen(filename.toStdString().c_str(), "a+");
    if (!outH264File) {
        std::cout << "fopen fail" << std::endl;
        return -1;
    }

    m_pH264Player->ExtractH264(outH264File);
}

bool MainWindow::InitFileMenu(QMenuBar *pMenuBar)
{
    if (!pMenuBar) {
        return false;
    }

    QMenu *fileMenu = new QMenu("File(&F)");
    if (!fileMenu) {
        return false;
    }

    QAction *action = nullptr;
    int ret = MakeAction(action, "Open(&O)", Qt::CTRL + Qt::Key_O);
    if (ret) {
        fileMenu->addAction(action);
    }

    pMenuBar->addMenu(fileMenu);
}

QString MainWindow::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select a video file", "./", "ALL(*.*)");
    m_filename = filename;

    std::cout << "m_filename = " << filename.toStdString() << std::endl;

    m_pH264Player->Open(filename.toStdString());

    ExtracH264("myH264.h264");

    return filename;
}

bool MainWindow::MakeAction(QAction *&action, QString text, int key)
{
    action = new QAction(text, nullptr);
    if (action != nullptr) {
        action->setShortcut(QKeySequence(key));
        return true;
    }

    return false;
}

