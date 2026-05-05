#include <ESP8266WebServer.h>
#include <Arduino.h>
#include "webdata.h"
#include <cstdlib> // atoi需要该头文件
#include "eeprom_flash.h"

#define SSID_OFFSET 1866
#define PSWD_OFFSET 4020
#define ALEFT_OFFSET 4329
#define ARIGH_OFFSET 4480
#define SPEED_OFFSET 4624
#define DIST_OFFSET 4762

#define LEDF_OFFSET_0 5092
#define LEDF_OFFSET_1 5151
#define LEDF_OFFSET_2 5210
#define LEDF_OFFSET_3 5277
#define LEDB_OFFSET_0 5782
#define LEDB_OFFSET_1 5841
#define LEDB_OFFSET_2 5900
#define LEDB_OFFSET_3 5967

extern SYS_CONFIG_T g_sys_cfg;
static SYS_CONFIG_T s_sys_cfg_tmp = {0};
ESP8266WebServer server(80); // 开启板子的80端口
// 存储表单提交的数据（示例：6个输入框的数据）
String inputData[8] = {"", "", "", "", "", "", "", ""};
static const uint32_t ledf_point[4] = {LEDF_OFFSET_0, LEDF_OFFSET_1, LEDF_OFFSET_2, LEDF_OFFSET_3};
static const uint32_t ledb_point[4] = {LEDB_OFFSET_0, LEDB_OFFSET_1, LEDB_OFFSET_2, LEDB_OFFSET_3};

void webdata_update(SYS_CONFIG_T *sys_cfg);
/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot()
{
  // server.send(200, "text/html", "<h1>You are connected</h1>");
  // String message = "Enter message to display: <form action=\"/display\"><input type=\"text\" name=\"message\"><input type=\"submit\"></form>";
  server.send(200, "text/html", message_root);
}

