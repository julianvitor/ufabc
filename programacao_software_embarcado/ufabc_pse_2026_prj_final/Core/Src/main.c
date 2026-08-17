/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 * notas: J Ranhel - rev 2026.07
 * PA5 =1 na entrada do callBack do RxCpltCallback(), =0 na saída
 * PB7 =1 na entrada do PeriodElapsedCallback(), =0 ma saída
 *
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// --- veja includes, macro, e constantes no main.h ---
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// ---- ver vários #defines (kts, delays, etc) no main.h ----
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//  --- ver macros definidas no main.h ---
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 64 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for task_interface */
osThreadId_t task_interfaceHandle;
const osThreadAttr_t task_interface_attributes = {
    .name = "task_interface",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for task_ping */
osThreadId_t task_pingHandle;
const osThreadAttr_t task_ping_attributes = {
    .name = "task_ping",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for task_send_uart */
osThreadId_t task_send_uartHandle;
const osThreadAttr_t task_send_uart_attributes = {
    .name = "task_send_uart",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for q_uart_msgs */
osMessageQueueId_t q_uart_msgsHandle;
const osMessageQueueAttr_t q_uart_msgs_attributes = {.name = "q_uart_msgs"};
/* Definitions for ef_tudo */
osEventFlagsId_t ef_tudoHandle;
const osEventFlagsAttr_t ef_tudo_attributes = {.name = "ef_tudo"};
/* USER CODE BEGIN PV */
// variáveis que todos vamos usar: buffers de entrada/saída na comunicação
// buffers para entrada e saida de dados via USART
uint8_t BufOUT[] = {'0', '0', '0', '0', '0'};  // inicia buffer OUT com cars "0"
uint8_t BufIN[] = {'0', '0', '0', '0', '0'};   // inicia buffer IN com cars "0"
int8_t DspHex[] = {0x10, 0x10, 0x10, 0x10};    // vetor val display (se=16 => off)
size_t sizeBuffs = sizeof(BufOUT);             // tamanho dos buffers - usa geral
// qual dig liga pto? (ex: 0xA=>1010=> 1000=MSD + 0010=DG2)
uint8_t ptoDec = 0;

// os vetores abaixo tem idx[0] = digito menos significativo no display
int8_t Crono[] = {0, 0, 0, 0};     // vetor com vals decimais do cronometro
int8_t ValAdc[] = {0, 0, 0, 0};    // vetor com vals decimais do ADC
int8_t ExCrono[] = {0, 0, 0, 0};   // vetor com vals decimais do cronometro
int8_t ExValAdc[] = {0, 0, 0, 0};  // vetor com vals decimais do ADC
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void* argument);
void f_task_interface(void* argument);
void f_task_ping(void* argument);
void f_task_send_uart(void* argument);

static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();

    /* Initialize interrupts */
    MX_NVIC_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Init scheduler */
    osKernelInitialize();

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the queue(s) */
    /* creation of q_uart_msgs */
    q_uart_msgsHandle = osMessageQueueNew(15, sizeof(uint8_t), &q_uart_msgs_attributes);

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of task_interface */
    task_interfaceHandle = osThreadNew(f_task_interface, NULL, &task_interface_attributes);

    /* creation of task_ping */
    task_pingHandle = osThreadNew(f_task_ping, NULL, &task_ping_attributes);

    /* creation of task_send_uart */
    task_send_uartHandle = osThreadNew(f_task_send_uart, NULL, &task_send_uart_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */

    /* USER CODE END RTOS_THREADS */

    /* Create the event(s) */
    /* creation of ef_tudo */
    ef_tudoHandle = osEventFlagsNew(&ef_tudo_attributes);

    /* USER CODE BEGIN RTOS_EVENTS */
    // Iniciar fl_teste e fl_ping como 1 e o resto como 0
    osEventFlagsSet(ef_tudoHandle, FLAG_MASK_TESTE | FLAG_MASK_PING);
    HAL_UART_Receive_DMA(&huart1, BufIN, sizeBuffs);
    /* USER CODE END RTOS_EVENTS */

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief NVIC Configuration.
 * @retval None
 */
static void MX_NVIC_Init(void) {
    /* EXTI3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    /* EXTI2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    /* EXTI1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    /* ADC1_2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
    /* USART1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {
    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */

    /** Common config
     */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
     */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {
    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel4_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    /* DMA1_Channel5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_6 | GPIO_PIN_9, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_5,
                      GPIO_PIN_SET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PA1 PA2 PA3 */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PB10 PB12 PB13 PB14
                             PB15 PB5 PB6 PB9 */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 |
                          GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
    if (huart->Instance == USART1) {
        // 1. Aborta qualquer recepção bugada que ficou pela metade
        HAL_UART_AbortReceive(huart);

        // 2. Limpa o buffer de entrada para evitar processar lixo
        BufIN[0] = '0';
        BufIN[1] = '0';
        BufIN[2] = '0';
        BufIN[3] = '0';
        BufIN[4] = '0';

        // 3. Reinicia a escuta do DMA limpa e alinhada do zero
        HAL_UART_Receive_DMA(&huart1, BufIN, sizeBuffs);
    }
}

// fn que atende ao callback da ISR do conversor ADC1
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    uint16_t val_adc = 0;                    // define var para ler ADC
    if (hadc->Instance == ADC1) {            // se veio ADC1
        val_adc = HAL_ADC_GetValue(&hadc1);  // capta valor adc
        // converter o valor lido em valores hexa p/ display
        int miliVolt = val_adc * 3300 / 4095;
        int uniADC = miliVolt / 1000;
        int decADC = (miliVolt - (uniADC * 1000)) / 100;
        int cnsADC = (miliVolt - (uniADC * 1000) - (decADC * 100)) / 10;
        int mlsADC = miliVolt - (uniADC * 1000) - (decADC * 100) - (cnsADC * 10);
        ValAdc[3] = uniADC;  // dig mais significativo
        ValAdc[2] = decADC;
        ValAdc[1] = cnsADC;
        ValAdc[0] = mlsADC;  // dig menos significativo
    }
}

// fn que atende ao callback da ISR quando RECEBE dado pela UART via ISR/DMA
// recebeu:    1 = val extCRN  2 = val extADC  3 = reqCRN
// 4 = reqADC  5 = reqSRV      6 = REQOFF      7 = MSGDND
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    // o que veio na UART1?
    if (huart->Instance == USART1) {
        // Mantem as funções de ping ativas mesmo durante a inicialização
        if (BufIN[4] == '?') {
            SEND_MSG(sndRESPING);
        } else if (BufIN[4] == '!') {
            osEventFlagsSet(ef_tudoHandle, FLAG_MASK_RESPPING);
        } else if (osKernelGetTickCount() > DT_INICIALIZACAO) {
            if (BufIN[0] == 'a') {
                // o que vc vai fazer aqui?
                // se veio um valor iniciado com 'axxxx", veio o valor do ADC
            } else if (BufIN[0] == 'c') {
                // o que vc vai fazer aqui?
                // se veio um valor iniciado com 'cxxxx", veio o valor do CRONO
            } else if (BufIN[2] == 'a') {
                // o que vc vai fazer aqui?
                // se veio "rqadc", o cliente solicitou o dado do ADC
            } else if (BufIN[2] == 's') {
                // o que vc vai fazer aqui?
                // se veio "rqsrv" esta' solicitando p/ placa ATUAR como Server
            } else if (BufIN[2] == 'o') {
                // o que vc vai fazer aqui?
                // se veio "rqoff" esta' solicitando PARAR de atuar como Server
            }
        }
    }
    // redispara a UART para receber dados novamente pelo DMA controller
    HAL_UART_Receive_DMA(&huart1, BufIN, sizeBuffs);
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void* argument) {
    /* USER CODE BEGIN 5 */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_f_task_interface */
/**
 * @brief Function implementing the task_interface thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_f_task_interface */
void f_task_interface(void* argument) {
    /* USER CODE BEGIN f_task_interface */
    int8_t DspTeste[] = {8, 8, 8, 8};
    uint8_t ptoTeste = 0x0F;
    /* Infinite loop */
    for (;;) {
        if (GET_FLAG_TESTE(ef_tudoHandle)) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                              GPIO_PIN_RESET);
            mostrar_no_display(DspTeste, ptoTeste);
        } else {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                              GPIO_PIN_SET);
            mostrar_no_display(DspHex, 0);
        }

        if (GET_FLAG_PING(ef_tudoHandle)) {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        }

        osDelay(DT_MUX_DISP);
    }
    /* USER CODE END f_task_interface */
}

