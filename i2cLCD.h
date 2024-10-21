/**
 * @file i2cLCD.h
 * @author Hisayuki Nomura
 * @brief Strawberry Linux製、I2C低電圧キャラクタ液晶モジュール（１６ｘ２行） （SB1602B）を、Raspberry PI picoのC/C++ SDKから使用するためのライブラのヘッダファイルです。\n 
 * メインプログラムにincludeしてください。このヘッダファイルの中の一部は、環境や使用方法により変更が必要になります。
 * @version 0.1
 * @date 2024-10-19
 * 
 * @copyright Copyright (c) 2024 \n
 * このプログラムの使用、再配布などは自由です。利用については個人の責任で使用してください。
 * 
 * @details  
 */

#ifndef __l2clcd_h__
#define __l2clcd_h__

// Strawberry Linuxのi2c液晶　（１６x２行、SB1602B)

// I2c定義
// I2C0 SDAを、GPIO8とGPIO9、400KHzで使用する。
// この設定は、PicoとLCDの結線により変更が必要


/**
 * @brief 使用するI2Cの番号。raspberry pi picoでは、i2c0 とi2c0の２系統のハードウェアを持っているので、そのどちらかを指定する。
 * @details i2c0とi2c1では、使用できるポートが異なるが、それ以外の機能に違いはない。
 * 液晶を接続するポート番号（I2C_SDAマクロとI2C_SCLマクロで定義)により、適切な番号を定義する。\n
 * i2c0 : GP0,1、GP4,5、GP8,9、GP12,13、GP16,17、GP20,21\n
 * i2c1 : GP2,3、GP6,7、GP10,11、GP14,15、GP18,19、GP26,27\n
 */
#define I2C_PORT i2c0               // 使用するI2Cの番号
/**
 * @brief SDAポートとして使用するポート番号。ピン番号ではない。例えば8の場合、GP8を意味するので１１番ピンとなる。
 */
#define I2C_SDA 8                   // i2Cに使用するポート番号
/**
 * @brief SCLポートとして使用するポート番号。ピン番号ではない。例えば9の場合、GP9を意味するので１２番ピンとなる。
 */
#define I2C_SCL 9                   // i2Cに使用するポート番号
/**
 * @brief I2Cのボーレート(HZ)。100*1000の場合は、100KHzとなる。 
 */
#define I2C_SPEED (100 * 1000)      // i2Cの速度

/**
 * @brief 一般コマンドの実行後の短い待ち時間。
 * @details 液晶コントローラでは、各命令ごとに待ち時間が決められている。
 * READ可能であれば、BUSYフラグを見て処理が完了してから次の命令を出せるが、この液晶はREADできない。そのため、
 * データシートに合わせて命令を実行後待ち時間を挿入する。\n
 * 待ち時間はコマンドごとに異なるが、オシレータが380KHzの場合一部の命令（Clear Display やReturn Home ）で1.08ms、それ以外は26.3μSになっている。\n 
 * このマクロは、ほとんどの命令での待ち時間として約30μ秒を定義している。\n
 * オシレータクロックを変更したときなどは、この値を調整する。
 */
#define CMD_DELAY       30 
/**
 * @brief 一部のコマンドの実行後の長い待ち時間
 * @details 液晶コントローラでは、各命令ごとに待ち時間が決められている。
 * READ可能であれば、BUSYフラグを見て処理が完了してから次の命令を出せるが、この液晶はREADできない。そのため、
 * データシートに合わせて命令を実行後待ち時間を挿入する。\n
 * 待ち時間はコマンドごとに異なるが、オシレータが380KHzの場合一部の命令（Clear Display やReturn Home ）で1.08ms、それ以外は26.3μSになっている。\n 
 * このマクロは、ClearDisplayなどでの待ち時間として約1msを定義している。\n* 
 * オシレータクロックを変更したときなどは、この値を調整する。
 */
#define CMD_DELAY_LONG  1000

/// @brief Strawberry Linux液晶のスレーブi2cアドレス。固定の0b0111110。
/// 使用するモジュールにより変更が必要かもしれない。（秋月の液晶も同じ）で
static int iI2CAddress = 0b0111110;        // アドレス 0b01111110


/**
 * @brief アイコンを表示/消去するときに、lcd_IconSetの引数として指定する値。
 * 例えば lcd_IconSet(true,LCD_ICON::BATTERY); などとすることで、バッテリーのアイコンを表示できる。
 */
enum LCD_ICON : uint16_t {
    ANTENA = 0b0000000000010000,
    PHONE  = 0b0000001000010000,
    SOUND = 0b0000010000010000,
    INPUT = 0b0000011000010000,
    UP = 0b0000011100010000,
    DOWN = 0b0000011100001000,
    LOCK = 0b0000100100010000,
    SILENT = 0b0000101100010000,
    BAT1 = 0b0000110100010000,
    BAT2 = 0b0000110100001000,
    BAT3 = 0b0000110100000100,
    BATTERY = 0b0000110100000010,
    S76 = 0b0000111100010000    
};



//プロトタイプ
/*低レベル関数。外部から呼ばれることが無いので、プロトタイプ宣言はコメントアウトしておく*/
/*
int i2c_write_byte(uint8_t val);            // i2cで１バイトのコマンド送信
int i2c_write_Data(unsigned char *buf);     // i2cでバッファのデータを連続送信
int i2c_write_DataByte(uint8_t val);
int lcd_IconSetRAW(bool isDisp , uint8_t addr , uint8_t bits); 
int lcd_send_byte(uint8_t val);
int lcd_FunctionSet(uint8_t mode);
*/



int lcd_ClearDisplay(void);
int lcd_ReturnHome(void);
int lcd_EntryModeSet(bool isDisplayToLeft);
int lcd_ContrastPowerIconSet(int contrast , bool is_PowerIconCtrl_IconOn , bool is_PowerIconCtrl_Boost);
int lcd_contrastSet(int contrast);
int lcd_FunctionSet(bool is8Bit , bool is2Line ,bool isExtInstruction);
int lcd_NormalMode();
int lcd_ExtendMode();
int lcd_InternalOSCSet(bool  isBs, uint8_t OSCFreq);
int lcd_FollowerControlSet(bool isOnOff,uint8_t ampRatio);
int lcd_CursorPosition(int line, int position) ;
int lcd_CursorMode(bool isDisplayOn , bool isUnderLine , bool isBlink);
int lcd_CursorDisplay(bool);
int lcd_IconSet(bool isDisp , LCD_ICON icon);
int lcd_string(const char *s);
void lcd_printf(const char *format, ...);

int lcd_DisplayShift(int8_t ShiftCnt);
int lcd_MoveCursor(int8_t MoveCnt);
int lcd_Sleep(bool isSleep);

/*初期化関連関数*/
int lcd_IconSetAll(bool isDisplay);
void lcd_init();                    // 初期化

#endif
