//
// Created by Shiro on 2023/6/16.
//

#ifndef WEBSCAN_FORMWINDOW_H
#define WEBSCAN_FORMWINDOW_H

#include <QWidget>
#include <QMap>
#include <QLabel>
#include <QLineEdit>
#include "src/Html.h"


QT_BEGIN_NAMESPACE
namespace Ui { class FormWindow; }
QT_END_NAMESPACE

class FormWindow :
    public QWidget
{
Q_OBJECT

public:
  explicit FormWindow(QWidget *parent = nullptr);
  
  ~FormWindow() override;

public slots:
  void receiveFormData(const QVector<Form>& forms_temp);
  void receiveCookie(const std::list<std::string> *cookie_list);
  void onBtnXssCheckClicked();
  void onBtnSendFormClicked();

private:
  //从输入中读取表单，更新表单结构
  void updateForm();
  //填充结构体
  void paddingContainer();

private:
  Ui::FormWindow *ui;
  QVector<QLabel*> arg_names;
  QVector<QLineEdit*> arg_values;
  QVector<uint8_t> will_be_injected;
  const QVector<Form> *forms;
  QVector<QString> payloads_xss;
  Form current_form;
  const std::list<std::string> *cookie;
};


#endif //WEBSCAN_FORMWINDOW_H
