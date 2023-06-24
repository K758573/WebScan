//
// Created by Shiro on 2023/6/14.
//

#ifndef WEBSCAN_MAINWINDOW_H
#define WEBSCAN_MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include "formwindow.h"
#include "WebScan.h"

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
  
  void sendFormData(const QVector<Form>& forms);

private slots:
  /**
   * 点击抓取按钮，提取所输入的网页的url，保存到到url_pages中
   */
  void onButtonGetUrlListClicked();
  /**
   * 点击链接树的节点，更新对应的界面
   * @param index 链接对应的索引
   */
  void onTreeUrlListClicked(QModelIndex index);
  
  /**
   * 点击提取表单，生成一个提取表单的界面，刷新当前界面，提取cookie
   */
  void onActionExtractForm();

private:
  Ui::MainWindow *ui;
  //保存的是完整的url地址
  QMap<QString, QString> url_pages;
  QMap<QString, QVector<Form>> url_forms;
  QString root_url;
  FormWindow *form_window;
  HttpRequest request;
};


#endif //WEBSCAN_MAINWINDOW_H