// 处理表单提交请求（解析数据）
void handleFormSubmit()
{
  bool is_update_param = 0;
  uint8_t cpy_len;
  uint8_t err_cnt = 1;
  String response = "";
  // 检查请求方法是否为POST
  if (server.method() != HTTP_POST)
  {
    server.send(405, "application/json", "{\"success\":false,\"msg\":\"仅支持POST请求\"}");
    return;
  }

  // 解析表单数据（对应前端6个输入框）
  inputData[0] = server.arg("ssid"); // 第一个输入框
  inputData[1] = server.arg("pswd"); // 第二个输入框
  inputData[2] = server.arg("scan_l");
  inputData[3] = server.arg("scan_r");
  inputData[4] = server.arg("speed");
  inputData[5] = server.arg("dist");
  inputData[6] = server.arg("led_f_mode");
  inputData[7] = server.arg("led_b_mode");
  memcpy(&s_sys_cfg_tmp, &g_sys_cfg, SYS_CONFIG_LEN);
  // 将数据缓存下来
  cpy_len = inputData[0].length();
  if ((cpy_len > 1) && (cpy_len <= 15))
  {
    err_cnt++;
    memcpy(s_sys_cfg_tmp.wifi_name, inputData[0].c_str(), cpy_len);
  }

  cpy_len = inputData[1].length();
  if ((cpy_len > 7) && (cpy_len <= 9))
  {
    err_cnt++;
    memcpy(s_sys_cfg_tmp.wifi_password, inputData[1].c_str(), cpy_len);
  }

  cpy_len = atoi(inputData[2].c_str());
  if ((cpy_len > 5) && (cpy_len <= 90))
  {
    err_cnt++;
    s_sys_cfg_tmp.scan_angle_left = cpy_len; // 需用c_str()转C风格字符串
  }

  cpy_len = atoi(inputData[3].c_str());
  if ((cpy_len > 5) && (cpy_len <= 90))
  {
    err_cnt++;
    s_sys_cfg_tmp.scan_angle_right = cpy_len;
  }

  cpy_len = atoi(inputData[4].c_str());
  if ((cpy_len >= 10) && (cpy_len <= 200))
  {
    err_cnt++;
    s_sys_cfg_tmp.scan_speed = cpy_len;
  }

  cpy_len = atoi(inputData[5].c_str());
  if ((cpy_len > 2) && (cpy_len <= 200))
  {
    err_cnt++;
    s_sys_cfg_tmp.scan_distance = cpy_len;
  }

  cpy_len = atoi(inputData[6].c_str());
  if (cpy_len <= 5)
  {
    err_cnt++;
    s_sys_cfg_tmp.led_f_mode = cpy_len;
  }

  cpy_len = atoi(inputData[7].c_str());
  if (cpy_len <= 5)
  {
    err_cnt++;
    s_sys_cfg_tmp.led_b_mode = cpy_len;
  }
  // ========== 核心：处理数据（根据你的需求修改） ==========
  // 示例1：打印到串口（调试用）
  Serial.println("===== 接收到表单数据 =====");
  for (int i = 0; i < 8; i++)
  {
    Serial.printf("input%d:%s\n", i, inputData[i].c_str());
  }

  // 返回成功响应给前端
  if (err_cnt < 9)
  {
    inputData[0] = err_cnt;
    response = "{\"success\":false,\"data\":\"" +
               inputData[0] + "\"}";
  }
  else
  {
    response = "{\"success\":true,\"msg\":\"数据接收成功\",\"data\":\"" +
               inputData[0] + "," + inputData[1] + "," + inputData[2] + "\"}";
  }
  server.send(200, "application/json", response);

  webdata_update(&s_sys_cfg_tmp);
  char buffer[40];
  Serial.println(s_sys_cfg_tmp.wifi_name);
  Serial.println(s_sys_cfg_tmp.wifi_password);
  sprintf(buffer, "scan_angle_left = %d", s_sys_cfg_tmp.scan_angle_left);
  Serial.println(buffer);
  sprintf(buffer, "scan_angle_right = %d", s_sys_cfg_tmp.scan_angle_right);
  Serial.println(buffer);
  sprintf(buffer, "scan_speed = %d", s_sys_cfg_tmp.scan_speed);
  Serial.println(buffer);
  sprintf(buffer, "scan_distance = %d", s_sys_cfg_tmp.scan_distance);
  Serial.println(buffer);
  sprintf(buffer, "led_f_mode = %d", s_sys_cfg_tmp.led_f_mode);
  Serial.println(buffer);
  sprintf(buffer, "led_b_mode = %d", s_sys_cfg_tmp.led_b_mode);
  Serial.println(buffer);
}
void webdata_update(SYS_CONFIG_T *sys_cfg)
{
  char tmp[20] = {0};
  uint32_t index, len;
  memcpy(message_root + SSID_OFFSET, sys_cfg->wifi_name, strlen(sys_cfg->wifi_name));
  index = SSID_OFFSET + strlen(sys_cfg->wifi_name);
  message_root[index] = '\"';

  memcpy(message_root + PSWD_OFFSET, sys_cfg->wifi_password, strlen(sys_cfg->wifi_password));
  index = PSWD_OFFSET + strlen(sys_cfg->wifi_password);
  message_root[index] = '\"';

  len = sprintf(tmp, "%d", sys_cfg->scan_angle_left);
  memcpy(message_root + ALEFT_OFFSET, tmp, len);
  index = ALEFT_OFFSET + len;
  message_root[index] = '\"';

  len = sprintf(tmp, "%d", sys_cfg->scan_angle_right);
  memcpy(message_root + ARIGH_OFFSET, tmp, len);
  index = ARIGH_OFFSET + len;
  message_root[index] = '\"';

  len = sprintf(tmp, "%d", sys_cfg->scan_speed);
  memcpy(message_root + SPEED_OFFSET, tmp, len);
  index = SPEED_OFFSET + len;
  message_root[index] = '\"';

  len = sprintf(tmp, "%d", sys_cfg->scan_distance);
  memcpy(message_root + DIST_OFFSET, tmp, len);
  index = DIST_OFFSET + len;
  message_root[index] = '\"';

  for (int i = 0; i < 4; i++)
  {
    memset(message_root + ledf_point[i], 0x20, 8);
    memset(message_root + ledb_point[i], 0x20, 8);
  }
  memcpy(message_root + ledf_point[sys_cfg->led_f_mode], "selected", 8);
  memcpy(message_root + ledb_point[sys_cfg->led_b_mode], "selected", 8);
}
void web_init()
{
  webdata_update(&g_sys_cfg);
  server.on("/", handleRoot);
  // 注册路由：处理表单提交
  server.on("/api/submit", handleFormSubmit);
  server.begin();
  Serial.println("HTTP server started");
}

void web_work()
{
  server.handleClient();
}

void web_close()
{
  server.stop();
}