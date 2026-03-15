#include <ESP8266WebServer.h>
#include <Arduino.h>
#include"webdata.h"
#include <cstdlib> // atoi需要该头文件
#include "eeprom_flash.h"

extern SYS_CONFIG_T g_sys_cfg;
static SYS_CONFIG_T s_sys_cfg_tmp = {0};
ESP8266WebServer server(80);//开启板子的80端口
// 存储表单提交的数据（示例：6个输入框的数据）
String inputData[8] = {"", "", "", "", "", "", "", ""};


/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
 // server.send(200, "text/html", "<h1>You are connected</h1>");
  //String message = "Enter message to display: <form action=\"/display\"><input type=\"text\" name=\"message\"><input type=\"submit\"></form>";
#if 0
  String message = \
"<form action=\"/display\">"                     \
    "<input type=\"text\" name=\"message\">"     \
    "<input type=\"text1\" name=\"message1\">"     \
    "<input type=\"text2\" name=\"message2\">"     \
    "<input type=\"text3\" name=\"message3\">"     \
    "<input type=\"text4\" name=\"message4\">"     \
    "<input type=\"text5\" name=\"message5\">"     \
    "<input type=\"submit\">"                    \
"</form>";
#endif
  server.send(200, "text/html", message_root);
}

// 处理表单提交请求（解析数据）
void handleFormSubmit() {
  bool is_update_param = 0;
  uint8_t cpy_len;
  uint8_t err_cnt = 1;
  String response = "";
  // 检查请求方法是否为POST
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"success\":false,\"msg\":\"仅支持POST请求\"}");
    return;
  }

  // 解析表单数据（对应前端6个输入框）
  inputData[0] = server.arg("ssid");   // 第一个输入框
  inputData[1] = server.arg("pswd");  // 第二个输入框
  inputData[2] = server.arg("scan_l");
  inputData[3] = server.arg("scan_r");
  inputData[4] = server.arg("speed");
  inputData[5] = server.arg("dist");
  inputData[6] = server.arg("led_f_mode");
  inputData[7] = server.arg("led_b_mode");

  //将数据缓存下来
  cpy_len = inputData[0].length();
  if (cpy_len > 15)
    goto OUT;
  err_cnt ++;
  memcpy(s_sys_cfg_tmp.wifi_name, inputData[0].c_str(), cpy_len);

  cpy_len = inputData[1].length();
  if (cpy_len > 9)
    goto OUT;
  err_cnt ++;
  memcpy(s_sys_cfg_tmp.wifi_password, inputData[1].c_str(), cpy_len);

  cpy_len = atoi(inputData[2].c_str());
  if (cpy_len > 90)
    goto OUT;
  err_cnt ++;
  s_sys_cfg_tmp.scan_angle_left = cpy_len; // 需用c_str()转C风格字符串
  cpy_len = atoi(inputData[3].c_str());
  if (cpy_len > 90)
    goto OUT;
  err_cnt ++;
  s_sys_cfg_tmp.scan_angle_right = cpy_len;
  cpy_len = atoi(inputData[4].c_str());
  if (cpy_len > 200)
    goto OUT;
  err_cnt ++;
  s_sys_cfg_tmp.scan_speed = cpy_len;
  cpy_len = atoi(inputData[5].c_str());
  if (cpy_len > 200)
    goto OUT;
  err_cnt ++;
  s_sys_cfg_tmp.scan_distance = cpy_len;
  cpy_len = atoi(inputData[6].c_str());
  if (cpy_len > 5)
    goto OUT;
  err_cnt ++;
  s_sys_cfg_tmp.led_f_mode = cpy_len;
  cpy_len = atoi(inputData[7].c_str());
  if (cpy_len > 5)
    goto OUT;
  err_cnt ++;
  s_sys_cfg_tmp.led_b_mode = cpy_len;

  // ========== 核心：处理数据（根据你的需求修改） ==========
  // 示例1：打印到串口（调试用）
  Serial.println("===== 接收到表单数据 =====");
  for (int i = 0; i < 8; i++) {
    Serial.printf("input%d:%s\n", i, inputData[i].c_str());
  }

OUT:
  // 返回成功响应给前端
  if (err_cnt < 9)
  {
    inputData[0] = err_cnt;
    response = "{\"success\":false,\"data\":\"" +
                    inputData[0]+ "\"}";
  }
  else{
    response = "{\"success\":true,\"msg\":\"数据接收成功\",\"data\":\"" +
                    inputData[0] + "," + inputData[1] + "," + inputData[2] + "\"}";
  }
  server.send(200, "application/json", response);
}

void web_init()
{
    server.on("/",   handleRoot);
    // 注册路由：处理表单提交
    server.on("/api/submit", handleFormSubmit);
    server.begin();
    Serial.println("HTTP server started");
}

void web_work()
{
    server.handleClient();
}