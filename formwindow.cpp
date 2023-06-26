//
// Created by Shiro on 2023/6/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FormWindow.h" resolved


#include "formwindow.h"
#include "ui_FormWindow.h"
#include "src/Log.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <utility>
#include <QFileDialog>
#include <thread>
#include <chrono>

enum ScanType
{
  SCAN_TYPE_XSS,
  SCAN_TYPE_BF,
  SCAN_TYPE_SQL,
  SCAN_TYPE_RCE,
  SCAN_TYPE_FILE_INCLUSION,
  SCAN_TYPE_SQL_TIME,
  SCAN_TYPE_FORM_PASS,
};


FormWindow::FormWindow(QWidget *parent, HttpRequest &request) :
    QWidget(parent), ui(new Ui::FormWindow), forms(nullptr), request(request)
{
  ui->setupUi(this);
  ui->comboBox->addItem("xss检测", ScanType::SCAN_TYPE_XSS);
  ui->comboBox->addItem("暴力破解", ScanType::SCAN_TYPE_BF);
  ui->comboBox->addItem("表单绕过", ScanType::SCAN_TYPE_FORM_PASS);
  ui->comboBox->addItem("sql注入", ScanType::SCAN_TYPE_SQL);
  ui->comboBox->addItem("时间盲注", ScanType::SCAN_TYPE_SQL_TIME);
  ui->comboBox->addItem("rce检测", ScanType::SCAN_TYPE_RCE);
  ui->comboBox->addItem("文件包含", ScanType::SCAN_TYPE_FILE_INCLUSION);
  ui->comboBox->setCurrentIndex(0);
  //  this->setAttribute(Qt::WA_DeleteOnClose);
  connect(ui->btn_send_form, &QPushButton::clicked, this, &FormWindow::onBtnSendFormClicked);
  connect(ui->btn_select_payload, &QPushButton::clicked, this, &FormWindow::onBtnSelectPayload);
  connect(ui->btn_start, &QPushButton::clicked, this, &FormWindow::onBtnStart);
  connect(this, &FormWindow::messageAdd, ui->message_show, &QTextEdit::append);
  connect(ui->btn_stop, &QPushButton::clicked, this, &FormWindow::onBtnStop);
  
  connect(this, &FormWindow::scanFinished, this, [this] {
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);
  });
}

