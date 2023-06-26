//
// Created by Shiro on 2023/6/16.
//

#ifndef WEBSCAN_FORMWINDOW_H
#define WEBSCAN_FORMWINDOW_H

#include <QWidget>
#include <QMap>
#include <QLabel>
#include <QLineEdit>
#include "WebScan.h"


QT_BEGIN_NAMESPACE
namespace Ui { class FormWindow; }
QT_END_NAMESPACE

class FormWindow :
    public QWidget
{
Q_OBJECT
private:
  enum THREAD_STATUS
  {
    THREAD_NOT_START, //线程未开始
    THREAD_FINISHED, //线程正常结束
    THREAD_STOP,  //终止线程
    THREAD_STOPPED, //线程被终止
    THREAD_RUNNING,//线程运行中
  };
  
  QVector<QPair<QLabel *, QLineEdit *>> args;
  QVector<uint8_t> will_be_injected;
  const QVector<Form> *forms;
  QVector<QString> payloads;
  Form form;
  HttpRequest &request;
  std::thread dd;
  int64_t success = 0, sum = 0;
  THREAD_STATUS status = THREAD_NOT_START;
  
  //页面的任务状态
  //检测函数，三个参数为payload，payload响应界面response，默认payload响应界面，返回true表示继续检测
  using check_function = const std::function<bool(const QString &, const QString &, const QString &)>;
  //检测总结函数，参数为最后一次使用的payload
  using check_print = const std::function<void(const QString &)>;

public:
  explicit FormWindow(QWidget *parent, HttpRequest &request);

public slots:
  /**
   * 收到表单数据时进行数据的初始化，窗口的初始化
   * @param forms_temp
   */
  void receiveFormData(const QVector<WebScan::Form> &forms_temp);

private:
  void onBtnSendFormClicked();
  
  void xssCheck();
  
  void bfCheck();
  
  void rceCheck();
  
  void sqlCheck();
  
  void fileInclusionCheck();
  
  void onBtnSelectPayload();
  
  void onBtnStart();
  
  void onBtnStop();
  
  //从输入中读取表单，更新表单结构
  void loadForm();
  
  void loadPayloads(const QString &filename);
  
  
  void beginCheck(check_function &process = nullptr, check_print &summary = nullptr);

signals:
  
  void messageAdd(const QString &msg);
  
  void scanFinished();

private:
  Ui::FormWindow *ui;
public:
  ~FormWindow() override;
};


#endif //WEBSCAN_FORMWINDOW_H
