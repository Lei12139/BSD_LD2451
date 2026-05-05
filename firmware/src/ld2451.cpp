#include <SoftwareSerial.h>
// 雷达串口配置 (RX=4, TX=5)
SoftwareSerial radarSerial(4, 5); // RX=D2(4), TX=D1(5)
// 雷达协议常量
const uint8_t DATA_HEADER[4] = {0xF4, 0xF3, 0xF2, 0xF1};
const uint8_t DATA_FOOTER[4] = {0xF8, 0xF7, 0xF6, 0xF5};
// 引脚定义
const int LEFT_PIN = 14;  // 左侧来车控制引脚（低电平亮）
const int RIGHT_PIN = 12; // 右侧来车控制引脚（低电平亮）
// 状态机变量
enum ParserState
{
    WAITING_HEADER, // 帧头
    READING_LENGTH, // 帧尾
    READING_DATA,   // 帧数据长
    READING_FOOTER  // 帧数据
};
ParserState state = WAITING_HEADER;
uint8_t headerIndex = 0;
uint16_t dataLength = 0;
uint16_t dataIndex = 0;
uint8_t packetBuffer[128]; // 数据缓冲区
// 车辆检测状态标志
bool hasVehicleDetected = false;
bool leftVehicleDetected = false;   // 左侧来车标志
bool rightVehicleDetected = false;  // 右侧来车标志
bool behindVehicleDetected = false; // 正后方来车标志
// 上次打印时间戳（用于控制输出频率）
unsigned long lastPrintTime = 0;
// 打印间隔（毫秒）
const unsigned long PRINT_INTERVAL = 1000; // 每2秒打印一次

void processRadarData(uint8_t byte);
void parseTargetData();
void sendConfigCommand();

void ld2451_setup(void)
{
    // 初始化串口
    // Serial.begin(115200);      // 调试串口
    radarSerial.begin(115200); // 雷达默认波特率

    // 初始化控制引脚
    pinMode(LEFT_PIN, OUTPUT);
    pinMode(RIGHT_PIN, OUTPUT);

    delay(1000);
    Serial.println("LD2451 Vehicle Detection System Started");

    // 初始状态设为高电平（灭）
    digitalWrite(LEFT_PIN, HIGH);
    digitalWrite(RIGHT_PIN, HIGH);

    // 发送雷达配置命令
    // sendConfigCommand();
}

void ld2451_loop(void)
{
    // 处理雷达数据
    while (radarSerial.available())
    {
        processRadarData(radarSerial.read());
    }

    // 检查是否未检测到车辆并循环打印
    unsigned long currentTime = millis();
    if (!hasVehicleDetected && currentTime - lastPrintTime >= PRINT_INTERVAL)
    {
        Serial.println("no car");
        // 未检测到车辆时保持高电平（灭）
        digitalWrite(LEFT_PIN, HIGH);
        digitalWrite(RIGHT_PIN, HIGH);
        lastPrintTime = currentTime;
    }
    else
    {
        // 根据左右侧检测状态控制引脚（低电平亮）
        if (leftVehicleDetected && rightVehicleDetected)
        {
            // 两侧都有来车时一起亮
            digitalWrite(LEFT_PIN, LOW);
            digitalWrite(RIGHT_PIN, LOW);
        }
        else if (leftVehicleDetected)
        {
            // 仅左侧来车
            digitalWrite(LEFT_PIN, LOW);
            digitalWrite(RIGHT_PIN, HIGH);
        }
        else if (rightVehicleDetected)
        {
            // 仅右侧来车
            digitalWrite(RIGHT_PIN, LOW);
            digitalWrite(LEFT_PIN, HIGH);
        }
    }

    // 每次循环结束后重置检测标志
    hasVehicleDetected = false;
    leftVehicleDetected = false;
    rightVehicleDetected = false;
}

