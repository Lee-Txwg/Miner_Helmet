#include "main.h"

u8 alarmFlag = 0;      // Alarm flag status
u8 alarm_is_free = 10; // Check if the alarm system is free, 0 means no free time

u8    humidityH;
u8    humidityL;
u8    temperatureH;
u8    temperatureL;
float smoke_ppm;
// Global threshold variables with default values
volatile u8    temperatureThreshold = 30; // Default: 30°C
volatile u8    humidityThreshold = 80;    // Default: 80%
volatile float smokeThreshold = 1000;       // Default: 50 (example value)

// The light threshold remains unchanged
const float lightThreshold = 1000.0;

extern char oledBuf[20]; // OLED display buffer
float Light = 0;         // Light intensity
u8 Led_Status = 0;       // LED status

char PUB_BUF[256];                          // Publish data buffer
const char *devSubTopic[] = {"helmet/cmd"}; // Subscribe topic for commands
const char devPubTopic[] = "helmet/status"; // Publish topic for status
u8 ESP8266_INIT_OK = 0;                     // ESP8266 initialization status

// Retarget printf
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0)
        ;                     // Wait until transmission is complete
    USART1->DR = (uint8_t)ch; // Send char
    return ch;
}

void USART_BaudRate_Init(uint32_t Data)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = Data;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    /* Enable USARTy Receive  interrupts */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); // Ê¹ÄÜ¿ÕÏÐÖÐ¶Ï
    /* Enable the USART2 */
    USART_Cmd(USART3, ENABLE);
    delay_ms(10); // µÈ´ý10ms
}

#ifdef Version_Rollback

// Function declarations
void HW_Init(void);
void Net_Init(void);
void Sensor_Process(void);
void Comm_Handler(unsigned short *tick);

int main(void)
{
    /*----------------- 1. Variable Definitions -----------------*/
    unsigned short tick = 0;
    unsigned char *rxData = NULL;

    /*----------------- 2. System Initialization -----------------*/
    /*--> [2.1] Hardware Init */
    HW_Init();

    /*--> [2.2] Network Init */
    Net_Init();

    /*--> [2.3] Timer Configuration */
    TIM2_Int_Init(4999, 7199);
    TIM3_Int_Init(2499, 7199);

    /*--> [2.4] System Ready Indication */
    BEEP = 0;
    delay_ms(250);
    BEEP = 1;

    /*--> [2.5] Cloud Connection */
    OneNet_Subscribe(devSubTopic, 1);

    /*----------------- 3. Main Loop -----------------*/
    while (1)
    {
        /*--> [3.1] Sensor Data Processing */
        if (tick % 40 == 0)
        {
            Sensor_Process();
        }

        /*--> [3.2] Communication Tasks */
        Comm_Handler(&tick);

        // /*--> [3.3] Data Reception */
        // rxData = ESP8266_GetIPD(3);
        // if (rxData != NULL)
        //     OneNet_RevPro(rxData);
        // delay_ms(10);

        /*--> [3.3] Data Reception */
        rxData = ESP8266_GetIPD(3);
        if (rxData != NULL)
        {
            //printf("Received data: %s1234344444444======\r\n", rxData); // 打印接收到的数据
            OneNet_RevPro(rxData);
        }
        delay_ms(10);
    }
}

