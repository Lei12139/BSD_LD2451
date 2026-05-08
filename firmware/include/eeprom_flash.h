#ifndef _eeprom_flash_H_
#define _eeprom_flash_H_

#include <ESP_EEPROM.h>
#include <Arduino.h>

#define EEPROM_BASE 0
#define first_download_add (EEPROM_BASE + 0) // 初次烧录固件后判定初始化设置参数

typedef struct
{
    uint8_t init_flag; // 储存参数是否初始化 ffh: 未初始化， 0xA5: 已初始化
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
    void para_save(SYS_CONFIG_T *sys_cfg);     // 保存参数到flash
    void para_init(SYS_CONFIG_T *sys_cfg);     // 数据初始化
    void load_all_para(SYS_CONFIG_T *sys_cfg); // 读取所有数据
};

extern eeprom_flash eeprom;

#define SYS_CONFIG_LEN (sizeof(SYS_CONFIG_T))
#define EEPROM_SIZE (1024)
#endif
