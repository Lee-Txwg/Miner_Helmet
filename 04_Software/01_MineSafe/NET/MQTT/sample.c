/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	sample.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-06-14
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		���ļ���������룬����ֻ��SDKʹ�õ���ʾ
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/


#include "mqttkit.h"


unsigned char dataMem[128];		//ȫ�����鷽ʽ


//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	devid�������豸��devid
//				proid����ƷID
//				auth_key�������豸��masterKey��apiKey���豸��Ȩ��Ϣ
//
//	���ز�����	��
//
//	˵����		��onenetƽ̨�������ӣ��ɹ������oneNetInfo.netWork����״̬��־
//==========================================================
void OneNet_DevLink(const char* devid, const char *proid, const char* auth_info)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	
	edpPacket._data = dataMem;				//ȫ�����鷽ʽ
	edpPacket._size = sizeof(dataMem);		//ȫ�����鷽ʽ
	
//---------------------------------------------����һ�����---------------------------------------------
	if(MQTT_PacketConnect(proid, auth_info, devid, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);

//---------------------------------------------���������жϷ�������--------------------------------------
		if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
		{
//---------------------------------------------�����ģ��������ؽ��--------------------------------------
			switch(MQTT_UnPacketConnectAck(dataPtr))
			{
				case 0:
					UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");
					oneNetInfo.netWork = 1;
				break;
				
				case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
				case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
				case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
				case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û������������\r\n");break;
				case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
				
				default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
			}
		}
		
//---------------------------------------------�����壺ɾ��---------------------------------------------
		MQTT_DeleteBuffer(&mqttPacket);
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
}

//==========================================================
//	�������ƣ�	OneNet_DisConnect
//
//	�������ܣ�	��ƽ̨�Ͽ�����
//
//	��ڲ�����	��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
_Bool OneNet_DisConnect(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	unsigned char dataMem1[8];				//�ֲ����鷽ʽ
	
	edpPacket._data = dataMem;				//�ֲ����鷽ʽ
	edpPacket._size = sizeof(dataMem);		//�ֲ����鷽ʽ
	
//---------------------------------------------����һ�����---------------------------------------------
	if(MQTT_PacketDisConnect(&mqttPacket) == 0)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
		
//---------------------------------------------��������ɾ��---------------------------------------------
		MQTT_DeleteBuffer(&mqttPacket);
	}
	
	return 0;

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//				devid���豸ID
//				apikey���豸apikey
//				streamArray��������
//				streamArrayNum������������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
_Bool OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	
	_Bool status = 0;
	short body_len = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT_TYPE%d\r\n", type);
	
//---------------------------------------------����һ����������������---------------------------------------------
	body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);
	if(body_len)
	{
//---------------------------------------------���������дЭ��ͷ-------------------------------------------------
		if(MQTT_PacketSaveData(devid, body_len, NULL, (uint8)type, &mqttPacket) == 0)
		{
//---------------------------------------------�����������-------------------------------------------------------
			body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, mqttPacket._data, mqttPacket._size, mqttPacket._len);
			
			if(body_len)
			{
				mqttPacket._len += body_len;
				UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
//---------------------------------------------�����ģ���������---------------------------------------------------
				NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
			}
			else
				UsartPrintf(USART_DEBUG, "WARN:	DSTREAM_GetDataStream_Body Failed\r\n");
			
//---------------------------------------------�����壺ɾ��-------------------------------------------------------
			MQTT_DeleteBuffer(&mqttPacket);
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	MQTT_NewBuffer Failed\r\n");
	}
	else
		status = 1;
	
	return status;
	
}

//==========================================================
//	�������ƣ�	OneNet_HeartBeat
//
//	�������ܣ�	�������
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_HeartBeat(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};

	unsigned char sCount = 3;
	
//---------------------------------------------����һ�����---------------------------------------------
	if(MQTT_PacketPing(&mqttPacket))
		return;
	
	while(sCount--)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);

//---------------------------------------------��������������������-------------------------------------
		if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_PINGRESP)
		{
			UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
			
			break;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Check Device\r\n");
		}
		
		RTOS_TimeDly(2);
	}
	
//---------------------------------------------�����ģ�ɾ��---------------------------------------------
	MQTT_DeleteBuffer(&mqttPacket);

}

//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	
	UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
//---------------------------------------------����һ�����---------------------------------------------
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL2, 0, 1, &mqttPacket) == 0)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
		
//---------------------------------------------��������ɾ��---------------------------------------------
		MQTT_DeleteBuffer(&mqttPacket);
	}
	
	return 0;

}

//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	for(; i < topic_cnt; i++)
		UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topics[i]);
	
//---------------------------------------------����һ�����---------------------------------------------
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
//---------------------------------------------��������ɾ��---------------------------------------------
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}
	
	return 0;

}

//==========================================================
//	�������ƣ�	OneNet_UnSubscribe
//
//	�������ܣ�	ȡ������
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool OneNet_UnSubscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	for(; i < topic_cnt; i++)
		UsartPrintf(USART_DEBUG, "UnSubscribe Topic: %s\r\n", topics[i]);
	
//---------------------------------------------����һ�����---------------------------------------------
	if(MQTT_PacketUnSubscribe(MQTT_UNSUBSCRIBE_ID, topics, topic_cnt, &mqttPacket) == 0)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨����ȡ����������
		
//---------------------------------------------��������ɾ��---------------------------------------------
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}
	
	return 0;

}

//==========================================================
//  Function Name:    OneNet_RevPro
//
//  Function:         Process data received from the platform
//
//  Input Parameter:  dataPtr: data returned from the platform
//
//  Return Value:     None
//
//  Note:             
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = -1;
	
//--------------------------------------------- Step 1: Parse the received message type ---------------------------------------------
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
//--------------------------------------------- Process different message types -------------------------------------------------
		case MQTT_PKT_CMD:															// Command message
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload);				// Parse topic and message info
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s\r\n", cmdid_topic, req_payload);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	// Send response packet
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
					
					NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);			// Send response
					MQTT_DeleteBuffer(&mqttPacket);									// Free buffer
				}
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														// Received a Publish packet
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &req_payload, &qos, &pkt_id);
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "topic: %s\r\npayload: %s\r\n", cmdid_topic, req_payload);
				
				switch(qos)
				{
					case 1:															// If qos is 1, device should reply with Ack
					
						if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
						{
							UsartPrintf(USART_DEBUG, "Tips:	Send PublishAck\r\n");
							NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					case 2:															// If qos is 2, device first replies Rec, server replies Rel, device replies Comp
																					// Platform replies Rel, device replies Comp
						if(MQTT_PacketPublishRec(pkt_id, &mqttPacket) == 0)
						{
							UsartPrintf(USART_DEBUG, "Tips:	Send PublishRec\r\n");
							NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					default:
						break;
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														// After sending Publish, platform replies with Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														// After sending Publish, platform replies Rec and device replies Rel
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send PublishRel\r\n");
					NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														// After receiving Publish, device replies Rec, platform replies Rel, device replies Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send PublishComp\r\n");
					NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														// After sending Publish, platform replies Rec, device replies Rel, platform finally replies Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														// After sending Subscribe, receive Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");
			else
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														// After sending UnSubscribe, receive Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe OK\r\n");
			else
				UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
	}
	
//--------------------------------------------- Handle special commands ---------------------------------------------
	if(req)
	{
		
	}
//--------------------------------------------- Free dynamic memory ---------------------------------------------
	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
