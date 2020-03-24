#include "mainwindow.h"

#include <QApplication>
#include "h264player.h"
#include <iostream>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();

//    if (argc < 2) {
//        std::cout << "Usage ./player filename" << std::endl;
//    }
//    std::string infilename = argv[1];
//    std::cout << "inifilename = " << infilename << std::endl;

//    H264Player player;
//    player.Init();
//    player.Open(infilename);
//    FILE *outH264File = fopen("myh264.h264", "a+");
//    if (!outH264File) {
//        std::cout << "fopen fail" << std::endl;
//        return -1;
//    }

//    player.ExtractH264(outH264File);

//    return 0;

}
