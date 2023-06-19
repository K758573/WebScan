//
// Created by Shiro on 2023/6/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FormWindow.h" resolved


#include "formwindow.h"
#include "src/Utils.h"
#include <utility>
#include "ui_FormWindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

void readPayload(QVector<QString> &payloads, const QString &filename)
{
  QFile file;
  file.setFileName(filename);
  file.open(QFile::ReadOnly);
  QTextStream qts(&file);
  while (!qts.atEnd())
    payloads.push_back(qts.readLine());
  file.close();
}

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
    current_form = forms.front();
    //纠正参数
    int i = 0;
    for (i; i < arg_names.size(); ++i) {
      if (!arg_names[i]->isHidden()) {
        current_form.args[arg_names[i]->text().toStdString()] = arg_values[i]->text().toStdString();
      }
    }
    auto response = Html::httpRequest(current_form);
    ui->form_response->setHtml(QString::fromStdString(response));
  });
  connect(ui->btn_xss_check, &QPushButton::clicked, this, [&] {
    readPayload(payloads_xss, "xss_payload_list.txt");
    ///xss检测
    ui->message_show->append("构造表单中...");
    Form form = current_form;
    for (auto &item: form.args) {
      item.second = "";
    }
    std::stringstream ssm;
    ssm << form;
    ui->message_show->append("基础表单结构");
    ui->message_show->append(ssm.str().c_str());
    //原始页面
    QString raw_html = QString::fromStdString(Html::httpRequest(current_form));
    size_t raw_count, response_count;
    size_t success_count = 0, sum = 0;
    for (const auto &it: payloads_xss) {
      //发送payload，参数填入第一个输入框
      form.args[arg_names.front()->text().toStdString()] = it.toStdString();
      std::string ret;
      try {
        ret = Html::httpRequest(form);
      } catch (std::exception &e) {
        qDebug() << e.what();
      }
      QString response = QString::fromStdString(ret);
      
      raw_count = raw_html.count(it);
      response_count = response.count(it);
      ui->message_show
        ->append(QString::fromStdString(std::format("payload is : {},\n原payload个数 {} ,回显payload个数 {} ",
                                                    it.toStdString(),
                                                    raw_count,
                                                    response_count)));
      if (response_count > raw_count) {
        ++success_count;
      }
      ++sum;
    }
    ui->message_show->append("尝试了 " + QString::number(sum) + " 个payload ");
    ui->message_show->append(QString::number(success_count) + " 个payload成功回显");
    if (success_count * 2 > sum) {
      ui->message_show->append("大概率存在xss漏洞");
    } else {
      ui->message_show->append("可能存在xss漏洞");
    }
  });
}

FormWindow::~FormWindow()
{
  delete ui;
}

void FormWindow::receiveFormData(QVector<Form> forms_temp)
{
  this->forms.clear();
  this->payloads_xss.clear();
  this->ui->message_show->clear();
  this->ui->form_response->setHtml("");
  
  if (forms_temp.empty()) {
    this->close();
    return;
  }
  this->forms = std::move(forms_temp);
  auto form = this->forms.front();
  current_form = forms.front();
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
