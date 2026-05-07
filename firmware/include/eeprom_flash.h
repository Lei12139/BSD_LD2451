#ifndef _eeprom_flash_H_
#define _eeprom_flash_H_

#include <EEPROM.h>
#include <Arduino.h>

#define eeprom_size 1024

#define EEPROM_BASE 0
#define first_download_add (EEPROM_BASE + 0)       // 初次烧录固件后判定初始化设置参数
#define set_temp_low_add (EEPROM_BASE + 1)         // 预设温度值低八位
#define set_temp_high_add (EEPROM_BASE + 2)        // 预设温度高八位
#define set_temp_mode_add (EEPROM_BASE + 3)        // 温度模式 0：回流焊 1：恒温
#define miot_able_add (EEPROM_BASE + 4)            // 是否启用接入小爱 0：启用 1：不启用
#define temp_mode1_time_low_add (EEPROM_BASE + 5)  // 恒温时长低八位
#define temp_mode1_time_high_add (EEPROM_BASE + 6) // 恒温时长高八位
#define set_oled_light_add (EEPROM_BASE + 7)       // 屏幕亮度
#define temp0_data_start_add (EEPROM_BASE + 8)     // 回流参数

#define blinker_id_add (EEPROM_BASE + 10) //...点灯注册设备号

#define hotbed_max_temp_low_add (EEPROM_BASE + 30)             // 设备最高温度低八位
#define hotbed_max_temp_high_add (hotbed_max_temp_low_add + 1) // 设备最高温度高八位

#define adc_max_temp_low_add (hotbed_max_temp_low_add + 2)  // 设备读取最高温度低八位
#define adc_max_temp_high_add (hotbed_max_temp_low_add + 3) // 设备读取最高温度高八位

typedef struct
{
    uint8_t init_flag; // 储存参数是否初始化 ffh: 未初始化， 00: 已初始化
    char wifi_name[16];
    char wifi_password[10];
    int8_t scan_angle_left;  // 雷达扫描角度 左侧  单位： 度
    int8_t scan_angle_right; // 左右角度内的车辆触发告警
    uint8_t scan_speed;      // 雷达预警速度，大于该速度触发告警  单位： km/h
    uint8_t scan_distance;   // 雷达预警距离，小于该距离触发告警  单位： m
    int8_t b_ang;            // 正后方预警角度
    uint8_t b_speed;         // 正后方预警速度
    uint8_t b_dist;          // 正后方预警距离。在正后方预警角内，达到预警速度&距离时触发 正后方告警
    uint8_t led_f_mode;      // 前灯工作模式
    uint8_t led_b_mode;      // 尾灯工作模式
} SYS_CONFIG_T;

class eeprom_flash
{
public:
    void data_init(SYS_CONFIG_T *sys_cfg);     // 数据初始化
    void read_all_data(SYS_CONFIG_T *sys_cfg); // 读取所有数据
    void write(int add, uint8_t dat);          // 写一个字节
    uint8_t read(int add);                     // 读一个字节
    void write_task();
    void write_str(int add, String str); // 写字符串

    uint8_t write_flg = 0;
    uint8_t write_t = 21;
};

extern eeprom_flash eeprom;

#define SYS_CONFIG_LEN (sizeof(SYS_CONFIG_T))

#endif
