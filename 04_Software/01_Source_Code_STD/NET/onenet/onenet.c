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

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"

//C��
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "led.h"

#define PROID		"huaqing_smarthome"

#define AUTH_INFO	"huaqing_smarthome_zui_jiandan"

#define DEVID		"huaqing_smarthome"


extern unsigned char esp8266_buf[128];
extern u8 alarmFlag;//�Ƿ񱨾��ı�־
extern u8 alarm_is_free;




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
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	MCU_ID = *(vu32*)(0x1FFFF7E8);	//��ȡMCU��Ψһ��ʶ
	sprintf(ProductID,"%s_%X",PROID,MCU_ID);
	sprintf(Auth_Info,"%s_%X",AUTH_INFO,MCU_ID);
	sprintf(DeviceID,"%s_%X",DEVID,MCU_ID);
	
	
	UsartPrintf(USART_DEBUG,"OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s	--- ������....\r\n"
                        ,ProductID, Auth_Info, DeviceID);
	
	if(MQTT_PacketConnect(ProductID, Auth_Info, DeviceID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:DEBUG_LOG("Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:DEBUG_LOG("WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:DEBUG_LOG("WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:DEBUG_LOG("WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:DEBUG_LOG("WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:DEBUG_LOG("WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:DEBUG_LOG("ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
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
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	for(; i < topic_cnt; i++)
		DEBUG_LOG("Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
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

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	DEBUG_LOG("Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
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
	
// 	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
// 	char *req_payload = NULL;
// 	char *cmdid_topic = NULL;
	
// 	unsigned short topic_len = 0;
// 	unsigned short req_len = 0;
	
// 	unsigned char type = 0;
// 	unsigned char qos = 0;
// 	static unsigned short pkt_id = 0;
	
// 	short result = 0;

// 	char *dataPtr = NULL;
// 	char numBuf[10];
// 	int num = 0;
	
// 	cJSON *json , *json_value;
	
// 	type = MQTT_UnPacketRecv(cmd);
// 	switch(type)
// 	{
// 		case MQTT_PKT_CMD:															//�����·�
			
// 			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
// 			if(result == 0)
// 			{
// 				DEBUG_LOG("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

// 				MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
// 			}
// 		break;
			
// 		case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
		
// 			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
// 			if(result == 0)
// 			{
// 				DEBUG_LOG("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
// 																	cmdid_topic, topic_len, req_payload, req_len);
// 				// �����ݰ�req_payload����JSON��ʽ����
// 				json = cJSON_Parse(req_payload);
// 				if (!json)UsartPrintf(USART_DEBUG,"Error before: [%s]\n",cJSON_GetErrorPtr());
// 				else
// 				{
// 					json_value = cJSON_GetObjectItem(json , "target");
// 					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value->string);
// 					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value->valuestring);
// 					if(strstr(json_value->valuestring,"LED") != NULL)
// 					{
// 						json_value = cJSON_GetObjectItem(json , "value");
// 						if(json_value->valueint)LED0 = 0;//����
// 						else LED0 = 1;//�ص�
// 					}
// 					else
// 					{
// 						json_value = cJSON_GetObjectItem(json , "value");
// 						if(json_value->valueint)alarmFlag = 1;//�򿪱�����
// 						else alarmFlag = 0;//�رձ�����
// 						alarm_is_free=0;//��λ�����Ƹ��ֶ����ƾ���һ�������ȼ� �������Զ����ƣ����Ҫ��alarm_is_free ��0
// 					}
// //					if(json_value->valueint)//json_value > 0��Ϊ����
// //					{
// //						LED0 = 0;//��LED0 
// //					}
// //					else
// //					{
// //						LED0 = 1;//�ر�LED0 
// //					}
// 				}
// 				cJSON_Delete(json);
// 			}
// 		break;
			
// 		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
// 			if(MQTT_UnPacketPublishAck(cmd) == 0)
// 				DEBUG_LOG("Tips:	MQTT Publish Send OK\r\n");
			
// 		break;
			
// 		case MQTT_PKT_PUBREC:														//����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ
		
// 			if(MQTT_UnPacketPublishRec(cmd) == 0)
// 			{
// 				DEBUG_LOG("Tips:	Rev PublishRec\r\n");
// 				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
// 				{
// 					DEBUG_LOG("Tips:	Send PublishRel\r\n");
// 					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
// 					MQTT_DeleteBuffer(&mqttPacket);
// 				}
// 			}
		
// 		break;
			
// 		case MQTT_PKT_PUBREL:														//�յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp
			
// 			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
// 			{
// 				DEBUG_LOG("Tips:	Rev PublishRel\r\n");
// 				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
// 				{
// 					DEBUG_LOG("Tips:	Send PublishComp\r\n");
// 					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
// 					MQTT_DeleteBuffer(&mqttPacket);
// 				}
// 			}
		
// 		break;
		
// 		case MQTT_PKT_PUBCOMP:														//����Publish��Ϣ��ƽ̨����Rec���豸�ظ�Rel��ƽ̨�ٷ��ص�Comp
		
// 			if(MQTT_UnPacketPublishComp(cmd) == 0)
// 			{
// 				DEBUG_LOG("Tips:	Rev PublishComp\r\n");
// 			}
		
// 		break;
			
// 		case MQTT_PKT_SUBACK:														//����Subscribe��Ϣ��Ack
		
// 			if(MQTT_UnPacketSubscribe(cmd) == 0)
// 				DEBUG_LOG("Tips:	MQTT Subscribe OK\r\n");
// 			else
// 				DEBUG_LOG("Tips:	MQTT Subscribe Err\r\n");
		
// 		break;
			
// 		case MQTT_PKT_UNSUBACK:														//����UnSubscribe��Ϣ��Ack
		
// 			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
// 				DEBUG_LOG("Tips:	MQTT UnSubscribe OK\r\n");
// 			else
// 				DEBUG_LOG("Tips:	MQTT UnSubscribe Err\r\n");
		
// 		break;
		
// 		default:
// 			result = -1;
// 		break;
// 	}
	
// 	ESP8266_Clear();									//��ջ���
	
// 	if(result == -1)
// 		return;
	
// 	dataPtr = strchr(req_payload, '}');					//����'}'

// 	if(dataPtr != NULL && result != -1)					//����ҵ���
// 	{
// 		dataPtr++;
		
// 		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
// 		{
// 			numBuf[num++] = *dataPtr++;
// 		}
		
// 		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		
// 	}

// 	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
// 	{
// 		MQTT_FreeBuffer(cmdid_topic);
// 		MQTT_FreeBuffer(req_payload);
// 	}

// }



void OneNet_RevPro(unsigned char *cmd)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
    
    char *req_payload = NULL;
    char *cmdid_topic = NULL;
    
    unsigned short topic_len = 0;
    unsigned short req_len = 0;
    
    unsigned char type = 0;
    unsigned char qos = 0;
    static unsigned short pkt_id = 0;
    
    short result = 0;
    
    char *dataPtr = NULL;
    char numBuf[10];
    int num = 0;
    
    cJSON *json, *cmd_type, *cmd_value;
    
    type = MQTT_UnPacketRecv(cmd);
    switch(type)
    {
        case MQTT_PKT_PUBLISH:    // 收到Publish消息
            result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
            if(result == 0)
            {
                DEBUG_LOG("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
                    cmdid_topic, topic_len, req_payload, req_len);
                
                // 解析JSON数据
                json = cJSON_Parse(req_payload);
                if (!json) {
                    DEBUG_LOG("Error parsing JSON: [%s]\n", cJSON_GetErrorPtr());
                }
                else {
                    // 获取cmd字段
                    cmd_type = cJSON_GetObjectItem(json, "cmd");
                    if(cmd_type != NULL && cmd_type->valuestring != NULL)
                    {
                        // 获取value字段
                        cmd_value = cJSON_GetObjectItem(json, "value");
                        if(cmd_value != NULL && cmd_value->valuestring != NULL)
                        {
                            // 处理灯光控制命令
                            if(strcmp(cmd_type->valuestring, "light") == 0)
                            {
                                if(strcmp(cmd_value->valuestring, "on") == 0)
                                {
                                    LED0 = 0;  // 开灯
                                    DEBUG_LOG("LED turned ON\r\n");
                                }
                                else if(strcmp(cmd_value->valuestring, "off") == 0)
                                {
                                    LED0 = 1;  // 关灯
                                    DEBUG_LOG("LED turned OFF\r\n");
                                }
                            }
                            // 处理报警器控制命令
                            else if(strcmp(cmd_type->valuestring, "alarm") == 0)
                            {
                                if(strcmp(cmd_value->valuestring, "on") == 0)
                                {
                                    alarmFlag = 1;      // 开启报警
                                    alarm_is_free = 0;  // 标记为手动控制
                                    DEBUG_LOG("Alarm turned ON\r\n");
                                }
                                else if(strcmp(cmd_value->valuestring, "off") == 0)
                                {
                                    alarmFlag = 0;      // 关闭报警
                                    alarm_is_free = 0;  // 标记为手动控制
                                    DEBUG_LOG("Alarm turned OFF\r\n");
                                }
                            }
                            // 处理阈值设置命令（如果需要的话）
                            else if(strcmp(cmd_type->valuestring, "threshold") == 0)
                            {
                                // 在这里添加阈值设置逻辑
                                DEBUG_LOG("Threshold command received\r\n");
                            }
                        }
                    }
                    cJSON_Delete(json);  // 释放JSON对象
                }
            }
            break;
            
        case MQTT_PKT_PUBACK:    // 发送Publish消息，平台回复的Ack
            if(MQTT_UnPacketPublishAck(cmd) == 0)
                DEBUG_LOG("Tips: MQTT Publish Send OK\r\n");
            break;
            
        case MQTT_PKT_SUBACK:    // 发送Subscribe消息的Ack
            if(MQTT_UnPacketSubscribe(cmd) == 0)
                DEBUG_LOG("Tips: MQTT Subscribe OK\r\n");
            else
                DEBUG_LOG("Tips: MQTT Subscribe Err\r\n");
            break;
            
        case MQTT_PKT_UNSUBACK:    // 发送UnSubscribe消息的Ack
            if(MQTT_UnPacketUnSubscribe(cmd) == 0)
                DEBUG_LOG("Tips: MQTT UnSubscribe OK\r\n");
            else
                DEBUG_LOG("Tips: MQTT UnSubscribe Err\r\n");
            break;
            
        default:
            result = -1;
            break;
    }
    
    ESP8266_Clear();    // 清空缓存
    
    // 释放资源
    if(type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }
}
