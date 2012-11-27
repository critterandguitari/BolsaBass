/*
 * GENERATED FILE - DO NOT EDIT
 * (C) Code Red Technologies Ltd, 2008-2012
 * Generated linker script file for STM32F407VE
 * Created from generic_c.ld (vRed Suite v4.3 (0 [Build 1025] [2012-09-19] ))
 * By Red Suite v4.3.0 [Build 1025] [2012-09-19]  on Tue Nov 27 16:32:36 EST 2012
 */


INCLUDE "STM32F4_test_Debug_lib.ld"
INCLUDE "STM32F4_test_Debug_mem.ld"

ENTRY(ResetISR)

SECTIONS
{
	
	.text_Flash2 : ALIGN(4)
	{
	   FILL(0xff)
		*(.text_Flash2*)
		*(.text_MFlash1024*)
	} > MFlash1024

	/* MAIN TEXT SECTION */	
	.text : ALIGN(4)
	{
		FILL(0xff)
		KEEP(*(.isr_vector))
		
		/* Global Section Table */
		. = ALIGN(4) ;
		__section_table_start = .;
		__data_section_table = .;
		LONG(LOADADDR(.data));
		LONG(    ADDR(.data)) ;
		LONG(  SIZEOF(.data));
		LONG(LOADADDR(.data_RAM2));
		LONG(    ADDR(.data_RAM2)) ;
		LONG(  SIZEOF(.data_RAM2));
		LONG(LOADADDR(.data_RAM3));
		LONG(    ADDR(.data_RAM3)) ;
		LONG(  SIZEOF(.data_RAM3));
		LONG(LOADADDR(.data_RAM4));
		LONG(    ADDR(.data_RAM4)) ;
		LONG(  SIZEOF(.data_RAM4));
		__data_section_table_end = .;
		__bss_section_table = .;
		LONG(    ADDR(.bss));
		LONG(  SIZEOF(.bss));
		LONG(    ADDR(.bss_RAM2));
		LONG(  SIZEOF(.bss_RAM2));
		LONG(    ADDR(.bss_RAM3));
		LONG(  SIZEOF(.bss_RAM3));
		LONG(    ADDR(.bss_RAM4));
		LONG(  SIZEOF(.bss_RAM4));
		__bss_section_table_end = .;
		__section_table_end = . ;
		/* End of Global Section Table */
		

		*(.after_vectors*)
		
		*(.text*)
		*(.rodata .rodata.*)
		. = ALIGN(4);
		
	} > MFlash512

	/*
	 * for exception handling/unwind - some Newlib functions (in common
	 * with C++ and STDC++) use this. 
	 * Use KEEP so not discarded with --gc-sections
	 */
	.ARM.extab : ALIGN(4)
	{
		KEEP(*(.ARM.extab* .gnu.linkonce.armextab.*))
	} > MFlash512
	__exidx_start = .;
	
	.ARM.exidx : ALIGN(4)
	{
		KEEP(*(.ARM.exidx* .gnu.linkonce.armexidx.*))
	} > MFlash512
	__exidx_end = .;
	
	_etext = .;
		
	
	.data_RAM2 : ALIGN(4)
	{
	   FILL(0xff)
		*(.data.$RAM2*)
		*(.data.$PeriphRam16*)
	   . = ALIGN(4) ;
	} > PeriphRam16 AT>MFlash512
	
	.data_RAM3 : ALIGN(4)
	{
	   FILL(0xff)
		*(.data.$RAM3*)
		*(.data.$Ram64*)
	   . = ALIGN(4) ;
	} > Ram64 AT>MFlash512
	
	.data_RAM4 : ALIGN(4)
	{
	   FILL(0xff)
		*(.data.$RAM4*)
		*(.data.$Ram128*)
	   . = ALIGN(4) ;
	} > Ram128 AT>MFlash512
	
	/* MAIN DATA SECTION */

	.uninit_RESERVED : ALIGN(4)
	{
		KEEP(*(.bss.$RESERVED*))
		. = ALIGN(4) ;
		_end_uninit_RESERVED = .;
	} > Ram112

	.data : ALIGN(4)
	{
		FILL(0xff)
		_data = .;
		*(vtable)
		*(.data*)
		. = ALIGN(4) ;
		_edata = .;
	} > Ram112 AT>MFlash512

	
	.bss_RAM2 : ALIGN(4)
	{
		*(.bss.$RAM2*)
		*(.bss.$PeriphRam16*)
	   . = ALIGN(4) ;
	} > PeriphRam16
	
	.bss_RAM3 : ALIGN(4)
	{
		*(.bss.$RAM3*)
		*(.bss.$Ram64*)
	   . = ALIGN(4) ;
	} > Ram64
	
	.bss_RAM4 : ALIGN(4)
	{
		*(.bss.$RAM4*)
		*(.bss.$Ram128*)
	   . = ALIGN(4) ;
	} > Ram128

	/* MAIN BSS SECTION */
	.bss : ALIGN(4)
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4) ;
		_ebss = .;
		PROVIDE(end = .);
	} > Ram112
	
	PROVIDE(_pvHeapStart = .);
	PROVIDE(_vStackTop = __top_Ram112 - 0);
}
