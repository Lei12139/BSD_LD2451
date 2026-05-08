#include "eeprom_flash.h"
#include "version.h"

eeprom_flash eeprom;

// 保存参数到flash
void eeprom_flash::para_save(SYS_CONFIG_T *sys_cfg)
{
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(0, *sys_cfg);
    EEPROM.commit();
    EEPROM.end();
}
// 参数初始化为默认值
void eeprom_flash::para_init(SYS_CONFIG_T *sys_cfg)
{
    // EEPROM初始化数据
    memset(sys_cfg, 0, SYS_CONFIG_LEN);
    sys_cfg->init_flag = 0xa5;
    memcpy(sys_cfg->wifi_name, "BSD_LD2451", strlen("BSD_LD2451"));
    memcpy(sys_cfg->wifi_password, "12345678", strlen("12345678"));
    sys_cfg->scan_angle_left = 6;
    sys_cfg->scan_angle_right = 7;
    sys_cfg->scan_speed = 20;
    sys_cfg->scan_distance = 22;
    sys_cfg->led_f_mode = 1;
    sys_cfg->led_b_mode = 2;
}

// 读取所有参数
void eeprom_flash::load_all_para(SYS_CONFIG_T *sys_cfg)
{
    char buffer[40];
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(0, *sys_cfg);
    if (sys_cfg->init_flag != 0xA5)
    {
        Serial.println("EE init");
        para_init(sys_cfg);
        EEPROM.put(0, *sys_cfg);
        EEPROM.commit();
        Serial.println("EE done");
    }
    else
    {
        Serial.println("EE read done");
    }
    EEPROM.end();
    Serial.println(sys_cfg->wifi_name);
    Serial.println(sys_cfg->wifi_password);
    sprintf(buffer, "scan_angle_left = %d", sys_cfg->scan_angle_left);
    Serial.println(buffer);
    sprintf(buffer, "scan_angle_right = %d", sys_cfg->scan_angle_right);
    Serial.println(buffer);
    sprintf(buffer, "scan_speed = %d", sys_cfg->scan_speed);
    Serial.println(buffer);
    sprintf(buffer, "scan_distance = %d", sys_cfg->scan_distance);
    Serial.println(buffer);
    sprintf(buffer, "led_f_mode = %d", sys_cfg->led_f_mode);
    Serial.println(buffer);
    sprintf(buffer, "led_b_mode = %d", sys_cfg->led_b_mode);
    Serial.println(buffer);
}
