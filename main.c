#include "stdint.h"
#include "stdbool.h"
#include "math.h"
#include "string.h"
#include "inc/hw_adc.h"
//#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_timer.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "inc/tm4c123gh6pm.h"

//Defines
// defini��es da comunica��o serial
#define UART_BAUD   115200
#define UART_CONFIG (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8)
// pinos do DAC
#define DAC_GPIO_BASE   GPIO_PORTB_BASE
#define DAC_GPIO_PERIPH SYSCTL_PERIPH_GPIOB
#define GPIO_PIN_DAC    0xFF
// defini��es do DAC
#define DAC_MAX         0xFF
#define DAC_MED         0x7F
#define DAC_MIN         0x00


//Prot�tipos de fun��o.
void ConfigureUART(void);
void ConfigurePins(void);
void ConfigureTimers(void);
void SetTimerFreq(uint32_t timer, uint32_t freq);
void ConfigureADC(void);


//Vari�veis globais
// Da senoide gerada
unsigned short sen_frq=10;     //em kHz
unsigned short sen_res=128;    //numero de pontos da senoide


int adc_buffer[200];
int adc_start = 0;
int i = 0;
uint32_t pui32ADC0Value[1];

//*****************************************************************************
// Tabela de controle para o controlador uDMA.
//*****************************************************************************
#pragma DATA_ALIGN(uDMA_table, 1024)
uint8_t uDMA_table[1024];
tDMAControlTable *uDMA_table_end = (tDMAControlTable *)uDMA_table;

unsigned char seno[128] = { 128 ,   133 ,   138 ,   143 ,   148 ,   152 ,   157 ,   162 ,
                            166 ,   171 ,   175 ,   179 ,   184 ,   188 ,   191 ,   195 ,
                            199 ,   202 ,   205 ,   208 ,   211 ,   214 ,   216 ,   218 ,
                            220 ,   222 ,   224 ,   225 ,   226 ,   227 ,   228 ,   228 ,
                            228 ,   228 ,   228 ,   227 ,   226 ,   225 ,   224 ,   222 ,
                            220 ,   218 ,   216 ,   214 ,   211 ,   208 ,   205 ,   202 ,
                            199 ,   195 ,   191 ,   188 ,   184 ,   179 ,   175 ,   171 ,
                            166 ,   162 ,   157 ,   152 ,   148 ,   143 ,   138 ,   133 ,
                            128 ,   123 ,   118 ,   113 ,   108 ,   104 ,   99  ,   94  ,
                            90  ,   85  ,   81  ,   77  ,   72  ,   68  ,   65  ,   61  ,
                            57  ,   54  ,   51  ,   48  ,   45  ,   42  ,   40  ,   38  ,
                            36  ,   34  ,   32  ,   31  ,   30  ,   29  ,   28  ,   28  ,
                            28  ,   28  ,   28  ,   29  ,   30  ,   31  ,   32  ,   34  ,
                            36  ,   38  ,   40  ,   42  ,   45  ,   48  ,   51  ,   54  ,
                            57  ,   61  ,   65  ,   68  ,   72  ,   77  ,   81  ,   85  ,
                            90  ,   94  ,   99  ,   104 ,   108 ,   113 ,   118 ,   123};


void ConfigurePins(void)
{
    SysCtlPeripheralDisable(DAC_GPIO_PERIPH);
    SysCtlPeripheralReset(DAC_GPIO_PERIPH);
    SysCtlPeripheralEnable(DAC_GPIO_PERIPH);
    SysCtlDelay(10);

    GPIOPinTypeGPIOOutput(DAC_GPIO_BASE, GPIO_PIN_DAC);
    GPIOPinWrite(DAC_GPIO_BASE,0xFF,0x0);
}

void ConfigureUART(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    // Habilita o GPIO usado pela UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);    // Habilita o m�dulo UART

    // Configura os pinos da GPIO para o modo UART
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);                   // Usa o oscilador interno de 16MHz como clock para a UART
    UARTConfigSetExpClk(UART0_BASE, 16000000, UART_BAUD, UART_CONFIG);  // configura o baud rate e n�mero de bits da UART

    // habilita as interrup��es da Uart
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    IntEnable(INT_UART0);
}

void ConfigureTimers(void){



      //TimerIntClear(TIMER0_BASE,TIMER_TIMA_DMA);
      //TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0IntHandler);
      //TimerIntEnable(TIMER0_BASE,TIMER_TIMA_DMA);

      //IntEnable(INT_TIMER1A);

      //TimerIntEnable( TIMER1_BASE, TIMER_TIMA_TIMEOUT );
      //IntMasterEnable();
      //TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
      //TimerDMAEventSet(TIMER1_BASE,TIMER_DMA_TIMEOUT_A);

}


void ConfigureuDMA(void)
{
        SysCtlPeripheralDisable(SYSCTL_PERIPH_TIMER1);
        SysCtlPeripheralReset(SYSCTL_PERIPH_TIMER1);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
        SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER1);

        TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
        TimerLoadSet(TIMER1_BASE, TIMER_A, (80000000-1)/((sen_frq*1000)*sen_res));
        TimerEnable(TIMER1_BASE,TIMER_A);
        IntEnable(INT_TIMER1A);




        //Disable all the atributes in case any was set
        uDMAChannelAttributeDisable(UDMA_CH18_TIMER1A,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

        uDMAChannelControlSet(UDMA_CH18_TIMER1A | UDMA_PRI_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE |
        UDMA_ARB_1);

        uDMAChannelControlSet(UDMA_CH18_TIMER1A | UDMA_ALT_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE |
        UDMA_ARB_1);


        uDMAChannelTransferSet(UDMA_CH18_TIMER1A | UDMA_PRI_SELECT,
        UDMA_MODE_PINGPONG,
        (void *)seno, (void *)(DAC_GPIO_BASE + 0x3FC),
        128);
        uDMAChannelTransferSet(UDMA_CH18_TIMER1A | UDMA_ALT_SELECT,
        UDMA_MODE_PINGPONG,
        (void *)seno, (void *)(DAC_GPIO_BASE + 0x3FC),
        128);

        //Enable the DMA channel
        uDMAChannelEnable(UDMA_CH18_TIMER1A);


}

void SetTimerFreq(uint32_t timer, uint32_t freq){
    TimerLoadSet(timer, TIMER_A, MAP_SysCtlClockGet()/freq);
}

void ConfigureADC(void){

    //
    // Display the setup on the console.
    //
    UARTprintf("ADC ->\n");
    UARTprintf("  Type: Single Ended\n");
    UARTprintf("  Samples: One\n");
    UARTprintf("  Update Rate: 250ms\n");
    UARTprintf("  Input Pin: AIN0/PE3\n\n");

    //
    // The ADC0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //
    // For this example ADC0 is used with AIN0 on port E7.
    // The actual port and pins used may be different on your part, consult
    // the data sheet for more information.  GPIO port E needs to be enabled
    // so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    //
    // Select the analog ADC function for these pins.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);

    //
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
    // to sequence 3.  This example is arbitrarily using sequence 3.
    //
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // information on the ADC sequences and steps, reference the datasheet.
    //
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC0_BASE, 3);
}

void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

void main()
{
    FPULazyStackingEnable();
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ConfigurePins();
    InitConsole();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UDMA);
    IntEnable(INT_UDMAERR);
    uDMAEnable();
    uDMAControlBaseSet(uDMA_table);
    ConfigureuDMA();
    //ConfigureTimers();
while(1)
{

}

}

