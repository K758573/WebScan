#include <QApplication>
#include "mainwindow.h"

int main(int argc ,char** argv)
{
  QApplication app(argc, argv);
  MainWindow main(nullptr);
  main.show();
  return QApplication::exec();
}