void HW_Init(void)
{
    /*----------------- 1. UART Configuration -----------------*/
    /*--> [1.1] Debug UART */
    Usart1_Init(115200);
    DEBUG_LOG("\r\n");
    DEBUG_LOG("UART1 Initialization         [OK]");
    delay_init();
    DEBUG_LOG("Delay initialization         [OK]");

    /*----------------- 2. Display Configuration -----------------*/
    /*--> [2.1] OLED Setup */
    delay_ms(500);
    OLED_Init();
    OLED_ColorTurn(0);
    OLED_DisplayTurn(0);
    OLED_Clear();
    DEBUG_LOG("OLED initialization          [OK]");
    OLED_Refresh_Line("OLED");

    /*----------------- 3. System Configuration -----------------*/
    /*--> [3.1] Interrupt Setup */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    DEBUG_LOG("Interrupt priority set       [OK]");
    OLED_Refresh_Line("NVIC");

    /*----------------- 4. Peripheral Initialization -----------------*/
    /*--> [4.1] GPIO Devices */
    LED_Init();
    DEBUG_LOG("LED initialization           [OK]");
    OLED_Refresh_Line("LED");

    KEY_Init();
    DEBUG_LOG("Key initialization           [OK]");
    OLED_Refresh_Line("Key");

    /*--> [4.2] External Interfaces */
    EXTIX_Init();
    DEBUG_LOG("External interrupt initialized [OK]");
    OLED_Refresh_Line("EXIT");

    BEEP_Init();
    DEBUG_LOG("BEEP initialization          [OK]");
    OLED_Refresh_Line("BEEP");

    /*--> [4.3] Sensors */
    DHT11_Init();
    DEBUG_LOG("DHT11 initialization         [OK]");
    OLED_Refresh_Line("DHT11");

    BH1750_Init();
    DEBUG_LOG("BH1750 initialization        [OK]");
    OLED_Refresh_Line("BH1750");

    ADCx_Init();   // Initialize ADC for MQ2 sensor
    DEBUG_LOG("MQ2 initialization           [OK]");
    OLED_Refresh_Line("MQ2");
    

    /*--> [4.4] Communication Interface */
    Usart2_Init(115200);
    DEBUG_LOG("UART2 initialization         [OK]");
    OLED_Refresh_Line("UART2");

    DEBUG_LOG("Peripheral initialization    [OK]");
    delay_ms(1000);
}

void Net_Init(void)
{
    /*----------------- 1. WiFi Module Initialization -----------------*/
    /*--> [1.1] Module Status Check */
    DEBUG_LOG("Initializing ESP8266 WiFi module...");
    if (!ESP8266_INIT_OK)
    {
        OLED_Clear();
        OLED_ShowString(0, 0, (u8 *)"WiFi", 16, 1);
        OLED_ShowChinese(32, 0, 8, 16, 1);
        OLED_ShowChinese(48, 0, 9, 16, 1);
        OLED_ShowChinese(64, 0, 10, 16, 1);
        OLED_ShowString(80, 0, (u8 *)"...", 16, 1);
        OLED_Refresh();
    }

    /*--> [1.2] WiFi Configuration */
    ESP8266_Init();

    /*----------------- 2. Connection Status Display -----------------*/
    OLED_Clear();
    OLED_ShowChinese(0, 0, 4, 16, 1);
    OLED_ShowChinese(16, 0, 5, 16, 1);
    OLED_ShowChinese(32, 0, 6, 16, 1);
    OLED_ShowChinese(48, 0, 8, 16, 1);
    OLED_ShowChinese(64, 0, 9, 16, 1);
    OLED_ShowChinese(80, 0, 10, 16, 1);
    OLED_ShowString(96, 0, (u8 *)"...", 16, 1);
    OLED_Refresh();

    /*----------------- 3. Cloud Connection -----------------*/
    while (OneNet_DevLink())
    {
        delay_ms(500);
    }
    OLED_Clear();
}