/* USER CODE BEGIN Header_f_task_ping */
/**
 * @brief Function implementing the task_ping thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_f_task_ping */
void f_task_ping(void* argument) {
    /* USER CODE BEGIN f_task_ping */

    osDelay(DT_INICIALIZACAO);

    uint8_t fl_esperando_resp = 0;
    uint32_t t_ping = osKernelGetTickCount();

    /* Infinite loop */
    for (;;) {
        uint32_t now = osKernelGetTickCount();
        // Verifica se estourou o tempo de timeout do Ping
        if ((now - t_ping) > DT_PING) {
            if (fl_esperando_resp == 1) {
                // ERRO: O tempo passou e não houve resposta
                // Limpa a flag ping
                osEventFlagsClear(ef_tudoHandle, FLAG_MASK_PING);
                fl_esperando_resp = 0;
            } else {
                // Enviar um novo Ping
                SEND_MSG(sndREQPING);
                fl_esperando_resp = 1;
                t_ping = now;  // Reinicia o cronômetro do ping
            }
        } else {
            // Se ainda está dentro do tempo e estamos esperando a resposta
            if (fl_esperando_resp == 1) {
                // Verifica se a flag resping para ver se houve reposta
                if (GET_FLAG_RESPPING(ef_tudoHandle)) {
                    // (OK) Resposta do ping recebida
                    fl_esperando_resp = 0;

                    // Limpa a flag de resposta
                    osEventFlagsClear(ef_tudoHandle, FLAG_MASK_RESPPING);

                    // Garante que a flag ping esteja ativa sinalizando conexão saudável
                    osEventFlagsSet(ef_tudoHandle, FLAG_MASK_PING);
                }
            }
        }

        osDelay(1);
    }
    /* USER CODE END f_task_ping */
}

