//
// Created by Shiro on 2023/6/14.
//

#ifndef WEBSCAN_MAINWINDOW_H
#define WEBSCAN_MAINWINDOW_H

#include <QMainWindow>
#include "formwindow.h"
#include "src/Utils.h"
#include "src/Html.h"

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

signals:
  
  void sendFormData(QVector<Form> forms);

private:
  void initUrlData();


private:
  Ui::MainWindow *ui;
  QMap<QUrl, QString> url_pages;
  QMap<QUrl, QVector<Form>> url_forms;
  QString current_url;
  QString root_url;
  FormWindow *form_window;
};


#endif //WEBSCAN_MAINWINDOW_H
