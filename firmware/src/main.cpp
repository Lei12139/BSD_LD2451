#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include "eeprom_flash.h"
#include "web.h"
#include "ld2451.h"

#define LED_BUILTIN 2

Ticker ticker_1s;   // 建立Ticker用于实现定时功能
Ticker ticker_20ms; // 建立Ticker用于实现定时功能
Ticker ticker_5min; // 建立Ticker用于实现定时功能

SYS_CONFIG_T g_sys_cfg;
int tick_cnt;
int is_wifi_on;
// 亮灯任务
void task_led1_H()
{
  static uint8_t dutyCycle = 0;
  dutyCycle += 2;
  // 读取模拟数值
  analogWrite(LED_BUILTIN, dutyCycle);
}
// 旋钮任务
void task_knob()
{
  tick_cnt++;
  Serial.printf("HTTP server started %d\n", tick_cnt);
}
// 定时关闭wifi
void task_ap_web()
{
  is_wifi_on = 0;
  web_close();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  ticker_1s.detach();
}

void setup()
{
  delay(1000);
  Serial.begin(115200);
  delay(10);
  Serial.println();

  // eeprom init
  eeprom.load_all_para(&g_sys_cfg);

  // WiFi热点初始化
  Serial.print("Configuring access point:");
  Serial.println(g_sys_cfg.wifi_name);
  // wifi_get_macaddr(SOFTAP_IF, mac);
  WiFi.softAP(g_sys_cfg.wifi_name, g_sys_cfg.wifi_password); // 开启WiFi热点
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // 网页初始化
  web_init();

  // pin初始化
  pinMode(LED_BUILTIN, OUTPUT); // 初始化引脚为输出模式

  ld2451_setup();
  // 每隔一秒钟调用sayHi函数一次，attach函数的第一个参数
  // 是控制定时间隔的变量。该参数的单位为秒。第二个参数是
  // 定时执行的函数名称。
  ticker_1s.attach(2, task_knob);
  ticker_20ms.attach_ms(20, task_led1_H);
  ticker_5min.once(5 * 60, task_ap_web);
  tick_cnt = 0;
  is_wifi_on = 1;
}

void loop()
{
  if (is_wifi_on)
    web_work();
  ld2451_loop();
}