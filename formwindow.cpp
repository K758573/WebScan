//
// Created by Shiro on 2023/6/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FormWindow.h" resolved


#include "formwindow.h"
#include "src/Utils.h"
#include <utility>
#include "ui_FormWindow.h"


FormWindow::FormWindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::FormWindow)
{
  ui->setupUi(this);
  arg_names.push_back(ui->label1);
  arg_names.push_back(ui->label2);
  arg_names.push_back(ui->label3);
  arg_names.push_back(ui->label4);
  arg_names.push_back(ui->label5);
  arg_names.push_back(ui->label6);
  arg_names.push_back(ui->label7);
  arg_names.push_back(ui->label8);
  arg_names.push_back(ui->label9);
  arg_names.push_back(ui->label10);
  arg_values.push_back(ui->lineEdit1);
  arg_values.push_back(ui->lineEdit2);
  arg_values.push_back(ui->lineEdit3);
  arg_values.push_back(ui->lineEdit4);
  arg_values.push_back(ui->lineEdit5);
  arg_values.push_back(ui->lineEdit6);
  arg_values.push_back(ui->lineEdit7);
  arg_values.push_back(ui->lineEdit8);
  arg_values.push_back(ui->lineEdit9);
  arg_values.push_back(ui->lineEdit10);
  connect(ui->pushButton, &QPushButton::clicked, this, [&] {
    current_form = &forms.front();
    //纠正参数
    int i = 0;
    for (i; i < arg_names.size(); ++i) {
      if (!arg_names[i]->isHidden()) {
        current_form->args[arg_names[i]->text().toStdString()] = arg_values[i]->text().toStdString();
      }
    }
    
    auto response = Html::httpRequest(*current_form);
    ui->form_response->setHtml(QString::fromStdString(response));
  });
  
}

FormWindow::~FormWindow()
{
  delete ui;
}

void FormWindow::receiveFormData(QVector<Form> forms_temp)
{
  if (forms_temp.empty()) {
    this->close();
    return;
  }
  this->forms = std::move(forms_temp);
  auto form = this->forms.front();
  ui->label_method->setText(QString::fromStdString(form.method));
  int i = 0;
  for (const auto &it: form.args) {
    arg_names[i]->setText(QString::fromStdString(it.first));
    arg_values[i]->setText(QString::fromStdString(it.second));
    arg_names[i]->show();
    arg_values[i]->show();
    ++i;
  }
  for (i; i < arg_names.size(); ++i) {
    arg_names[i]->hide();
    arg_values[i]->hide();
  }
}
