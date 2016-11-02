#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

//void PortBIntHandler(void);
void PortBInit(void);
void PortBIntHandler(void);
volatile unsigned long FallingEdges = 0;
//volatile unsigned long ui32Loop,Count;
//volatile unsigned long Old_Count,Elapsed_Count;
//volatile unsigned long New_Count=0;
volatile uint32_t g_ui32Freq = 0;
volatile uint32_t g_ui32Counter = 0;
volatile uint32_t g_ui32Freq1=0;


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

void
SysTickIntHandler(void)
{
    //
    // Update the Systick interrupt counter.
    //
	SysTickIntRegister(PortBIntHandler); //Check for Rising Edges for 1 sec period
    g_ui32Counter++; //increment counter
}

int
main(void)
{

    uint32_t ui32PrevCount = 0;


    //
// Set the clocking to run directly from the external crystal/oscillator.    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for Systick operation.
    //
    InitConsole();

    //
    // Display the setup on the console.
    //
    UARTprintf("SysTick Firing Interrupt ->");
    UARTprintf("\n   Rate = 1sec\n\n");
    //
    // Initialize the interrupt counter.
    //
    g_ui32Counter = 0;

    //
    // Set up the period for the SysTick timer.  The SysTick timer period will
    // be equal to the system clock, resulting in a period of 1 second.
    //

    SysTickPeriodSet(SysCtlClockGet());


    //
    // Enable interrupts to the processor.
    //
    IntMasterEnable();

    //
    // Enable the SysTick Interrupt.
    //
    SysTickIntEnable();

    //
    // Enable SysTick.
    //
    SysTickEnable();
    PortBInit();
    //
    // Loop forever while the SysTick runs.
    //
    while(1)
    {
        //
       // Check to see if systick interrupt count changed, and if so then
        // print a message with the count.
        //
        if(ui32PrevCount != g_ui32Counter)
        {
            //
            // Print the interrupt counter.
        	//g_ui32Freq1= g_ui32Freq/100;
            //
            UARTprintf("Frequency : %d Hz\r", g_ui32Freq);
            ui32PrevCount = g_ui32Counter;
        }
    }
}

void PortBInit(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlDelay(10);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_0);
	//GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3);
	GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_0,GPIO_RISING_EDGE);
	IntEnable(INT_GPIOB);
	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0);
}

void PortBIntHandler(void){

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0);//Clear interrupt flag
    g_ui32Freq++; //Increment frequency whenever the rising edge is detected.
    IntEnable(INT_GPIOB);
}