/* USER CODE BEGIN Header_f_task_send_uart */
/**
 * @brief Function implementing the task_send_uart thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_f_task_send_uart */
void f_task_send_uart(void* argument) {
    /* USER CODE BEGIN f_task_send_uart */
    EvtEnviar msg_rec = sndNADA;
    int8_t adc_crn_local[4] = {0, 0, 0, 0};
    /* Infinite loop */
    for (;;) {
        if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX) {
            // UART está ocupada transmitindo, então não podemos enviar nada agora
            osDelay(1);
            continue;
        }

        if (osMessageQueueGetCount(q_uart_msgsHandle) == 0) {
            osDelay(1);
            continue;
        }
        msg_rec = sndNADA;

        if (osMessageQueueGet(q_uart_msgsHandle, &msg_rec, NULL, 0) == osOK) {
            switch (msg_rec) {
                case sndREQADC:
                    STR_BUFF(REQADC);
                    break;
                case sndREQSRV:
                    STR_BUFF(REQSRV);
                    break;
                case sndREQOFF:
                    STR_BUFF(REQOFF);
                    break;
                case sndMSGNSV:
                    STR_BUFF(MSGNSV);
                    break;
                case sndREQPING:
                    STR_BUFF(PNGPRG);
                    break;
                case sndRESPING:
                    STR_BUFF(PNGRSP);
                    break;

                case sndREQCRN:
                    STR_BUFF(REQCRN);
                    break;

                case sndADC:
                    taskENTER_CRITICAL();
                    adc_crn_local[0] = ValAdc[0];
                    adc_crn_local[1] = ValAdc[1];
                    adc_crn_local[2] = ValAdc[2];
                    adc_crn_local[3] = ValAdc[3];
                    taskEXIT_CRITICAL();
                    BufOUT[0] = 'a';
                    BufOUT[1] = conv_num_ASC(adc_crn_local[3]);  // min
                    BufOUT[2] = conv_num_ASC(adc_crn_local[2]);  // seg
                    BufOUT[3] = conv_num_ASC(adc_crn_local[1]);  // seg(unidade)
                    BufOUT[4] = conv_num_ASC(adc_crn_local[0]);  // decimos
                    break;

                case sndCRN:
                    taskENTER_CRITICAL();
                    adc_crn_local[0] = Crono[0];
                    adc_crn_local[1] = Crono[1];
                    adc_crn_local[2] = Crono[2];
                    adc_crn_local[3] = Crono[3];
                    taskEXIT_CRITICAL();
                    BufOUT[0] = 'c';
                    BufOUT[1] = conv_num_ASC(adc_crn_local[3]);  // min
                    BufOUT[2] = conv_num_ASC(adc_crn_local[2]);  // seg
                    BufOUT[3] = conv_num_ASC(adc_crn_local[1]);  // seg(unidade)
                    BufOUT[4] = conv_num_ASC(adc_crn_local[0]);  // decimos

                    break;

                case sndNADA:
                    break;
            }

            HAL_UART_Transmit_DMA(&huart1, BufOUT, sizeBuffs);
        }

        osDelay(1);
    }
    /* USER CODE END f_task_send_uart */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM4 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    /* USER CODE BEGIN Callback 0 */
    uint16_t static contaCRN = 0;  // conta num vezes até DT-CRONO
    uint16_t static contaADC = 0;  // conta num vezes até DT-CRONO

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM4) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */
    // CRONOMETRO UP/DOWN: teste se já passou DT_CRONO ms
    if (contaCRN >= DT_CRONO) {
        contaCRN = 0;                        // retorna conta para zero
        if (MD_CRONO == 0) {                 // MD_CRONO = 0 incrementa o cronômetro
            ++Crono[0];                      // inc decimo seg (dígito menos signif)
            if (Crono[0] > 9) {              // se > 9
                Crono[0] = 0;                // volta p/ zero
                ++Crono[1];                  // inc unidade de segundo
                if (Crono[1] > 9) {          // se > 9
                    Crono[1] = 0;            // volta p/ zero
                    ++Crono[2];              // inc dezena de segundos
                    if (Crono[2] > 5) {      // se > 5
                        Crono[2] = 0;        // volta p/ zero
                        ++Crono[3];          // inc minutos  (dígito mais signif)
                        if (Crono[3] > 9) {  // se > 9
                            Crono[3] = 0;    // volta p/ zero
                        }
                    }
                }
            }
        } else {                             // MD_CRONO = 1 decrementa o cronômetro
            --Crono[0];                      // dec decimo de segundos
            if (Crono[0] < 0) {              // se < 0
                Crono[0] = 9;                // volta p/ 9
                --Crono[1];                  // dec unidade de segundo
                if (Crono[1] < 0) {          // se < 0
                    Crono[1] = 9;            // volta p/ 9
                    --Crono[2];              // dec dezena de segundos
                    if (Crono[2] < 0) {      // se < 0
                        Crono[2] = 5;        // volta p/ 5
                        --Crono[3];          // dec minutos
                        if (Crono[3] < 0) {  // se < 0
                            Crono[3] = 9;    // volta p/ 9
                        }
                    }
                }
            }
        }
    } else {
        ++contaCRN;
    }  // se nao muda crono, so' inc contaCRN

    // se passou o tempo para disparar o ADC, disparar
    if (contaADC >= DT_ADC) {      // contou DT_ADC ms?
        contaADC = 0;              // retorna conta para zero
        HAL_ADC_Start_IT(&hadc1);  // dispara ADC p/ conversão por IRQ
    } else {
        ++contaADC;
    }  // se nao muda crono, so' inc contaADC
    /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