void FormWindow::onBtnStop()
{
  if (status == THREAD_RUNNING) {
    status = THREAD_FINISHED;
    while (status != THREAD_NOT_START) {
      //wait thread to stop
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  ui->btn_start->setEnabled(true);
  ui->btn_stop->setEnabled(false);
}

void FormWindow::onBtnStart()
{
  if (ui->label_selected_payload->text().isEmpty()) {
    QMessageBox::information(nullptr, "提示", "还未选择payload");
    return;
  }
  ui->btn_start->setEnabled(false);
  ui->btn_stop->setEnabled(true);
  status = THREAD_RUNNING;
  auto st = ui->comboBox->currentData().value<ScanType>();
  switch (st) {
    case SCAN_TYPE_XSS:
      xssCheck();
      break;
    case SCAN_TYPE_BF:
      bfCheck();
      break;
    case SCAN_TYPE_SQL:
      sqlCheck();
      break;
    case SCAN_TYPE_RCE:
      rceCheck();
      break;
    case SCAN_TYPE_FILE_INCLUSION:
      fileInclusionCheck();
      break;
    case SCAN_TYPE_SQL_TIME:
      sqlTimeCheck();
      break;
    case SCAN_TYPE_FORM_PASS:
      onBtnSendFormClicked();
      break;
  }
}


void FormWindow::onBtnSelectPayload()
{
  QString filename = QFileDialog::getOpenFileName(nullptr, "选择使用的payload", "./", "payload (*.txt);;all(*.*)");
  ui->label_selected_payload->setText(filename);
}

FormWindow::~FormWindow()
{
  if (status == THREAD_RUNNING) {
    status = THREAD_FINISHED;
    while (status != THREAD_NOT_START) {
      //wait thread to stop
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  delete ui;
}

void FormWindow::receiveFormData(const QVector<Form> &forms_temp)
{
  //无表单，关闭界面
  if (forms_temp.empty()) {
    this->close();
    QMessageBox::information(nullptr, "提示", "当前页面未发现表单", QMessageBox::Ok);
    return;
  }
  //todo 可能页面有多个表单，目前只显示一个表单的信息
  this->forms = &forms_temp;
  this->form = this->forms->front();
  ///生成与表单参数相同数量的输入框
  ///根据表单内容设定输入框的初始值
  for (const auto &it: this->form.args) {
    auto *label = new QLabel(this->ui->widget);
    auto *edit = new QLineEdit(this->ui->widget);
    this->args.emplace_back(label, edit);
    this->ui->form_layout->addRow(label, edit);
    //name
    label->setText(it.first.c_str());
    //value
    if (it.second.empty()) {
      //无默认值的参数使用$占位
      edit->setText("$");
    } else {
      edit->setText(it.second.c_str());
    }
  }
  this->ui->label_method->setText(this->form.method.c_str());
  this->show();
}

void FormWindow::onBtnSendFormClicked()
{
  loadForm();
  auto response = request(form);
  ui->form_response->setPlainText(QString::fromStdString(response));
  emit scanFinished();
}

void FormWindow::beginCheck(check_function &process, check_print &summary)
{
  //加载表单和payload
  loadForm();
  loadPayloads(ui->label_selected_payload->text());
  //输出一些信息
  std::stringstream ssm;
  ssm << "使用的payload=" << ui->label_selected_payload->text().toStdString() << '\n'
      << std::format("构造表单中...\n基础表单结构:\n{}被用于注入的参数是:", this->form.tostring());
  for (const auto &it: will_be_injected) {
    ssm << args[it].first->text().toStdString() << ',';
  }
  emit messageAdd(ssm.str().c_str());
  assert(process != nullptr);
  success = 0, sum = 0;
  std::thread([this, process, summary] {
    //遍历payload
    Form temp_form = this->form;
    int64_t i = 0, payloads_size = payloads.size();
    QString raw = QString::fromStdString(request(temp_form));
    for (; i < payloads_size; ++i) {
      if (status == THREAD_FINISHED) {
        status = THREAD_NOT_START;
        emit scanFinished();
        return;
      }
      ++sum;
      //填充payload,被置空的参数都将填入payload
      for (const auto &it: will_be_injected) {
        temp_form.args[args[it].first->text().toStdString()] = payloads[i].toStdString();
      }
      //发送请求
      emit messageAdd(std::format("使用的payload={}", payloads[i].toStdString()).c_str());
      QString response = QString::fromStdString(request(temp_form));
      //查看参数是否含有token，自动更新
      if (temp_form.args.contains("token")) {
        //服务器返回了新的token，下一次请求需要使用
        auto token = HtmlUtils::extractToken(response.toStdString());
        temp_form.args["token"] = token;
      }
      //根据使用的payload和响应结果，调用检测函数
      if (!process(payloads[i], response, raw)) {
        //返回false，结束检测
        break;
      }
    }
    //结束输出函数
    if (summary != nullptr) {
      if (i < payloads_size) {
        summary(payloads[i]);
      } else {
        summary(payloads.back());
      }
    }
    emit scanFinished();
    status = THREAD_NOT_START;
  }).detach();
}

void FormWindow::xssCheck()
{
  auto check_function = [this](const QString &payload, const QString &response, const QString &raw) {
    size_t raw_count, response_count;
    raw_count = raw.count(payload);
    response_count = response.count(payload);
    emit messageAdd(std::format("是否回显 : {}", raw_count < response_count).c_str());
    if (response_count > raw_count) {
      ++success;
    }
    return true;
  };
  auto check_print = [this](const QString &payload) {
    emit messageAdd(std::format("尝试了 {} 个payload， {} 个payload成功回显", sum, success).c_str());
    if (success > 0) {
      emit messageAdd("存在xss漏洞");
    } else {
      emit messageAdd("未检测出xss漏洞");
    }
  };
  beginCheck(check_function, check_print);
}

void FormWindow::loadForm()
{
  will_be_injected.clear();
  //  std::ranges::for_each(std::ranges::all_of(form.args),)
  //读取表单数据，标记哪些参数被输入了
  for (int i = 0; i < args.size(); ++i) {
    //输入了一个$符号，认为这个参数没有被输入，这个参数将被用于注入，内部表单参数被置空
    if (args[i].second->text() == "$") {
      will_be_injected.emplace_back(i);
      form.args[args[i].first->text().toStdString()] = "";
    } else {
      form.args[args[i].first->text().toStdString()] = args[i].second->text().toStdString();
    }
  }
  
}

void FormWindow::bfCheck()
{
  beginCheck([this](const auto &payload, const auto &response, const QString &raw) {
    if (response.count("login success") > 0) {
      emit messageAdd("破解成功");
      ++success;
      return false;
    } else {
      return true;
    }
  }, [this](const auto &payload) {
    if (success > 0) {
      emit messageAdd("正确的密码是:" + payload);
    } else {
      emit messageAdd("破解失败");
    }
  });
}

void FormWindow::sqlCheck()
{
  check_function cf = [this](const QString &payload, const QString &response, const QString &raw) -> bool {
    if (response.count("You have an error in your SQL syntax") > 0) {
      ++success;
      return false;
    } else {
      return true;
    }
  };
  check_print cp = [this](const QString &payload) {
    if (success > 0) {
      emit messageAdd("存在sql注入漏洞,payload=" + payload);
    } else {
      emit messageAdd("未发现sql注入漏洞");
    }
  };
  beginCheck(cf, cp);
}

void FormWindow::loadPayloads(const QString &filename)
{
  payloads.clear();
  QFile file;
  file.setFileName(filename);
  file.open(QFile::ReadOnly);
  QTextStream qts(&file);
  while (!qts.atEnd())
    payloads.push_back(qts.readLine());
  file.close();
}

void FormWindow::rceCheck()
{
  check_function cf = [this](const QString &payload, const QString &response, const QString &raw) -> bool {
    if (response.count("special string hello world") > 0) {
      ++success;
      emit messageAdd(std::format("rce是否回显:{}", true).c_str());
    }
    return true;
  };
  check_print cp = [this](const QString &payload) {
    if (success > 0) {
      emit messageAdd("存在rce漏洞");
    } else {
      emit messageAdd("未发现rce漏洞");
    }
  };
  beginCheck(cf, cp);
}

void FormWindow::fileInclusionCheck()
{
  check_function cf = [this](const QString &payload, const QString &response, const QString &raw) -> bool {
    if (response.contains("failed to open stream: Permission denied in")) {
      ++success;
      emit messageAdd(std::format("文件包含是否回显:{}", true).c_str());
    }
    return true;
  };
  check_print cp = [this](const QString &payload) {
    if (success > 0) {
      emit messageAdd("存在文件包含漏洞");
    } else {
      emit messageAdd("未发现文件包含漏洞");
    }
  };
  beginCheck(cf, cp);
}

void FormWindow::sqlTimeCheck()
{
  auto last_time = std::chrono::system_clock::now();
  check_function cf = [this, last_time](const QString &payload, const QString &response,
                                        const QString &raw) mutable -> bool {
    auto current_time = std::chrono::system_clock::now();
    auto diff = current_time - last_time;
    last_time = current_time;
    QString msg("两次请求的间隔为" + QString::number(diff.count() / 10000000) + "秒");
    if (diff.count() > 20000000 && diff.count() < 30000000) {
      msg.append("注入成功");
      emit messageAdd(msg);
      ++success;
      return false;
    } else {
      emit messageAdd(msg);
      return true;
    }
  };
  check_print cp = [this](const QString &payload) {
    if (success > 0) {
      emit messageAdd("存在sql时间盲注，payload=" + payload);
    } else {
      emit messageAdd("未发现sql注入漏洞");
    }
  };
  beginCheck(cf, cp);
}
