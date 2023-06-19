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
  void receiveFormData(QVector<Form> forms_temp);

private:
  Ui::FormWindow *ui;
  QVector<QLabel*> arg_names;
  QVector<QLineEdit*> arg_values;
  QVector<Form> forms;
  QVector<QString> payloads_xss;
  Form current_form;
};


#endif //WEBSCAN_FORMWINDOW_H