void Sensor_Process(void)
{
    /*----------------- 1. Sensor Data Acquisition -----------------*/
    static float RS, R0 = 6.64;  // R0 is the reference resistance
    static u16 smoke_adc_value;
    static float smoke_vol;

    /*--> [1.1] Temperature and Humidity */
    DHT11_Read_Data(&humidityH, &humidityL, &temperatureH, &temperatureL);

    /*--> [1.2] Light Intensity */
    if (!i2c_CheckDevice(BH1750_Addr))
    {
        Light = LIght_Intensity();
    }

    /*--> [1.3] Smoke Sensor Reading */
    smoke_adc_value = Get_ADC_Value(ADC_Channel_1, 100);
    smoke_vol = (float)smoke_adc_value * (3.3 / 4096);
    RS = (5 - smoke_vol) / smoke_vol * 0.5;
    smoke_ppm = pow(11.5428 * R0 / RS, 0.6549f) * 100;
    printf("Smoke: %.2f PPM\n", smoke_ppm);

    /*----------------- 2. Status Monitoring -----------------*/
    /*--> [2.1] LED Status */
    Led_Status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);

    /*--> [2.2] Alarm Logic */
    if (alarm_is_free == 10)
    {
        alarmFlag = ((humidityH >= humidityThreshold)       ||
                     (temperatureH >= temperatureThreshold) ||
                     (Light >= lightThreshold)              ||
                     (smoke_ppm >= smokeThreshold)          );
    }
    if (alarm_is_free < 10)
    {
        alarm_is_free++;
    }

    /*----------------- 3. Data Logging -----------------*/
    // DEBUG_LOG(" | Humidity: %d.%d %% | Temperature: %d.%d C | Light Intensity: %.1f lx | Smoke: %.2f PPM | LED Status: %s | Alarm Status: %s | ",
    //           humidityH, humidityL, temperatureH, temperatureL, Light, smoke_ppm,
    //           Led_Status ? "Off" : "On", alarmFlag ? "Activated" : "Stopped");
}

void Comm_Handler(unsigned short *tick)
{
    /*----------------- 1. Communication Timing -----------------*/
    if (++(*tick) >= 100)
    {
        /*--> [1.1] Status Update */
        Led_Status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
        //DEBUG_LOG("==================================================================================");
        printf("==========================\r\n");

        /*----------------- 2. Data Publishing -----------------*/
        /*--> [2.1] Message Preparation */
        DEBUG_LOG("Publishing to EMQ ----- EMQ_Publish");
        sprintf(PUB_BUF, "{\"temperature\":%d.%d,\"humidity\":%d.%d,\"smoke\":%.2f,"
            "\"latitude\":39.9042,\"longitude\":116.4074,\"light\":\"%s\",\"danger\":%s,"
            "\"temperatureThreshold\":%d,\"humidityThreshold\":%d,\"smokeThreshold\":%.2f}",
            temperatureH, temperatureL,
            humidityH, humidityL,
            smoke_ppm,  // Updated to use actual smoke sensor reading
            Led_Status ? "on" : "off",
            alarmFlag ? "true" : "false",
            temperatureThreshold, humidityThreshold, smokeThreshold);

        /*--> [2.2] Data Transmission */
        OneNet_Publish(devPubTopic, PUB_BUF);

        /*----------------- 3. Cleanup -----------------*/
        printf("==========================\r\n");
        //DEBUG_LOG("==================================================================================");
        *tick = 0;
        ESP8266_Clear();
    }
}

