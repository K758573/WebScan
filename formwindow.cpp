//
// Created by Shiro on 2023/6/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FormWindow.h" resolved


#include "formwindow.h"
#include "src/Utils.h"
#include <utility>
#include "ui_FormWindow.h"
#include "src/Log.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>

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
    QWidget(parent), ui(new Ui::FormWindow), forms(nullptr)
{
  ui->setupUi(this);
  paddingContainer();
  connect(ui->btn_send_form, &QPushButton::clicked, this, &FormWindow::onBtnSendFormClicked);
  connect(ui->btn_xss_check, &QPushButton::clicked, this, &FormWindow::onBtnXssCheckClicked);
  connect(ui->btn_bf_check, &QPushButton::clicked, this, &FormWindow::onBtnBfCheckClicked);
}

FormWindow::~FormWindow()
{
  delete ui;
}

void FormWindow::receiveFormData(const QVector<Form> &forms_temp)
{
  if (forms_temp.empty()) {
    this->close();
    QMessageBox::information(nullptr, "提示", "表单未找到", QMessageBox::Ok);
    return;
  }
  this->forms = &forms_temp;
  this->payloads_xss.clear();
  this->ui->message_show->clear();
  this->ui->form_response->setPlainText("");
  this->will_be_injected.clear();
  //虽然保存了多个表单，但暂时默认只有一个表单
  current_form = this->forms->front();
  ui->label_method->setText(QString::fromStdString(current_form.method));
  unsigned long i = 0;
  for (const auto &it: current_form.args) {
    arg_names[i]->setText(QString::fromStdString(it.first));
    if (it.second.empty()) {
      arg_values[i]->setText("$");
    } else {
      arg_values[i]->setText(QString::fromStdString(it.second));
    }
    arg_names[i]->show();
    arg_values[i]->show();
    ++i;
  }
  for (; i < arg_names.size(); ++i) {
    arg_names[i]->hide();
    arg_values[i]->hide();
  }
  this->show();
}

void FormWindow::paddingContainer()
{
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
  will_be_injected.clear();
}

void FormWindow::onBtnSendFormClicked()
{
  updateForm();
  auto response = Html::httpRequest(current_form);
  ui->form_response->setPlainText(QString::fromStdString(response));
}

void FormWindow::onBtnXssCheckClicked()
{
  updateForm();
  readPayload(payloads_xss, "xss_payload_list.txt");
  ///xss检测
  ui->message_show->append("构造表单中...");
  //临时表单，以当前的输入内容为初始
  Form form = current_form;
  std::stringstream ssm;
  ssm << form;
  ui->message_show->append("基础表单结构");
  ui->message_show->append(ssm.str().c_str());
  ssm.str("");
  ssm << "被用于注入的参数是:\n";
  for (const auto &it: will_be_injected) {
    ssm << arg_names[it]->text().toStdString() << "=\n";
  }
  ui->message_show->append(ssm.str().c_str());
  //原始页面
  QString raw_html = QString::fromStdString(Html::httpRequest(current_form));
  size_t raw_count, response_count;
  size_t success_count = 0, sum = 0;
  for (const auto &payload: payloads_xss) {
    //发送payload,被置空的参数都将填入payload
    for (const auto &it: will_be_injected) {
      form.args[arg_names[it]->text().toStdString()] = payload.toStdString();
    }
    std::string ret;
    try {
      ret = Html::httpRequest(form);
    } catch (std::exception &e) {
      LOG("{}", e.what());
    }
    QString response = QString::fromStdString(ret);
    raw_count = raw_html.count(payload);
    response_count = response.count(payload);
    ui->message_show
      ->append(QString::fromStdString(std::format("payload is : {},\n原payload个数 {} ,回显payload个数 {} ",
                                                  payload.toStdString(),
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
  ui->message_show->append("\n");
}

void FormWindow::updateForm()
{
  will_be_injected.clear();
  //读取表单数据，标记哪些参数被输入了
  for (uint i = 0; i < arg_names.size(); ++i) {
    if (!arg_names[i]->isHidden()) {
      //输入了一个$符号，认为这个参数没有被输入，这个参数将被用于注入，内部表单参数被置空
      if (arg_values[i]->text().toStdString() == "$") {
        will_be_injected.push_back(i);
        current_form.args[arg_names[i]->text().toStdString()] = "";
        continue;
      }
      current_form.args[arg_names[i]->text().toStdString()] = arg_values[i]->text().toStdString();
    }
  }
}

void FormWindow::onBtnBfCheckClicked()
{
  updateForm();
  //读取payload。。。
  payloads_bf.append("123");
  payloads_bf.append("123456");
  payloads_bf.append("123456789");
  payloads_bf.append("12345678900");
  //
  Form form = current_form;
  for (const auto &payload: payloads_bf) {
    //发送payload,被置空的参数都将填入payload
    for (const auto &it: will_be_injected) {
      ui->message_show->append(arg_names[it]->text() + "=" + payload);
      form.args[arg_names[it]->text().toStdString()] = payload.toStdString();
    }
    QString ret = QString::fromStdString(Html::httpRequest(form));
    if (ret.count("login success") > 0) {
      ui->message_show->append("破解成功");
      return;
    }
  }
  
}
