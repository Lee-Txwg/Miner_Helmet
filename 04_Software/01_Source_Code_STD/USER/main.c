#include "main.h"

volatile u8 alarmFlag = 0;      // Alarm flag status
volatile u8 alarm_is_free = 10; // Check if the alarm system is free, 0 means no free time

u8    humidityH;
u8    humidityL;
u8    temperatureH;
u8    temperatureL;
float smoke_ppm;
extern nmea_msg gpsx; 
uint8_t key=0XFF;

// Global threshold variables with default values
volatile u8    temperatureThreshold = 30; // Default: 30°C
volatile u8    humidityThreshold = 80;    // Default: 80%
volatile float smokeThreshold = 2000;       // Default: 50 (example value)

// The light threshold remains unchanged
const float lightThreshold = 1000.0;

extern char oledBuf[20]; // OLED display buffer
float Light = 0;         // Light intensity
u8 Led_Status = 0;       // LED status

char PUB_BUF[256];                          // Publish data buffer
const char *devSubTopic[] = {"helmet/cmd"}; // Subscribe topic for commands
const char devPubTopic[] = "helmet/status"; // Publish topic for status
u8 ESP8266_INIT_OK = 0;                     // ESP8266 initialization status


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
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
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

    /*--- [4.5] GPS UART (USART3) ---*/
    // 1) Configure the GPIO pins for USART3 (PB10/PB11)
    GPIO_Config_Init();   
    DEBUG_LOG("GPIOB10/11 for USART3 set  [OK]");
    OLED_Refresh_Line("GPIO3");

    // 2) Initialize USART3 at 38.4 kbps with interrupts
    USART_Config_Init();
    DEBUG_LOG("USART3 @38400bps init       [OK]");
    OLED_Refresh_Line("UART3");

    // 3) Enable the NVIC for USART3 IRQ
    NVIC_Configuration();
    DEBUG_LOG("NVIC for USART3 enabled     [OK]");
    OLED_Refresh_Line("NVIC3");

    if (Ublox_Cfg_Rate(1000, 1) != 0)   // Set update rate to 1000 ms and check module status
    {
        while ((Ublox_Cfg_Rate(1000, 1) != 0) && key)   // Retry until NEO-6M responds and settings save
        {
            USART_BaudRate_Init(9600);                  // Init UART2 at 9600 baud (default)
            Ublox_Cfg_Prt(38400);                       // Change module baud rate to 38400
            Ublox_Cfg_Tp(1000000, 100000, 1);           // PPS: 1 Hz pulse, 100 ms width
            key = Ublox_Cfg_Cfg_Save();                 // Save configuration
        }
        delay_ms(500);                                  // Wait 500 ms
    }
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
    static float RS, R0 = 6.64f;        /* R0: reference resistance */
    static uint16_t smoke_adc_value;
    static float smoke_vol;

    /*--> [1.1] Update GPS data */
    GpsDataRead();  // fills gpsx and updates latitude & longitude globals

    /*--> [1.2] Read temperature & humidity */
    DHT11_Read_Data(&humidityH, &humidityL, &temperatureH, &temperatureL);

    /*--> [1.3] Smoke sensor reading */
    smoke_adc_value = Get_ADC_Value(ADC_Channel_1, 100);
    smoke_vol       = (float)smoke_adc_value * (3.3f / 4096.0f);
    RS              = (5.0f - smoke_vol) / smoke_vol * 0.5f;
    smoke_ppm       = powf(11.5428f * R0 / RS, 0.6549f) * 100.0f;
    printf("Smoke: %.2f PPM\r\n", smoke_ppm);

    /*----------------- 2. Status Monitoring -----------------*/
    /*--> [2.1] Read LED status */
    Led_Status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);

    /*--> [2.2] Alarm logic */
    if (10 == alarm_is_free)
    {
        alarmFlag = ((humidityH    >= humidityThreshold)       ||
                     (temperatureH >= temperatureThreshold)    ||
                     (smoke_ppm    >= smokeThreshold));
    }

    if (alarm_is_free < 10)
    {
        alarm_is_free++;
    }

    /*----------------- 3. Data Logging -----------------*/
    // Add DEBUG_LOG here if detailed output is needed
}


void Comm_Handler(unsigned short *tick)
{
    /* Convert raw GPS data to floating-point degrees */
    float lon = gpsx.longitude / 100000.0f;
    float lat = gpsx.latitude  / 100000.0f;
    char  ew_hemi = gpsx.ewhemi;   // 'E' or 'W'
    char  ns_hemi = gpsx.nshemi;   // 'N' or 'S'

    /*----------------- 1. Communication Timing -----------------*/
    if (100 <= ++(*tick))
    {
        /*--> [1.1] Read LED status */
        Led_Status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);

        printf("================================\r\n");

        /*----------------- 2. Data Publishing -----------------*/
        /*--> [2.1] Prepare JSON payload */
        DEBUG_LOG("Publishing to EMQ - EMQ_Publish");

        sprintf(PUB_BUF,
            "{"
                "\"temperature\":%d.%d,"
                "\"humidity\":%d.%d,"
                "\"smoke\":%.2f,"
                "\"latitude\":%.5f,"
                "\"latitudeHem\":\"%c\","
                "\"longitude\":%.5f,"
                "\"longitudeHem\":\"%c\","
                "\"light\":\"%s\","
                "\"danger\":%s,"
                "\"temperatureThreshold\":%d,"
                "\"humidityThreshold\":%d,"
                "\"smokeThreshold\":%.2f"
            "}",
            temperatureH, temperatureL,
            humidityH,    humidityL,
            smoke_ppm,
            lat,          ns_hemi,
            lon,          ew_hemi,
            (Led_Status == 1) ? "on"  : "off",
            (alarmFlag  == 1) ? "true" : "false",
            temperatureThreshold,
            humidityThreshold,
            smokeThreshold
        );

        /*--> [2.2] Publish to OneNet */
        OneNet_Publish(devPubTopic, PUB_BUF);

        /*----------------- 3. Cleanup -----------------*/
        printf("================================\r\n");
        *tick = 0;            /* Reset tick counter */
        ESP8266_Clear();      /* Clear ESP8266 UART buffers */
    }
}