#else
int main(void)
{
    unsigned short timeCount = 0;  // Time counter
    unsigned char *dataPtr = NULL; // Data pointer

    Usart1_Init(115200); // Initialize UART1 for debugging
    DEBUG_LOG("\r\n");
    DEBUG_LOG("UART1 Initialization         [OK]");
    delay_init(); // Initialize delay functions
    DEBUG_LOG("Delay initialization         [OK]");

    delay_ms(500);       // Wait for OLED initialization
    OLED_Init();         // Initialize OLED display
    OLED_ColorTurn(0);   // Set OLED color mode (0 for normal display, 1 for reversed)
    OLED_DisplayTurn(0); // Set OLED display direction (0 for normal, 1 for rotated)
    OLED_Clear();        // Clear OLED screen
    DEBUG_LOG("OLED initialization          [OK]");
    OLED_Refresh_Line("OLED");

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // Set priority group for interrupts
    DEBUG_LOG("Interrupt priority set       [OK]");
    OLED_Refresh_Line("NVIC");

    LED_Init(); // Initialize LED GPIO
    DEBUG_LOG("LED initialization           [OK]");
    OLED_Refresh_Line("LED");

    KEY_Init(); // Initialize button GPIO
    DEBUG_LOG("Key initialization           [OK]");
    OLED_Refresh_Line("Key");

    EXTIX_Init(); // Initialize external interrupt for exit
    DEBUG_LOG("External interrupt initialized [OK]");
    OLED_Refresh_Line("EXIT");

    BEEP_Init(); // Initialize BEEP (beeper)
    DEBUG_LOG("BEEP initialization          [OK]");
    OLED_Refresh_Line("BEEP");

    DHT11_Init(); // Initialize DHT11 sensor
    DEBUG_LOG("DHT11 initialization         [OK]");
    OLED_Refresh_Line("DHT11");

    BH1750_Init(); // Initialize BH1750 light sensor
    DEBUG_LOG("BH1750 initialization        [OK]");
    OLED_Refresh_Line("BH1750");

    Usart2_Init(115200); // Initialize UART2 for STM32-ESP8266 communication
    DEBUG_LOG("UART2 initialization         [OK]");
    OLED_Refresh_Line("UART2");

    DEBUG_LOG("Peripheral initialization    [OK]");

    delay_ms(1000);

    DEBUG_LOG("Initializing ESP8266 WiFi module...");
    if (!ESP8266_INIT_OK)
    {
        OLED_Clear();
        OLED_ShowString(0, 0, (u8 *)"WiFi", 16, 1);
        OLED_ShowChinese(32, 0, 8, 16, 1);  // Show Chinese characters (message part 1)
        OLED_ShowChinese(48, 0, 9, 16, 1);  // Show Chinese characters (message part 2)
        OLED_ShowChinese(64, 0, 10, 16, 1); // Show Chinese characters (message part 3)
        OLED_ShowString(80, 0, (u8 *)"...", 16, 1);

        OLED_Refresh();
    }
    ESP8266_Init(); // Initialize ESP8266 WiFi module

    OLED_Clear();
    OLED_ShowChinese(0, 0, 4, 16, 1);   // Show Chinese characters (message part 4)
    OLED_ShowChinese(16, 0, 5, 16, 1);  // Show Chinese characters (message part 5)
    OLED_ShowChinese(32, 0, 6, 16, 1);  // Show Chinese characters (message part 6)
    OLED_ShowChinese(48, 0, 8, 16, 1);  // Show Chinese characters (message part 7)
    OLED_ShowChinese(64, 0, 9, 16, 1);  // Show Chinese characters (message part 8)
    OLED_ShowChinese(80, 0, 10, 16, 1); // Show Chinese characters (message part 9)
    OLED_ShowString(96, 0, (u8 *)"...", 16, 1);
    OLED_Refresh();

    while (OneNet_DevLink())
    { // Wait for OneNET device connection
        delay_ms(500);
    }

    OLED_Clear();

    TIM2_Int_Init(4999, 7199); // Initialize Timer 2 for interrupts
    TIM3_Int_Init(2499, 7199); // Initialize Timer 3 for interrupts

    BEEP = 0; // Turn off the beep
    delay_ms(250);
    BEEP = 1; // Turn on the beep

    OneNet_Subscribe(devSubTopic, 1); // Subscribe to OneNET device topic

    while (1)
    {
        if (timeCount % 40 == 0) // 1000ms / 25 = 40 iterations per second
        {
            /********** Waiting to acquire data from the sensor **************/
            DHT11_Read_Data(&humidityH, &humidityL, &temperatureH, &temperatureL);

            /********** Waiting to acquire data from the light sensor **************/
            if (!i2c_CheckDevice(BH1750_Addr))
            {
                Light = LIght_Intensity();
            }

            /********** Getting the LED0 status **************/
            Led_Status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);

            /********** Alarm logic **************/
            if (alarm_is_free == 10) // Initial value for alarm_is_free is set to 10
            {
                if ((humidityH < 80) && (temperatureH < 30) && (Light < 1000))
                    alarmFlag = 0;
                else
                    alarmFlag = 1;
            }
            if (alarm_is_free < 10)
                alarm_is_free++;
            //      DEBUG_LOG("alarm_is_free = %d", alarm_is_free);
            //      DEBUG_LOG("alarmFlag = %d\r\n", alarmFlag);

            /********** Logging data **************/
            DEBUG_LOG(" | Humidity: %d.%d %% | Temperature: %d.%d C | Light Intensity: %.1f lx | LED Status: %s | Alarm Status: %s | ",
                      humidityH, humidityL, temperatureH, temperatureL, Light,
                      Led_Status ? "Off" : "On", alarmFlag ? "Activated" : "Stopped");
        }
        if (++timeCount >= 200) // 5000ms / 25 = 200 iterations for 5000ms
        {
            Led_Status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4); // Get LED0 status
            DEBUG_LOG("==================================================================================");
            // DEBUG_LOG("Publishing to OneNet ----- OneNet_Publish");
            // sprintf(PUB_BUF, "{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Light\":%.1f,\"Led\":%d,\"Beep\":%d}",
            //         humidityH, humidityL, temperatureH, temperatureL, Light, Led_Status ? 0 : 1, alarmFlag);
            // OneNet_Publish(devPubTopic, PUB_BUF);

            // ... existing code ...
            DEBUG_LOG("Publishing to OneNet ----- OneNet_Publish");
            sprintf(PUB_BUF, "{\"temperature\":%d.%d,\"humidity\":%d.%d,\"smoke\":%d,\"latitude\":39.9042,\"longitude\":116.4074,\"light\":\"%s\",\"danger\":%s}",
                    temperatureH, temperatureL,    // Temperature value from sensor
                    humidityH, humidityL,          // Humidity value from sensor
                    0,                             // Smoke detection status is 0 (not triggered)
                    Led_Status ? "on" : "off",     // Light status converted from Led_Status
                    alarmFlag ? "true" : "false"); // Danger status based on alarmFlag
            OneNet_Publish(devPubTopic, PUB_BUF);
            // ... existing code ...
            DEBUG_LOG("==================================================================================");
            timeCount = 0;
            ESP8266_Clear();
        }

        dataPtr = ESP8266_GetIPD(3);
        if (dataPtr != NULL)
            OneNet_RevPro(dataPtr);
        delay_ms(10);
    }
}

