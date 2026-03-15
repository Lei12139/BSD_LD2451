#include <ESP8266WebServer.h>
#include <Arduino.h>
#include"webdata.h"

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
  String ssid = server.arg("ssid"); // 获取用户输入的消息
  Serial.println(ssid);
  server.send(200, "text/html", message_root);
}

// 处理表单提交请求（解析数据）
void handleFormSubmit() {
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
  // ========== 核心：处理数据（根据你的需求修改） ==========
  // 示例1：打印到串口（调试用）
  Serial.println("===== 接收到表单数据 =====");
  for (int i = 0; i < 8; i++) {
    Serial.printf("input%d:%s\n", i, inputData[i].c_str());
  }

  // 示例2：控制外设（比如根据数据控制LED）
  // if (inputData[0] == "on") digitalWrite(2, HIGH); // 2号引脚亮LED
  // if (inputData[0] == "off") digitalWrite(2, LOW);  // 2号引脚灭LED

  // 示例3：存储到Flash（需安装Preferences库）
  // Preferences prefs;
  // prefs.begin("formData", false);
  // prefs.putString("msg0", inputData[0]);
  // prefs.end();

  // 返回成功响应给前端
  String response = "{\"success\":true,\"msg\":\"数据接收成功\",\"data\":\"" + 
                    inputData[0] + "," + inputData[1] + "," + inputData[2] + "\"}";
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