/**
 ************************************************************
 ************************************************************
 ************************************************************
 *	�ļ����� 	onenet.c
 *
 *	���ߣ� 		�ż���
 *
 *	���ڣ� 		2017-05-08
 *
 *	�汾�� 		V1.1
 *
 *	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
 *
 *	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
 *				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
 ************************************************************
 ************************************************************
 ************************************************************
 **/

// ��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

// �����豸
#include "esp8266.h"

// Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

// Ӳ������
#include "usart.h"
#include "delay.h"

// C��
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "led.h"
#include "beep.h"

#define PROID "chan"

#define AUTH_INFO "huaqing_smarthome_zui_jiandan"

#define DEVID "huaqing_smarthome"

extern unsigned char esp8266_buf[128];
extern u8 alarm_is_free;

// Global threshold variables with default values
extern volatile u8 temperatureThreshold; // Default: 30°C
extern volatile u8 humidityThreshold;    // Default: 80%
extern volatile float smokeThreshold;    // Default: 50 (example value)
extern u8 Led_Status;                    // LED status

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
    char ProductID[64];
    char Auth_Info[64];
    char DeviceID[64];

    u32 MCU_ID;

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

    unsigned char *dataPtr;

    _Bool status = 1;

    MCU_ID = *(vu32 *)(0x1FFFF7E8); // ��ȡMCU��Ψһ��ʶ
    sprintf(ProductID, "%s_%X", PROID, MCU_ID);
    sprintf(Auth_Info, "%s_%X", AUTH_INFO, MCU_ID);
    sprintf(DeviceID, "%s_%X", DEVID, MCU_ID);

    UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                             "PROID: %s,	AUIF: %s,	DEVID:%s	--- ������....\r\n",
                ProductID, Auth_Info, DeviceID);

    if (MQTT_PacketConnect(ProductID, Auth_Info, DeviceID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
    {
        ESP8266_SendData(mqttPacket._data, mqttPacket._len); // �ϴ�ƽ̨

        dataPtr = ESP8266_GetIPD(250); // �ȴ�ƽ̨��Ӧ
        if (dataPtr != NULL)
        {
            if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
            {
                switch (MQTT_UnPacketConnectAck(dataPtr))
                {
                case 0:
                    DEBUG_LOG("Tips:	���ӳɹ�\r\n");
                    status = 0;
                    break;

                case 1:
                    DEBUG_LOG("WARN:	����ʧ�ܣ�Э�����\r\n");
                    break;
                case 2:
                    DEBUG_LOG("WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");
                    break;
                case 3:
                    DEBUG_LOG("WARN:	����ʧ�ܣ�������ʧ��\r\n");
                    break;
                case 4:
                    DEBUG_LOG("WARN:	����ʧ�ܣ��û������������\r\n");
                    break;
                case 5:
                    DEBUG_LOG("WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");
                    break;

                default:
                    DEBUG_LOG("ERR:	����ʧ�ܣ�δ֪����\r\n");
                    break;
                }
            }
        }

        MQTT_DeleteBuffer(&mqttPacket); // ɾ��
    }
    else
        DEBUG_LOG("WARN:	MQTT_PacketConnect Failed\r\n");

    return status;
}

//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{

    unsigned char i = 0;

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

    for (; i < topic_cnt; i++)
        DEBUG_LOG("Subscribe Topic: %s\r\n", topics[i]);

    if (MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
    {
        ESP8266_SendData(mqttPacket._data, mqttPacket._len); // ��ƽ̨���Ͷ�������

        MQTT_DeleteBuffer(&mqttPacket); // ɾ��
    }
}

//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_PUBLISH-��Ҫ����
//
//	˵����
//==========================================================
void OneNet_Publish(const char *topic, const char *msg)
{

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

    //DEBUG_LOG("Publish Topic: %s, Msg: %s\r\n", topic, msg);

    if (MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
    {
        ESP8266_SendData(mqttPacket._data, mqttPacket._len); // ��ƽ̨���Ͷ�������

        MQTT_DeleteBuffer(&mqttPacket); // ɾ��
    }
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص����� Ҳ����*cmd
//
//	���ز�����	��
//
//	˵����
//==========================================================
// void OneNet_RevPro(unsigned char *cmd)
// {
//     MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};

//     char *req_payload = NULL;
//     char *cmdid_topic = NULL;

//     unsigned short topic_len = 0;
//     unsigned short req_len = 0;

//     unsigned char type = 0;
//     unsigned char qos = 0;
//     static unsigned short pkt_id = 0;

//     short result = 0;

//     char *dataPtr = NULL;
//     char numBuf[10];
//     int num = 0;

//     cJSON *json, *cmd_type, *cmd_value;

//     type = MQTT_UnPacketRecv(cmd);
//     switch(type)
//     {
//         case MQTT_PKT_PUBLISH:    // 收到Publish消息
//             result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
//             if(result == 0)
//             {
//                 DEBUG_LOG("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
//                     cmdid_topic, topic_len, req_payload, req_len);

//                 // 解析JSON数据
//                 json = cJSON_Parse(req_payload);
//                 if (!json) {
//                     DEBUG_LOG("Error parsing JSON: [%s]\n", cJSON_GetErrorPtr());
//                 }
//                 else {
//                     // 获取cmd字段
//                     cmd_type = cJSON_GetObjectItem(json, "cmd");
//                     if(cmd_type != NULL && cmd_type->valuestring != NULL)
//                     {
//                         // 获取value字段
//                         cmd_value = cJSON_GetObjectItem(json, "value");
//                         if(cmd_value != NULL && cmd_value->valuestring != NULL)
//                         {
//                             // 处理灯光控制命令
//                             if(strcmp(cmd_type->valuestring, "light") == 0)
//                             {
//                                 if(strcmp(cmd_value->valuestring, "on") == 0)
//                                 {
//                                     LED0 = 0;  // 开灯
//                                     DEBUG_LOG("LED turned ON\r\n");
//                                 }
//                                 else if(strcmp(cmd_value->valuestring, "off") == 0)
//                                 {
//                                     LED0 = 1;  // 关灯
//                                     DEBUG_LOG("LED turned OFF\r\n");
//                                 }
//                             }
//                             // 处理报警器控制命令
//                             else if(strcmp(cmd_type->valuestring, "alarm") == 0)
//                             {
//                                 if(strcmp(cmd_value->valuestring, "on") == 0)
//                                 {
//                                     alarmFlag = 1;      // 开启报警
//                                     alarm_is_free = 0;  // 标记为手动控制
//                                     DEBUG_LOG("Alarm turned ON\r\n");
//                                 }
//                                 else if(strcmp(cmd_value->valuestring, "off") == 0)
//                                 {
//                                     alarmFlag = 0;      // 关闭报警
//                                     alarm_is_free = 0;  // 标记为手动控制
//                                     DEBUG_LOG("Alarm turned OFF\r\n");
//                                 }
//                             }
//                             // 处理阈值设置命令（如果需要的话）
//                             else if(strcmp(cmd_type->valuestring, "threshold") == 0)
//                             {
//                                 // 在这里添加阈值设置逻辑
//                                 DEBUG_LOG("Threshold command received\r\n");
//                             }
//                         }
//                     }
//                     cJSON_Delete(json);  // 释放JSON对象
//                 }
//             }
//             break;

//         case MQTT_PKT_PUBACK:    // 发送Publish消息，平台回复的Ack
//             if(MQTT_UnPacketPublishAck(cmd) == 0)
//                 DEBUG_LOG("Tips: MQTT Publish Send OK\r\n");
//             break;

//         case MQTT_PKT_SUBACK:    // 发送Subscribe消息的Ack
//             if(MQTT_UnPacketSubscribe(cmd) == 0)
//                 DEBUG_LOG("Tips: MQTT Subscribe OK\r\n");
//             else
//                 DEBUG_LOG("Tips: MQTT Subscribe Err\r\n");
//             break;

//         case MQTT_PKT_UNSUBACK:    // 发送UnSubscribe消息的Ack
//             if(MQTT_UnPacketUnSubscribe(cmd) == 0)
//                 DEBUG_LOG("Tips: MQTT UnSubscribe OK\r\n");
//             else
//                 DEBUG_LOG("Tips: MQTT UnSubscribe Err\r\n");
//             break;

//         default:
//             result = -1;
//             break;
//     }

//     ESP8266_Clear();    // 清空缓存

//     // 释放资源
//     if(type == MQTT_PKT_PUBLISH)
//     {
//         MQTT_FreeBuffer(cmdid_topic);
//         MQTT_FreeBuffer(req_payload);
//     }
// }

void OneNet_RevPro(unsigned char *cmd)
{
    // MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};

    char *req_payload = NULL;
    char *cmdid_topic = NULL;

    unsigned short topic_len = 0;
    unsigned short req_len = 0;

    unsigned char type = 0;
    unsigned char qos = 0;
    static unsigned short pkt_id = 0;

    short result = 0;

    cJSON *json;
    cJSON *tmp;

    type = MQTT_UnPacketRecv(cmd);
    switch (type)
    {
    case MQTT_PKT_PUBLISH: // 收到Publish消息
        result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
        if (result == 0)
        {
            DEBUG_LOG("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
                      cmdid_topic, topic_len, req_payload, req_len);

            // ===== 1. 解析JSON并进行key-value设置 =====
            json = cJSON_Parse(req_payload);
            if (!json)
            {
                DEBUG_LOG("Error parsing JSON: [%s]\n", cJSON_GetErrorPtr());
            }
            else
            {
                // temperature_threshold
                tmp = cJSON_GetObjectItem(json, "temperature_threshold");
                if (tmp && tmp->type == cJSON_Number)
                {
                    temperatureThreshold = (u8)(tmp->valuedouble);
                    DEBUG_LOG("Set temperatureThreshold = %.2f\r\n", tmp->valuedouble);
                }
                // humidity_threshold
                tmp = cJSON_GetObjectItem(json, "humidity_threshold");
                if (tmp && tmp->type == cJSON_Number)
                {
                    humidityThreshold = (u8)(tmp->valuedouble);
                    DEBUG_LOG("Set humidityThreshold = %.2f\r\n", tmp->valuedouble);
                }
                // smoke_threshold
                tmp = cJSON_GetObjectItem(json, "smoke_threshold");
                if (tmp && tmp->type == cJSON_Number)
                {
                    smokeThreshold = (float)(tmp->valuedouble);
                    DEBUG_LOG("Set smokeThreshold = %.2f\r\n", tmp->valuedouble);
                }
                // light_switch
                tmp = cJSON_GetObjectItem(json, "light_switch");
                if (tmp && tmp->type == cJSON_String)
                {
                    if (strcmp(tmp->valuestring, "on") == 0)
                    {
                        //LED0 = 1;
                        TIM_SetCompare4(TIM1, 999);
                        DEBUG_LOG("Light switched ON\r\n");
                    }
                    else if (strcmp(tmp->valuestring, "off") == 0)
                    {
                        //LED0 = 0;
                        TIM_SetCompare4(TIM1, 0);
                        DEBUG_LOG("Light switched OFF\r\n");
                    }
                }

                // remote_warning
                tmp = cJSON_GetObjectItem(json, "remote_warning");
                if (tmp && tmp->type == cJSON_String)
                {
                    if (strcmp(tmp->valuestring, "activate") == 0)
                    {
                        BEEP_remote = 1;
                        DEBUG_LOG("Remote Warning ACTIVATED\r\n");
                    }
                    else if (strcmp(tmp->valuestring, "deactivate") == 0)
                    {
                        BEEP_remote = 0;
                        DEBUG_LOG("Remote Warning DEACTIVATED\r\n");
                    }
                }
                
                cJSON_Delete(json);
            }
        }
        break;

    case MQTT_PKT_PUBACK: // 发送Publish消息，平台回复的Ack
        if (MQTT_UnPacketPublishAck(cmd) == 0)
            DEBUG_LOG("Tips: MQTT Publish Send OK\r\n");
        break;

    case MQTT_PKT_SUBACK: // 发送Subscribe消息的Ack
        if (MQTT_UnPacketSubscribe(cmd) == 0)
            DEBUG_LOG("Tips: MQTT Subscribe OK\r\n");
        else
            DEBUG_LOG("Tips: MQTT Subscribe Err\r\n");
        break;

    case MQTT_PKT_UNSUBACK: // 发送UnSubscribe消息的Ack
        if (MQTT_UnPacketUnSubscribe(cmd) == 0)
            DEBUG_LOG("Tips: MQTT UnSubscribe OK\r\n");
        else
            DEBUG_LOG("Tips: MQTT UnSubscribe Err\r\n");
        break;

    default:
        result = -1;
        break;
    }

    ESP8266_Clear(); // 清空缓存

    // 释放资源
    if (type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }
}
