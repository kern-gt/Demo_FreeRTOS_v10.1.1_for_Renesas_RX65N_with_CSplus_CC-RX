/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名： main.c Ver1.00
＊　内容　　　： FreeRTOSv10.0.1デモプロジェクト（RenesasRX65N用）
＊　作成日　　： 2018/3/11
＊　作成者　　： kern-gt

	文字エンコード：UTF-8

	FreeRTOS:v10.0.1　(RX600 RXv2)
	開発環境：CS+forCC V6.01.00
	コンパイラ：CC-RX V2.08.00
	CPUボード：TARGET BOARD for RX65N(RTK5RX65N0C00000BR)
	CPU：R5F565NEDDFP

	デモ内容：
	CPUボード上のLED0を1Hz、LED1を5Hzで点滅させる2つのタスクを動かします。

	クロック発生回路とポート初期化をスマートコンフィグレータで設定しています。
	FreeRTOSではカーネルタイマにCMT0,システムコールにソフトウェア割込み(SWINT)を使用している
	ので、その機能は使用しないでください。
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
/**----------------------------------------------------------------------------
<<利用ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
/* Renesas includes. */
/*#include "r_cg_macrodriver.h"*/
/*#include "r_cg_userdefine.h"*/
#include "iodefine.h"

/* Kernel includes. */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"

/* Prototype */
void main(void);
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vApplicationSetupTimerInterrupt(void);


void main(void)
{
	/*タスク生成*/
	xTaskCreate(vTask1,"Task1",100,NULL,1,NULL);
	xTaskCreate(vTask2,"Task2",100,NULL,1,NULL);
	
	/* タスクスケジューラ起動*/
	vTaskStartScheduler();

	/*ここには戻ってこないはず*/
	while(1);
}


/***タスク定義*******************************************************************/
void vTask1(void *pvParameters)
{
	while(1) {
		/*CPUボード上のLED0を1Hzで点滅*/
		PORTD.PODR.BIT.B6 = ~PORTD.PODR.BIT.B6;
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}

void vTask2(void *pvParameters)
{
	while(1) {
		/*CPUボード上のLED1を5Hzで点滅*/
		PORTD.PODR.BIT.B7 = ~PORTD.PODR.BIT.B7;
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}

/***カーネルタイマ初期設定*******************************************************************/
/*
	カーネルタイマ（OSタイマ）にはCMT0を使用している。
	スケジューラ起動時にこのコールバック関数が呼ばれタイマの周期設定後、タイマを起動する。
	また、FreeRTOS/portable/port.cにOStickハンドラとしてCMT0の割込みハンドラを定義しているので、
	コード生成ツールを使用する場合はCMT0を使用しないようにする。
	以下の関数はデモ用のサンプルを参考に一部修正している。
		　デフォルトのtick周波数は1000Hzでこのまま使うのであれば問題ないが、100Hzで使いたい場合は114Hz以下の
		設定ができないのでその際はCMT0のクロックソースをPCLKB/8からPCLKB/32に変更するようにしている。ただし、
		30Hz未満にはできないので注意すること。
	
	 The RX port uses this callback function to configure its tick interrupt.
	This allows the application to choose the tick interrupt source. 
*/
void vApplicationSetupTimerInterrupt( void )
{
	const uint32_t ulEnableRegisterWrite = 0xA50BUL, ulDisableRegisterWrite = 0xA500UL;

	/* Disable register write protection. */
	SYSTEM.PRCR.WORD = ulEnableRegisterWrite;

	/* Enable compare match timer 0. */
	MSTP( CMT0 ) = 0;

	/* Interrupt on compare match. */
	CMT0.CMCR.BIT.CMIE = 1;

	/* Set the compare match value. */
	if(configTICK_RATE_HZ > 114){
		/* Divide the PCLK by 8. */
		CMT0.CMCR.BIT.CKS = 0;
		CMT0.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ ) -1 ) / 8 );
	}else  if(configTICK_RATE_HZ > 29){
		/* Divide the PCLK by 32. */
		CMT0.CMCR.BIT.CKS = 1;
		CMT0.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ ) -1 ) / 32 );
	}else{
		/*configTICK_RATE_HZ is very small.*/
		while(1);
	}

	/* Enable the interrupt... */
	_IEN( _CMT0_CMI0 ) = 1;

	/* ...and set its priority to the application defined kernel priority. */
	_IPR( _CMT0_CMI0 ) = configKERNEL_INTERRUPT_PRIORITY;

	/* Start the timer. */
	CMT.CMSTR0.BIT.STR0 = 1;

    /* Reneable register protection. */
    SYSTEM.PRCR.WORD = ulDisableRegisterWrite;
}
/*-----------------------------------------------------------*/
