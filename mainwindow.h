//
// Created by Shiro on 2023/6/14.
//

#ifndef WEBSCAN_MAINWINDOW_H
#define WEBSCAN_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow :
    public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  
  ~MainWindow() override;
private:
  void initUrlData();
private:
  Ui::MainWindow *ui;
  QMap<QUrl,QString> url_pages;
  QString root_url;
};


#endif //WEBSCAN_MAINWINDOW_H
