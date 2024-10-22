/**
 * @file LCDDriver.cpp
 * @author Hisayuki Nomura
 * @brief ST7032をコントローラーに使用した液晶用のライブラリのサンプルプログラム\n 
 * @version 0.1
 * @date 2024-10-19
 * 
 * @copyright Copyright (c) 2024 \n
 * このプログラムの使用、再配布などは自由です。利用については個人の責任で使用してください。
 * 
 * 
 * @details i2clcd.cppのライブラリで定義されている処理を行うサンプルプログラムです。\n
 * ライブラリにある、表示関連の関数のうち、代表的なものを実行します。
 * 何が実行されるかは、ソースコードを参照してください。\n
 * 
 * このサンプルプログラムを動かすには、Raspberry Pi PicoのGPIO8(pin#11)にSDA、GPIO9(pin#12)にSDLを接続してください。
 * また、LCDの１番ピンはVDDに接続してください。
 * 
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "i2cLCD.h"
#include <time.h>


int main()
{
    // Raspberry pi PICOのSDKで自動的に作成される、標準入出力の初期化処理
    stdio_init_all();

    // I2Cの初期化処理。使用するポートとボーレートを規定
    i2c_init(I2C_PORT, I2C_SPEED);
    
    // I2Cの初期化処理。ポートの８と９を、SDAとSCLに使用することを設定
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // I2Cの初期化処理。ポートの８と９をプルアップする
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);



    while (true) {
        // 液晶ライブラリの初期化処理
        lcd_init();
        int iRet;
        // カーソルを表示させる。ライブラリの戻り値は、送信バイト数になる。負の値が返った時はエラーになる。
        // この関数ではエラーのチェックを行っているが、これ以降は省略する。
        iRet = lcd_CursorDisplay(true);
        assert (iRet>=0);

        // 基本的な画面表示
        lcd_string("Hello, World!");

        // 表示位置の変更と、フォーマット付きの画面表示
        lcd_CursorPosition(1,0);                // カーソルを２行目の先頭に移動させて
        clock_t cl = clock();
        lcd_printf("Clock:%ld",(unsigned long)cl);  // フォーマット付きで文字を表示する

        // カーソル形状の変更、表示と相対移動
        lcd_CursorMode(true,true,false);        // カーソルを下線のみに
        lcd_CursorPosition(1,0);
        lcd_CursorDisplay(true);               // カーソルを表示
        // カーソル位置を左から右に移動させる
        for (int i=0;i<MAX_CHARS-1;i++) {
            lcd_MoveCursor(1);
            sleep_ms(200);
        }
        lcd_CursorMode(true,false,true);        // カーソルを点滅に
        // カーソル位置を右から左に移動させる
        for (int i=0;i<MAX_CHARS-1;i++) {
            lcd_MoveCursor(-1);
            sleep_ms(200);
        }    
    
        // 画面の消去（省電力モード）と復帰を行い、画面を点滅させる
        for (int i=0;i < 5;i++) {
            lcd_Sleep(true);
            sleep_ms(500);
            lcd_Sleep(false); 
            sleep_ms(500);
        }
        #if LCD_ICONEXIST
        // アイコンの操作（Strawberry Linux液晶のみ)を行う。アンテナアイコンを点滅させる
        for (int i=0;i < 10;i++) {
            lcd_IconSet(true,LCD_ICON::ANTENA);
            sleep_ms(1000);
            lcd_IconSet(false,LCD_ICON::ANTENA);
            sleep_ms(1000);
        }
        #endif


        // スクリーンに表示されている内容のシフト操作
        lcd_ClearDisplay();
        lcd_CursorPosition(0,0);        
        lcd_string("Hello, ");
        lcd_DisplayShift(-1);
        lcd_string ("P");
        lcd_DisplayShift(-1);
        lcd_string ("i");
        lcd_DisplayShift(-1);
        lcd_string ("c");
        lcd_DisplayShift(-1);
        lcd_string ("o");        
        
        sleep_ms(1000);
    }
}
