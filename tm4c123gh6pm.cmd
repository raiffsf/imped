/******************************************************************************
 *
 * Default Linker Command file for the Texas Instruments TM4C123GH6PM
 *
 * This is derived from revision 15071 of the TivaWare Library.
 *
 *****************************************************************************/

--retain=g_pfnVectors

MEMORY
{
    FLASH (RX) : origin = 0x00000000, length = 0x00040000
    SRAM (RWX) : origin = 0x20000000, length = 0x00008000
}

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M4_T_le_eabi.lib                                           */

/* Section allocation in memory */

SECTIONS
{
    .intvecs:   > 0x00000000
    .text   :   > FLASH
    .const  :   > FLASH
    .cinit  :   > FLASH
    .pinit  :   > FLASH
    .init_array : > FLASH
	i.ADCIntClear	:	>	FLASH
	i.ADCSequenceConfigure	:	>	FLASH
	i.ADCSequenceEnable	:	>	FLASH
	i.ADCSequenceStepConfigure	:	>	FLASH
	i.FPULazyStackingEnable	:	>	FLASH
	i.GPIODirModeSet	:	>	FLASH
	i.GPIOPadConfigSet	:	>	FLASH
	i.GPIOPinConfigure	:	>	FLASH
	i.GPIOPinTypeADC	:	>	FLASH
	i.GPIOPinTypeGPIOOutput	:	>	FLASH
	i.GPIOPinTypeUART	:	>	FLASH
	i.GPIOPinWrite	:	>	FLASH
	.constdata	:	>	FLASH
	i.IntDisable	:	>	FLASH
	i.IntEnable	:	>	FLASH
	i.IntMasterEnable	:	>	FLASH
	i.IntRegister	:	>	FLASH
	i.IntUnregister	:	>	FLASH
	i._IntDefaultHandler	:	>	FLASH
	vtable	:	>	FLASH
	.emb_text	:	>	FLASH
	i.SysCtlClockGet	:	>	FLASH
	i.SysCtlClockSet	:	>	FLASH
	i.SysCtlPeripheralEnable	:	>	FLASH
	i.TimerConfigure	:	>	FLASH
	i.TimerControlTrigger	:	>	FLASH
	i.TimerDisable	:	>	FLASH
	i.TimerEnable	:	>	FLASH
	i.TimerIntEnable	:	>	FLASH
	i.TimerLoadSet	:	>	FLASH
	i.TimerSynchronize	:	>	FLASH
	i.UARTCharPut	:	>	FLASH
	i.UARTClockSourceSet	:	>	FLASH
	i.UARTConfigSetExpClk	:	>	FLASH
	i.UARTIntClear	:	>	FLASH
	i.UARTIntEnable	:	>	FLASH
	i.UARTIntStatus	:	>	FLASH
	i.uDMAChannelControlSet	:	>	FLASH
	i.uDMAChannelEnable	:	>	FLASH
	i.uDMAChannelSizeGet	:	>	FLASH
	i.uDMAChannelTransferSet	:	>	FLASH
	i.uDMAChannelBaseSet	:	>	FLASH
	i.uDMAControlBaseSet	:	>	FLASH
	i.uDMAEnable	:	>	FLASH

    .vtable :   > 0x20000000
    .data   :   > SRAM
    .bss    :   > SRAM
    .sysmem :   > SRAM
    .stack  :   > SRAM
}

__STACK_TOP = __stack + 512;