// 雷达配置命令发送
void sendConfigCommand()
{
    // 使能配置命令
    uint8_t enableCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00,
                           0xFF, 0x00, 0x01, 0x00,
                           0x04, 0x03, 0x02, 0x01};
    radarSerial.write(enableCmd, sizeof(enableCmd));
    delay(50);

    // 目标检测参数配置 (100m距离, 只检测靠近, 1km/h最小速度, 2s延迟)
    uint8_t configCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x06, 0x00,
                           0x02, 0x00, 0x64, 0x01, 0x01, 0x02,
                           0x04, 0x03, 0x02, 0x01};
    radarSerial.write(configCmd, sizeof(configCmd));
    delay(50);

    // 结束配置命令
    uint8_t endCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00,
                        0xFE, 0x00, 0x04, 0x03, 0x02, 0x01};
    radarSerial.write(endCmd, sizeof(endCmd));

    Serial.println("Radar Configuration Applied");
}

// 雷达数据处理状态机
void processRadarData(uint8_t byte)
{
    switch (state)
    {
    case WAITING_HEADER:
        if (byte == DATA_HEADER[headerIndex])
        {
            headerIndex++;
            if (headerIndex == 4)
            {
                state = READING_LENGTH;
                dataIndex = 0;
            }
        }
        else
        {
            headerIndex = 0;
        }
        break;
    case READING_LENGTH:
        packetBuffer[dataIndex++] = byte;
        if (dataIndex == 2)
        {
            dataLength = (packetBuffer[1] << 8) | packetBuffer[0]; // 小端格式
            dataIndex = 0;

            if (dataLength > 0 && dataLength < sizeof(packetBuffer))
            {
                state = READING_DATA;
            }
            else
            {
                state = WAITING_HEADER;
            }
        }
        break;
    case READING_DATA:
        packetBuffer[dataIndex++] = byte;
        if (dataIndex >= dataLength)
        {
            state = READING_FOOTER;
            headerIndex = 0;
        }
        break;
    case READING_FOOTER:
        if (byte == DATA_FOOTER[headerIndex])
        {
            headerIndex++;
            if (headerIndex == 4)
            {
                parseTargetData();
                state = WAITING_HEADER;
                headerIndex = 0;
                Serial.println("parseTargetData done \n");
            }
        }
        else
        {
            state = WAITING_HEADER;
            headerIndex = 0;
        }
        break;
    }
}

// 解析目标数据
void parseTargetData()
{
    if (dataLength < 2)
        return;

    uint8_t targetCount = packetBuffer[0];
    uint8_t alarmInfo = packetBuffer[1];

    // 重置左右侧检测状态
    leftVehicleDetected = false;
    rightVehicleDetected = false;
    behindVehicleDetected = false;
    // 检查有效目标
    if (targetCount == 0 || dataLength < 2 + targetCount * 5)
    {
        return;
    }

    // 处理每个目标
    for (int i = 0; i < targetCount; i++)
    {
        int offset = 2 + i * 5;

        // 解析目标参数
        int8_t angle = packetBuffer[offset] - 0x80;  // 角度转换
        uint8_t distance = packetBuffer[offset + 1]; // 距离(米)
        uint8_t dir = packetBuffer[offset + 2];      // 方向 (0x01=靠近)
        uint8_t speed = packetBuffer[offset + 3];    // 速度(km/h)
        uint8_t snr = packetBuffer[offset + 4];      // 信噪比

        // 过滤条件：正后方快速来车
        if ((abs(angle) <= 5) && (speed > 10) && (distance < 15))
        {
            behindVehicleDetected = true;
            return;
        }
        else if (dir == 0x01 && speed >= 1 && abs(angle) > 5)
        {
            // 修复方向判断逻辑（angle>0对应右侧，angle<=0对应左侧）
            String direction = (angle > 0) ? "ringt" : "left";

            // 串口输出结果
            Serial.print(direction + "car!! ");
            Serial.print("speed:" + String(speed) + "km/h, ");
            Serial.print("distance:" + String(distance) + "m, ");
            Serial.print("angle:" + String(angle));
            Serial.println();

            // 检测到车辆时更新标志
            hasVehicleDetected = true;

            // 更新左右侧检测状态
            if (direction == "left")
            {
                leftVehicleDetected = true;
            }
            else
            {
                rightVehicleDetected = true;
            }
        }
    }
}