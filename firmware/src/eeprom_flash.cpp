#include "eeprom_flash.h"
#include "version.h"

eeprom_flash eeprom;

void eeprom_flash::data_init(SYS_CONFIG_T *sys_cfg)
{
    // EEPROM初始化数据
    write_flg = 2;

    sys_cfg->init_flag = 0;
    memcpy(sys_cfg->wifi_name, "BSD_LD2451", strlen("BSD_LD2451"));
    memcpy(sys_cfg->wifi_password, "12345678", strlen("12345678"));
    sys_cfg->scan_angle_left = 6;
    sys_cfg->scan_angle_right = 7;
    sys_cfg->scan_speed = 20;
    sys_cfg->scan_distance = 22;
    sys_cfg->led_f_mode = 1;
    sys_cfg->led_b_mode = 2;
    for (int x = first_download_add; x < eeprom_size; x++)
        EEPROM.write(x, *((uint8_t *)sys_cfg+x));
}
void eeprom_flash::read_all_data(SYS_CONFIG_T *sys_cfg)
{
    char buffer[40];
    EEPROM.begin(eeprom_size);
    if (EEPROM.read(first_download_add))
    {
        Serial.println("EE init");
        data_init(sys_cfg);
        EEPROM.commit();
        Serial.println("EE done");
    }
    else
    {
        for (int x = 0; x < SYS_CONFIG_LEN; x++)
            *((uint8_t *)sys_cfg+x) = EEPROM.read(x);
        Serial.println("EE read done");
    }
    EEPROM.end();
    Serial.println(sys_cfg->wifi_name);
    Serial.println(sys_cfg->wifi_password);
    sprintf(buffer, "scan_angle_left = -%d"  , sys_cfg->scan_angle_left);
    Serial.println(buffer);
    sprintf(buffer, "scan_angle_right = %d"  , sys_cfg->scan_angle_right);
    Serial.println(buffer);
    sprintf(buffer, "scan_speed = %d"        , sys_cfg->scan_speed);
    Serial.println(buffer);
    sprintf(buffer, "scan_distance = %d"     , sys_cfg->scan_distance);
    Serial.println(buffer);
    sprintf(buffer, "led_f_mode = %d"        , sys_cfg->led_f_mode);
    Serial.println(buffer);
    sprintf(buffer, "led_b_mode = %d"        , sys_cfg->led_b_mode);
    Serial.println(buffer);
}
uint8_t eeprom_flash::read(int add)
{
    uint8_t x;
    EEPROM.begin(eeprom_size);
    x = EEPROM.read(add);
    EEPROM.end();
    return x;
}
void eeprom_flash::write(int add, uint8_t dat)
{

    EEPROM.begin(eeprom_size);
    EEPROM.write(add, dat);
    EEPROM.end();
}

void eeprom_flash::write_task()
{
    if (write_flg == 2)
    {
        write_flg = 0;
    }
}

void eeprom_flash::write_str(int add, String str)
{
    const char *p = &str[0];
    EEPROM.begin(eeprom_size);
    while (*p)
    {
        EEPROM.write(add++, *p++);
        yield();
    }
    EEPROM.write(add, '\0');
    EEPROM.end();
}