// int main()
// {
//     u8 i = 0;
//     u16 value = 0;
//     float RS;
//     float R0;
//     float vol;
//     float ppm;

//     // SysTick_Init(72);   // Initialize SysTick timer with 72MHz system clock
//     // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // Configure interrupt priority grouping into 2 groups
//     // LED_Init();   // Initialize LED
//     Usart1_Init(115200);   // Initialize USART1 for debugging output
//     ADCx_Init();   // Initialize ADC1 for MQ2 sensor

// 	  delay_init();  // Initialize delay functions
//     DEBUG_LOG("Delay initialization         [OK]");

//     while(1)
//     {
//         i++;
// //        if(i % 20 == 0)
// //        {
// //            LED1 = !LED1;  // Toggle LED every 20 iterations
// //        }
// //
//         if(i % 50 == 0)
//         {
//             value = Get_ADC_Value(ADC_Channel_1, 20);  // Get ADC value from PA1 (ADC_Channel_1)
//             printf("Detected ADC value: %d\r\n", value);

//             vol = (float)value * (3.3 / 4096);  // Convert ADC value to voltage
//             printf("Detected voltage: %.2fV\r\n", vol);

//             RS = (5 - vol) / vol * 0.5;  // Calculate resistance based on voltage
//             R0 = 6.64;  // Reference resistance for the sensor
//             ppm = pow(11.5428 * R0 / RS, 0.6549f) * 100;  // Calculate the ppm (smoke concentration) value
//             printf("Detected smoke ppm: %.2f PPM\r\n", ppm);
//             printf("\n");
//         }
//         delay_ms(10);   // Small delay to allow time for system to stabilize
//     }
// }
#endif // end of #ifdef Version_Rollback
