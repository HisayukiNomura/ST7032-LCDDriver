/**
 * @file i2cLCD.cpp
 * @author Hisayuki Nomura
 * @brief ST7032をコントローラーに使用した液晶を、Raspberry PI picoのC/C++ SDKから使用するためのライブラリです。\n 
 * 
 *  @version 0.1
 * @date 2024-10-19
 * 
 * @copyright Copyright (c) 2024 \n
 * このプログラムの使用、再配布などは自由です。利用については個人の責任で使用してください。
 * 
 * @details このファイルは、LCDドライバの本体（すべて）です。このライブラリを使用するには、Raspberry PI PicoのSDK　が必要です。
 * 同じPicoでも、Ardiunoのフレームワークを使用するパターンや、Pythonでのコントロールはほかに資料がたくさんあるのでそちらを参照してください。\n
 * ソースコードの拡張子はcppですが、C++の機能は使用していないのでCに変更してコンパイルできます。\n
 *
 * テスト済みの製品は次の２製品です。
 * [Strawberry Linux SB1602B](https://strawberry-linux.com/catalog/items?code=27001)\n
 * - [アプリケーションノート](https://strawberry-linux.com/pub/i2c_lcd-an001.pdf)\n
 * [秋月電子Raspberry Pi キャラクター液晶ディスプレイモジュールキット(AE-AQM0802+PCA9515)](https://akizukidenshi.com/catalog/g/g111354/)] \n
 * - [マニュアル](https://akizukidenshi.com/goodsaffix/ae-aqm0802a.pdf)
 * - [データシート](https://akizukidenshi.com/goodsaffix/AQM0802.pdf)
 * 
 * [ST7032のデーターシート](https://strawberry-linux.com/pub/ST7032i.pdf)\n
 * 
 * ST7032を使用しているほかの製品、秋月の[AQM1602Y-RN-GBW](https://akizukidenshi.com/catalog/g/g111916/)等もそのままか小修整で使用できるかもしれません。\n 
 *
 *
 * 
 * このライブラリを使用する場合、直接I2Cコマンドをデバイスに送信することは行いません。各機能は、ライブラリが受け取り、内部で現在の状態を
 * 保存してからLCDコントローラーにi2cを送信します。
 * そのため、LCDを、ライブラリを使わずに操作してしまうと、ライブラリの内部状態とLCDの状態がズレてしまい、正常に動作しなくなります。
 * ライブラリを使うときは、LCDのコントロールはすべて、このライブラリから実行してください。\n

 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "i2cLCD.h"
#include "i2cLCDlocal.h"


#undef  i2c_default
#define i2c_default I2C_PORT


/**
 * @brief I2Cで１バイトのコマンドを送信する低レベルの関数。\n
 * 通常はこの関数は直接使用せず、lcd_send_byte(uint8_t val)関数を使用する。
 * 
 * @param val 送信するコマンド。LCD_COMMAND(0x00)に続いて送信される。
 * @param nostop ストップビットを送らない。通常はfalseでよい。省略するとfalseとなる。
 * @return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_COMMAND＋valで２バイト）
 * 
 * @attention
 * この関数が呼ばれる前に、通常モード（LCD_FUNCTIONSETで、LCD_FUNC_INSTTBL_SELECTのビットをクリア）しておく必要がある。
 * このライブラリでは、LCD_FUNC_INSTTBL_SELECTについては必要な時だけ１にする（デフォルトでオフ）になるようにしている。
 * @details
 * この関数は、内部的に使用される関数なので直接呼び出すのは推奨しない。\n 
 * 本ライブラリでは、設定した値を変数に保存している。（Strawberry Linuxの液晶がWrite Onlyで設定値を読みだせないので）　
 * 本関数のような低レベル関数を使用すると、この仕組みが上手く動作しなくなり、液晶の設定値とソフトウェアで保存した設定が
 * マッチしなくなり、誤動作する可能性がある。
 * 
 */

static int i2c_write_byte(uint8_t val,bool nostop) 
{
    uint8_t t_data[2];
    t_data[0]=LCD_COMMAND;
    t_data[1]=val;
    volatile int iRet;
    iRet = i2c_write_blocking(I2C_PORT, I2C_ADDRESS, t_data, 2, nostop);

    sleep_us(CMD_DELAY);
    return iRet;
}
/**
 * @brief I2Cで１バイトのコマンドを送信する低レベルの関数。\n
 * 通常はこの関数は直接使用せず、lcd_send_byte(uint8_t val)関数を使用する。
 * 
 * @param val 送信するコマンド。LCD_COMMAND(0x00)に続いて送信される。
 * @return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_COMMAND＋valで２バイト）
 * 
 * @attention
 * この関数が呼ばれる前に、通常モード（LCD_FUNCTIONSETで、LCD_FUNC_INSTTBL_SELECTのビットをクリア）しておく必要がある。
 * このライブラリでは、LCD_FUNC_INSTTBL_SELECTについては必要な時だけ１にする（デフォルトでオフ）になるようにしている。
 * @details
 * この関数は、内部的に使用される関数なので直接呼び出すのは推奨しない。\n 
 * 本ライブラリでは、設定した値を変数に保存している。（Strawberry Linuxの液晶がWrite Onlyで設定値を読みだせないので）　
 * 本関数のような低レベル関数を使用すると、この仕組みが上手く動作しなくなり、液晶の設定値とソフトウェアで保存した設定が
 * マッチしなくなり、誤動作する可能性がある。
 * 
 */

static int i2c_write_byte(uint8_t val) 
{
    return i2c_write_byte(val,false);
}
/**
 * @brief I2Cで１バイトのデータを送信する低レベルの関数。\n
 * 通常はこの関数は直接使用せず、lcd_stringなどの上位関数を使用する。
 * @attention
 * この関数が呼ばれる前に、通常モード（LCD_FUNCTIONSETで、LCD_FUNC_INSTTBL_SELECTのビットをクリア）しておく必要がある。
 * このライブラリでは、LCD_FUNC_INSTTBL_SELECTについては必要な時だけ１にする（デフォルトでオフ）になるようにしている。
 * @param cmd コマンド。コマンドの場合は、LCD_COMMAND、文字列データのときはLCD_CHARACTER。
 * @param val 送信するデータ。LCD_CHARACTER(0x40)に続いて送信される
 * @return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_CHARACTER＋valで２バイト）
 * @details
 * この関数は、内部的に使用される関数なので直接呼び出すのは推奨しない。\n 
 * 本ライブラリでは、設定した値を変数に保存している。（Strawberry Linuxの液晶がWrite Onlyで設定値を読みだせないので）　
 * 本関数のような低レベル関数を使用すると、この仕組みが上手く動作しなくなり、液晶の設定値とソフトウェアで保存した設定が
 * マッチしなくなり、誤動作する可能性がある。
 */
static int i2c_write_DataByte(uint8_t cmd, uint8_t val)
{
    uint8_t t_data[2];
    t_data[0]=cmd;
    t_data[1]=val;
    volatile int iRet;
    iRet = i2c_write_blocking(I2C_PORT, I2C_ADDRESS, t_data, 2, false);

    sleep_us(CMD_DELAY);
    return iRet;
}

/**
 * @brief I2Cで１バイトのデータを送信する低レベルの関数。\n
 * 通常はこの関数は直接使用せず、lcd_stringなどの上位関数を使用する。
 * @attention
 * この関数が呼ばれる前に、通常モード（LCD_FUNCTIONSETで、LCD_FUNC_INSTTBL_SELECTのビットをクリア）しておく必要がある。
 * このライブラリでは、LCD_FUNC_INSTTBL_SELECTについては必要な時だけ１にする（デフォルトでオフ）になるようにしている。
 * @param val 送信するデータ。LCD_CHARACTER(0x40)に続いて送信される
 * @return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_CHARACTER＋valで２バイト）
 * @details
 * この関数は、内部的に使用される関数なので直接呼び出すのは推奨しない。\n 
 * 本ライブラリでは、設定した値を変数に保存している。（Strawberry Linuxの液晶がWrite Onlyで設定値を読みだせないので）　
 * 本関数のような低レベル関数を使用すると、この仕組みが上手く動作しなくなり、液晶の設定値とソフトウェアで保存した設定が
 * マッチしなくなり、誤動作する可能性がある。
 */
static int i2c_write_DataByte(uint8_t val) 
{
    return i2c_write_DataByte(LCD_CHARACTER,val);
}
/**
 * @brief I2Cで、複数バイトのデータを送信する低レベルの関数。\n
 * 通常はこの関数は直接使用せず、lcd_stringなどの上位関数を使用する。
 * 
 * @attention
 * この関数が呼ばれる前に、通常モード（LCD_FUNCTIONSETで、LCD_FUNC_INSTTBL_SELECTのビットをクリア）しておく必要がある。
 * このライブラリでは、LCD_FUNC_INSTTBL_SELECTについては必要な時だけ１にする（デフォルトでオフ）になるようにしている。
 
 * @param buf 送信するデータ。LCD_CHARACTER(0x40)＋buf[n]が連続して送信される。
 * @param len 送信するデータの長さ。負の値が指定された場合、長さは文字列の長さ（NULL文字まで）
 * @return int 送信したバイト数。負の値の場合はエラー。
 * @details
 * この関数は、内部的に使用される関数なので直接呼び出すのは推奨しない。\n 
 * 本ライブラリでは、設定した値を変数に保存している。（Strawberry Linuxの液晶がWrite Onlyで設定値を読みだせないので）　
 * 本関数のような低レベル関数を使用すると、この仕組みが上手く動作しなくなり、液晶の設定値とソフトウェアで保存した設定が
 * マッチしなくなり、誤動作する可能性がある。
 */
static int i2c_write_Data(unsigned char *buf, int length) 
{
    int iByteSent = 0;
    bool bRet;
    volatile uint8_t i;
    uint8_t t_data[MAX_CHARS* MAX_LINES+1];
    size_t len;
    if (length < 0) {
        len = strlen((const char *)buf);
    } else {
        len = length;
    }
    for (i = 0 ; i < len;i++) {
        t_data[0] = LCD_CHARACTER;
        t_data[i+1] = (uint8_t)buf[i];
    }
    volatile int iRet;
    iRet = i2c_write_blocking(I2C_PORT , I2C_ADDRESS , t_data, len+1, false);
    iByteSent += iRet;
    return iByteSent;
}
/**
 * @brief 文字列の終端までを送信する低レベルの関数。\n
 * 通常はこの関数は直接使用せず、lcd_stringなどの上位関数を使用する。
 * 
 * @param buf 文字列
 * @return int 送信したバイト数。負の値の場合はエラー。
 */
static int i2c_write_Data(unsigned char *buf) 
{
    return i2c_write_Data(buf,-1);
}

/**
 * @brief LCDにコマンドを送信する低レベルの関数。\n
 * 通常はこの関数は直接使用せず、コマンドに相当する関数(lcd_ClearDisplay、lcd_FunctionSet等)を呼び出す。
 * 
 * @param val 送信するデータ。LCD_COMMANDに続いて送信される。
 * @return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_CHARACTER＋valで２バイト）
 */
static int lcd_send_byte(uint8_t val) 
{
    int iRet = i2c_write_byte(val);
    return iRet;
}  



/**
 * @brief 液晶に表示されているテキストを消去する
 * return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_CHARACTER＋valで２バイト）
 * @details 液晶の表示メモリ（Dispray Data RAM…DDRAM)を0x20で埋め、カーソル位置を画面左上（Address Control…AC を０）に設定する。
 */
int lcd_ClearDisplay(void) 
{
    //int iRet = lcd_send_byte(LCD_CLEARDISPLAY);
    int iRet = lcd_send_byte(LCDCommands.CLEARDISPLAY);
    sleep_us(CMD_DELAY_LONG);
    return iRet;
}

/**
 * @brief DDRAMアドレスを00Hに設定しカーソルをもとに戻す。表示内容は変更されない。右⇒左モードの場合、カーソル位置は１行目の右端にセットされる
 * 
 * @return int 送信したバイト数。負の値の場合はエラー。
 */
int lcd_ReturnHome(void)
{
    int iRet = lcd_send_byte(LCDCommands.RETURNHOME);
    if (lcdSetting.isDisplayToLeft) {
        lcd_CursorPosition(0,MAX_CHARS-1);
    }
    sleep_us(CMD_DELAY_LONG);
    return iRet;    
}
/**
 * @brief ディスプレイを右から左に書くか、左から右に書くかを決定する
 * 
 * @param isDisplayToLeft trueにすると、右から左になる
 * @return int 送信したバイト数。負の値の場合はエラー。
  */
int lcd_EntryModeSet(bool a_isDisplayToLeft)
{
    uint8_t val = LCDCommands.ENTRYMODESET;
    if (a_isDisplayToLeft) {
        val |= (LCDCommands.EntryModeOpt.LEFT | LCDCommands.EntryModeOpt.SHIFTINCREMENT);
    } else {

    }
    int iRet = lcd_send_byte(val);
    lcdSetting.isDisplayToLeft = a_isDisplayToLeft;
    return iRet;

}

/**
 * @brief 液晶のカーソル位置を指定する。文字列を表示するときは、このカーソル位置から表示される。
 * 
 * @param line 表示する行　（0～１）
 * @param position 表示するカラム（０～１５）
 * @return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_CHARACTER＋valで２バイト）
 * @details ST7032では、文字を表示するためのメモリは、１行目が0x00から、２行目は0x40から始まる。
 * このプログラムは少し冗長だが、可読性を優先した。
 */
int lcd_CursorPosition(int line, int position) 
{
    int val = LCDCommands.SETDDRAMADDR;
    if (line == 0) {
        val |= (LCDCommands.DDRAMOpt.LCD_SETDDRAM_MASK & position);
    } else {
        val |= (0x40 | (LCDCommands.DDRAMOpt.LCD_SETDDRAM_MASK & position));
    }
    int iRet = lcd_send_byte(val);
    lcdSetting.curPosLine = line;
    lcdSetting.curPosColumn = position;

    return iRet;
}
/**
 * @brief FUNCTIONSETを送信する。引数には、ビットマスクを指定する。内部的にはこっちが便利なので残してあるが、個別に引数として指定するほうを推奨。
 * 
 * @param mode FunctionSetで指定する設定値。 
 * LCD_FUNC_8BITMODE , LCD_FUNC_2LINE , LCD_FUNC_DOUBLEHEIGHT,LCD_FUNC_INSTTBL_SELECTを | で組み合わせる。
 * @return int 送信したバイト数。負の値の場合はエラー。
 */
int lcd_FunctionSet(uint8_t mode)
{
    int iRet;
    lcdSetting.isFunc_ISMode = (mode & LCDCommands.FuncSetOpt.INSTRUCTIONTABLE) != 0;
    lcdSetting.isFunc_2LINE =  (mode & LCDCommands.FuncSetOpt.DOUBLELINE) != 0;
    lcdSetting.isFunc_DoubleHeight = (mode & LCDCommands.FuncSetOpt.DOUBLEHEIGHT) != 0;
    lcdSetting.isFunc_8Bit =  (mode & LCDCommands.FuncSetOpt.EIGHTBITMODE) != 0;
    iRet = lcd_send_byte(LCDCommands.FUNCTIONSET | mode);
    return iRet;
}
/**
 * @brief FUNCTIONSETを送信する。
 * 
 * @param is8Bit ８ビットモード。このライブラリでは常にtrueを指定する。falseにした場合は動作しない。
 * @param is2Line 16文字ｘ２行のモード。通常はtrue。ここをfalseにすると、１行モードになり文字の高さも２倍になる。
 * @param isExtInstruction 拡張インストラクション（ＩＳ）の指定。
 * @return int 
 * @details lcd_FunctionSet関数のラッパーとして動作し、lcd_FunctionSetで指定するほかのパラメータ、is8BitやisExtInstractionなどは変更されない。\n 
 * 使用用途が低いと思われる、１行で5x8ピクセル文字については、このライブラリでは指定する方法はない。普通に、２行モードのまま文字を１行目に表示するなどで対応するか、
 * int lcd_FunctionSet(uint8_t mode)を工夫して直接呼び出すなどの対応が必要になる。
 */
int lcd_FunctionSet(bool is8Bit , bool is2Line , bool isExtInstruction)
{
    uint8_t opt = 0;
    opt |= is8Bit ?  LCDCommands.FuncSetOpt.EIGHTBITMODE : 0;
    opt |= is2Line ? LCDCommands.FuncSetOpt.DOUBLELINE : LCDCommands.FuncSetOpt.DOUBLEHEIGHT; 
    opt |= isExtInstruction ? LCDCommands.FuncSetOpt.INSTRUCTIONTABLE : 0;
    return lcd_FunctionSet(opt);
}
/**
 * @brief ２行表示モードに変更する。
 * 
 * @param is2Line trueにすると、文字は5x8ピクセルの２行表示モードになる。falseを指定すると１行表示で5x16ピクセルになる。
 * @return int 
 * @details lcd_FunctionSet関数のラッパーとして動作し、lcd_FunctionSetで指定するほかのパラメータ、is8BitやisExtInstractionなどは変更されない。\n 
 * 使用用途が低いと思われる、１行で5x8ピクセル文字については、このライブラリでは指定する方法はない。普通に、２行モードのまま文字を１行目に表示するなどで対応するか、
 * int lcd_FunctionSet(uint8_t mode)を工夫して直接呼び出すなどの対応が必要になる。
 */
int lcd_2LineMode(bool is2Line) 
{
    return lcd_FunctionSet(lcdSetting.isFunc_8Bit,is2Line,lcdSetting.isFunc_ISMode);
}


/**
 * @brief 命令セットを標準モード（LCD_FUNCTIONSETの、LCD_FUNC_INSTTBL_SELECTを０）にする。\n
 *  lcd_FunctionSetで指定した、ほかの設定値は変更されない。
 * @return int 送信したバイト数。負の値の場合はエラー。
 * @details 本ライブラリを使う場合、自動的に通常モードに戻されるので、この関数を明示的に呼び出す必要はない。\n
 * 但し、何らかの理由で表示状態がずれてしまった場合などは、この関数を使って元に戻す
 */
int lcd_NormalMode()
{
    if (lcdSetting.isFunc_ISMode) { // 現在、拡張モードにある場合
        uint8_t val = 0;
        val |= lcdSetting.isFunc_2LINE ? LCDCommands.FuncSetOpt.DOUBLELINE:0;
        val |= lcdSetting.isFunc_DoubleHeight ? LCDCommands.FuncSetOpt.DOUBLEHEIGHT:0;
        val |= lcdSetting.isFunc_8Bit ? LCDCommands.FuncSetOpt.EIGHTBITMODE:0;
        int iRet;
        iRet = lcd_FunctionSet(val);
        return iRet;
    } else {
        return 0;
    }
}
/**
 * @brief 命令セットを拡張モード（LCD_FUNCTIONSETの、LCD_FUNC_INSTTBL_SELECTを１）にする。\n
 *  lcd_FunctionSetで指定した、ほかの設定値は変更されない。
 * @return int 送信したバイト数。負の値の場合はエラー。
 * @details 本ライブラリを使う場合、関数内で自動的に呼び出され、関数が終了すると標準モードに戻されるので、この関数を明示的に呼び出す必要はない。\n
 * 但し、何らかの理由で表示状態がずれてしまった場合などは、この関数を使って元に戻す
 */
int lcd_ExtendMode()
{
    
    if (lcdSetting.isFunc_ISMode) {
        return 0;
    } else {
        uint8_t val = 0;
        val |= LCDCommands.FuncSetOpt.INSTRUCTIONTABLE;
        val |= lcdSetting.isFunc_2LINE ? LCDCommands.FuncSetOpt.DOUBLELINE:0;
        val |= lcdSetting.isFunc_DoubleHeight ? LCDCommands.FuncSetOpt.DOUBLEHEIGHT:0;
        val |= lcdSetting.isFunc_8Bit ? LCDCommands.FuncSetOpt.EIGHTBITMODE:0;
        int iRet;
        iRet = lcd_FunctionSet(val);
        return iRet;
    }
}

/**
 * @brief 内部オシレータの周波数設定
 * 
 * @param isBs trueのときは 1/4 バイアス、falseの場合は 1/5バイアス
 * @param OSCFreq 内部オシレータ調整値
 * @return int 送信したバイト数。負の値の場合はエラー。
 */
int lcd_InternalOSCSet(bool  isBs, uint8_t OSCFreq)
{
    int iSendBytes;
    int iRet;
    uint8_t val = LCDCommands.IS1_INTOSC | (LCDCommands.IntOSCOpt.FREQMASK & OSCFreq);

    iRet = lcd_ExtendMode();
    iSendBytes += iRet;
    if (isBs) {
        val |= LCDCommands.IntOSCOpt.BIAS1BY4;
    }
    iRet = lcd_send_byte(val);               // 1/5 bias , 183 Hz
    iSendBytes += iRet;
    iRet = lcd_NormalMode();
    iSendBytes += iRet;
    lcdSetting.isBias1By4 = isBs;
    lcdSetting.OSCFreq = OSCFreq;
    return iSendBytes;
}



/**
 * @brief 液晶の表示と、カーソル表示の有無を設定する。
 * 
 * @param isDisplayOn 液晶全体の表示をオンにするかを指定する。trueの場合表示オン、falseで表示オフ。
 
 * @param isUnderLine カーソル位置に下線を表示する
 * @param isBlink カーソル位置の文字を点滅（■とカーソル位置文字を交互に表示）させる。
 * @return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_CHARACTER＋valで２バイト）
 * @details この関数で、isUnderline、isBlinkのどちらか（もしくは両方）をオンにして呼び出すと、
 * lcd_CursorDisplayの設定は変更されてカーソルは表示状態になる。
 * 表示させないようにするには、再度、lcd_CursorDisplay(false)を呼び出すか、isUnderline、isBlink両方をオフにして呼び出す必要がある。
 */
int lcd_CursorMode(bool isDisplayOn , bool isUnderLine , bool isBlink)
{
    bool iRet;
    uint8_t mode = 0;
    lcdSetting.isDisplayOn = isDisplayOn;
    lcdSetting.isUnderLine = isUnderLine;
    lcdSetting.isBlink = isBlink;
    mode |= (isDisplayOn ? LCDCommands.DisplayOnOffOpt.DISPLAY_ON : 0);
    mode |= (isUnderLine ? LCDCommands.DisplayOnOffOpt.CURSOR_ON : 0);
    mode |= (isBlink ? LCDCommands.DisplayOnOffOpt.CURBLINK_ON :  0);

    iRet = lcd_send_byte(LCDCommands.DISPLAYONOFF | mode);
    // もし、下線、点滅のどちらかがオンなら、初期状態としてカーソル表示はオンにする。
    if (isUnderLine || isBlink) {
        lcdSetting.isCursorDisplay = true;
    } else {
        lcdSetting.isCursorDisplay = false;
    }

    return iRet;
}
/**
 * @brief 液晶のカーソルを表示するかどうかを指定する。事前にlcd_CursorModeを使用して、
 * カーソルモードの下線、点滅のどちらかをオンにしておく必要がある。どちらもオフの場合、この関数を実行してもカーソルはオンにならない。
 * 
 * @param isDisp trueのときはカーソルを表示する、falseのときはカーソルを削除する。
 * @return int 送信したバイト数。-1の場合はエラー。2が正常（LCD_CHARACTER＋valで２バイト）。\n
 * カーソルモードが下線も点滅もオフの場合、何も行われないので０が返される。
 */
int lcd_CursorDisplay(bool isDisp)
{
    int iRet;
    // カーソルモードが下線オフ、点滅オフのときはカーソルの場合、この関数を呼び出しても
    // カーソルは表示されない。
    if ( lcdSetting.isUnderLine == false && lcdSetting.isBlink == false ) {
        return 0;
    }
    if (isDisp) {       // カーソル表示を行う場合、事前に保存してあったカーソルモードを送信する
        iRet = lcd_CursorMode(lcdSetting.isDisplayOn , lcdSetting.isUnderLine,lcdSetting.isBlink);
        lcdSetting.isCursorDisplay = true;
    } else {            // カーソル表示を消す場合、保存してあるカーソルモードはそのまま、カーソルを消す命令だけを送信する。
        iRet = lcd_send_byte(LCDCommands.DISPLAYONOFF | (lcdSetting.isDisplayOn ? LCDCommands.DisplayOnOffOpt.DISPLAY_ON:0));
        lcdSetting.isCursorDisplay = false;
    }
    return iRet;
}

/**
 * @brief 現在のカーソル位置に、指定された文字列(NULL終了）を表示する
 * 
 * @param s 表示する文字列
 * @return int 送信したバイト数。-1の場合はエラー。それ以外の正の値は正常。
 */
int lcd_string(const char *s) 
{
    int iRet = i2c_write_Data((unsigned char *)s);
    lcdSetting.curPosColumn += strlen(s);
    return iRet;
}
/**
 * @brief 現在のカーソル位置に、指定された長さのバッファを表示する。ヌル文字も出力できる。
 * 
 * @param s 表示する文字列
 * @param length 出力する長さ
 * @return int 送信したバイト数。-1の場合はエラー。それ以外の正の値は正常。
 */
int lcd_string(const char *s , int length) 
{
    int iRet = i2c_write_Data((unsigned char *)s,length);
    lcdSetting.curPosColumn += strlen(s);
    return iRet;
}

/**
 * @brief 現在のカーソル位置に、フォーマットされた文字列を表示する
 * 
 * @param format C標準のprintfフォーマット
 * @param ... 
 * @details 実行速度が遅いので、フォーマットが不要な時はlcd_stringを使用したほうが高速に動作する。\n 
 * 表示する文字列の長さが、画面の最大を超えた場合は、超えた部分の表示は捨てられる。
 */
void lcd_printf(const char *format, ...)
{
    char aryLCDBuf[MAX_LINES*MAX_CHARS+1];
    va_list va;
    va_start(va , format);
    vsnprintf(aryLCDBuf,sizeof(aryLCDBuf),format , va);
    lcdSetting.curPosColumn += strlen(aryLCDBuf);
    lcd_string(aryLCDBuf);
}


/**
 * @brief FollowerControlを送信する
 * 
 * @param isOnOff ボルテージフォロア回路のオン/オフを切り替える。
 * （コントローラの、OPF1,OPF2ともGNDに接続されているときのみ有効）
 * @param ampRatio V0 ジェネレータの増幅率調整値。LCDに供給される電圧の調整。例えば、４のときはVDDx1.5となる。
 * 液晶の明るさは増幅値x lcd_ContrastSetで設定されるVRefとなる。ST7032データシートの42Pに詳細あり。
 * @return int 送信したバイト数。負の値の場合はエラー。
 
 */
int lcd_FollowerControlSet(bool isOnOff,uint8_t ampRatio)
{
    int iSendBytes;
    int iRet;
    uint8_t val = LCDCommands.IS1_FOLLOWERCTRL | (LCDCommands.FollowerOpt.AMPRATIO_MASK & ampRatio) ;

    iRet = lcd_ExtendMode();
    iSendBytes += iRet;
    if (isOnOff) {
        val |= LCDCommands.FollowerOpt.ON;
    };
    iRet = lcd_send_byte(val);
    iSendBytes += iRet;

    iRet = lcd_NormalMode();    
    iSendBytes += iRet;
    lcdSetting.isFollowerOnOff = isOnOff;
    lcdSetting.followerAmpRatio = ampRatio;
    return iSendBytes;    
}


/**
 * @brief 液晶のコントラスト設定と、アイコン表示、電圧増幅の有無を設定する。実際のコントラストはlcd_FollowerControlSetで指定した増幅率と組み合わされて使用される。
 * @param contrast コントラスト値 0～0b00111111 (63)。この値からVRefが決定する。
 * VRef= 電源電圧 * ((コントラスト値 + 36) / 100)　になる。
 * 結果として、実際の液晶に供給される電圧は、V0 = 増幅率調整値(lcd_FollowerControlSetで設定）* VRefになる。
 * @param is_PowerIconCtrl_IconOn  LCD_TBL1_POWERICONCTRL_ONのビット。アイコン表示を行うかどうか。通常はtrue。
 * @param is_PowerIconCtrl_Boost 電源電圧の増幅を行う。通常はtrue。
 * @return int 送信したバイト数。負の値の場合はエラー。
 */
int lcd_ContrastPowerIconSet(int contrast , bool is_PowerIconCtrl_IconOn , bool is_PowerIconCtrl_Boost)
{
    lcd_ExtendMode();
    volatile int iRet;
    uint8_t otherBit = 0;
    otherBit |= is_PowerIconCtrl_IconOn ? LCDCommands.PowerIconOpt.ICON_ON : 0;
    otherBit |= is_PowerIconCtrl_Boost ?  LCDCommands.PowerIconOpt.POWERBOOST : 0;

    lcdSetting.uiContrast = contrast;
    lcdSetting.isPowerIconOn = is_PowerIconCtrl_IconOn;
    lcdSetting.isPowerBoost = is_PowerIconCtrl_Boost;

    // まず、LCD_TBL1_FOLLOWERCONTRASTで、下位４ビットを送信
    iRet = lcd_send_byte(LCDCommands.IS1_FOLLOWERCONTRAST | (LCDCommands.FollowerContrastOpt.CONTRASTLOWER_MASK & contrast) );
    if (iRet != 2) return iRet;
    // 次に、LCD_TBL1_POWERICONCTRLで、上位2ビットを送信
    iRet = lcd_send_byte(LCDCommands.IS1_POWERICONCTRL | otherBit | (LCDCommands.PowerIconOpt.CONTRASTUPPER_MASK & (contrast >> 4)) );
    if (iRet != 2) return iRet;
    lcd_NormalMode();
    return 4;
}
/**
 * @brief 液晶のコントラスト設定を行う。
 * @param contrast 
 * @return int 
 * @details 液晶コントローラーコマンドで、同時に設定されるアイコン表示制御と、電圧増幅設定については、以前設定した値がそのまま使用される。\n 
 * lcd_ContrastPowerIconSet関数のラッパーとして動作する。本当は、同じ名前にしてオーバーロードしたかったが、C言語の場合を考えて名前を変えた。\n 
 * (可変長引数と言うのも考えたがヤリスギのような気がしたので)
 */

int lcd_contrastSet(int contrast)
{
    return lcd_ContrastPowerIconSet(contrast , lcdSetting.isPowerIconOn ,lcdSetting.isPowerBoost);
}


// アイコンが接続されていない液晶の場合は不要
#if LCD_ICONEXIST
/**
 * @brief LCDのアイコンをオン/オフするための下位関数。通常はこの関数は直接使用せず、lcd_IconSetを使用する。\n
 *   この関数においては、表示するアイコンはアドレスとビットで指定する。アドレスは0～16、ビットは各5ビット、16x5=80種類のアイコンが制御できるが、
 *   Strawberry Linux液晶では使用しているのは13個。https://strawberry-linux.com/pub/i2c_lcd-an001.pdf　を参照。
 * @param isDisp 表示するか、消去するかのフラグ。trueのときはアイコンを表示
 * @param iconAddr アイコンのアドレス。00～0Fまでの範囲
 * @param bits アイコンのビット　各５ビットのそれぞれに、アイコンが割り当てられている。
 * @return int 送信したバイト数。負の値の場合はエラー。
 */
int lcd_IconSetRAW(bool isDisp , uint8_t iconAddr , uint8_t bits)
{
    int iSendBytes = 0;
    int iRet;
    uint8_t curValue = lcdSetting.aryIconValue[iconAddr];
    if (isDisp) {
        curValue = curValue | bits;
    } else {
        curValue = curValue & ~bits;
    }
    iRet = lcd_ExtendMode();
    iSendBytes += iRet;
    iRet = lcd_send_byte(LCDCommands.IS1_SETICON | iconAddr);
    iSendBytes += iRet;
    iRet = i2c_write_DataByte(curValue);
    iSendBytes += iRet;
    lcdSetting.aryIconValue[iconAddr] = curValue;
    iRet = lcd_NormalMode();
    iSendBytes += iRet;
    iRet = lcd_ReturnHome();                // ICONの設定をした後は、これ（lcd_cursorでもよい）を実行しないとNormalモードに戻らない？
    iSendBytes += iRet;
    return iSendBytes;
}
/**
 * @brief LCDのアイコンをオン/オフする。
 * 
 * @param isDisp 表示するか、消去するかのフラグ。trueのときはアイコンを表示
 * @param icon 操作するアイコン。　例えば、LCD_ICON::PHONE などと指定すれば、電話アイコンが表示/非表示の対象となる。
 * @return int 送信したバイト数。負の値の場合はエラー。
 */
int lcd_IconSet(bool isDisp ,LCD_ICON icon)
{
    int iRet;
    uint8_t iConAddr = (uint8_t)((icon >> 8) & 0xFF);
    uint8_t iConBits = (uint8_t)(icon & 0xFF);
    iRet = lcd_IconSetRAW(isDisp,iConAddr,iConBits);
    return iRet;
}

/**
 * @brief LCDのアイコンをすべて表示/消去する。
 * @param isDisplay アイコンを表示するか消去するかのフラグ。 trueのときはアイコンを表示
 * @return int i2cで送信したバイト数。-1のときはエラー
 * @details LCDの最上部に表示されるアイコンをすべて削除する。
 */
int lcd_IconSetAll(bool isDisplay)
{

    int iSendBytes = 0;
    int iRet;
    if (isDisplay) {
        iSendBytes += lcd_IconSet(true,LCD_ICON::ANTENA);
        iSendBytes += lcd_IconSet(true,LCD_ICON::PHONE);
        iSendBytes += lcd_IconSet(true,LCD_ICON::SOUND);
        iSendBytes += lcd_IconSet(true,LCD_ICON::INPUT);
        iSendBytes += lcd_IconSet(true,LCD_ICON::UP);
        iSendBytes += lcd_IconSet(true,LCD_ICON::DOWN);
        iSendBytes += lcd_IconSet(true,LCD_ICON::LOCK);
        iSendBytes += lcd_IconSet(true,LCD_ICON::SILENT);
        iSendBytes += lcd_IconSet(true,LCD_ICON::BAT1);
        iSendBytes += lcd_IconSet(true,LCD_ICON::BAT2);
        iSendBytes += lcd_IconSet(true,LCD_ICON::BAT3);
        iSendBytes += lcd_IconSet(true,LCD_ICON::BATTERY);
        iSendBytes += lcd_IconSet(true,LCD_ICON::S76);
    } else {
        for (int i = 0;i<16;i++) {
            lcd_ExtendMode();
            lcdSetting.aryIconValue[i] = 0;
            iRet = lcd_send_byte(LCDCommands.IS1_SETICON | i);
            iSendBytes += iRet;
            iRet = i2c_write_DataByte(0);
            iSendBytes += iRet;
            lcd_NormalMode();
        }
    }
    lcd_ReturnHome();
    
    
    return iSendBytes;
}

#endif
/**
 * @brief ディスプレイに表示されている文字を左右にシフトする。カーソル位置は表示文字に追従する。
 * 
 * @param ShiftCnt 負の値のときは、文字が左に指定数シフト、正の値のときは文字が右に移動する。
 * @return int i2cで送信したバイト数。-1のときはエラー
 * @details カーソル位置は最後の位置のまま、シフト操作に追従する。そのため、１文字左にシフトしてそのまま文字を１文字書くことで電光掲示板のように左にスクロールしながら文字を表示できる。\n 
 * 右シフトの場合、カーソル位置が変わらないので、シフト後の文字列の右に文字が書かれるので左シフトとは一見動作が異なる。
 * 自分でカーソルを0,0に移動させて文字を表示する必要がある。\n 
 * 二行表示されている場合、二行とも同時に同じ方向にシフトする。
 */
int lcd_DisplayShift(int8_t ShiftCnt)
{
    int iSendBytes = 0;
    int iRet;
    int8_t movecnt = abs(ShiftCnt);
    uint8_t moveOpt = 0;
    if (ShiftCnt <0) {
        moveOpt |= (LCDCommands.CurDispShiftOpt.DISPLAY_LEFT  | LCDCommands.CurDispShiftOpt.CURSOR_LEFT);
    } else {
        moveOpt |= (LCDCommands.CurDispShiftOpt.DISPLAY_RIGHT | LCDCommands.CurDispShiftOpt.CURSOR_RIGHT);
    }
    for (int8_t i=0;i<movecnt;i++) {
        iRet = lcd_send_byte(LCDCommands.IS0_CURDISPSHIFT | moveOpt);
        iSendBytes += iRet;
    }
    return iSendBytes;
}
/**
 * @brief カーソル位置を相対移動させる
 * 
 * @param MoveCnt 負の値の場合カーソルは左に移動、正の値の場合カーソルは右に移動する。
 * @return int i2cで送信したバイト数。-1のときはエラー
 */
int lcd_MoveCursor(int8_t MoveCnt)
{
    int iSendBytes = 0;
    int iRet;
    int8_t movecnt = abs(MoveCnt);
    uint8_t moveOpt = 0;
    if (MoveCnt <0) {
        moveOpt |= LCDCommands.CurDispShiftOpt.CURSOR_LEFT;
    } else {
        moveOpt |= LCDCommands.CurDispShiftOpt.CURSOR_RIGHT;
    }
    for (int8_t i=0;i<movecnt;i++) {
        iRet = lcd_send_byte(LCDCommands.IS0_CURDISPSHIFT | moveOpt);
        iSendBytes += iRet;
    }
    return iSendBytes;

}
/**
 * @brief 液晶の表示を止め、低消費電力モードにする
 * 
 * @param isSleep trueのときは低消費電力モードに移行、falseでWake。
 * @return int i2cで送信したバイト数。-1のときはエラー
 */
int lcd_Sleep(bool isSleep)
{
    int iRet;
    int iSendBytes = 0;

    iRet = lcd_ExtendMode();
    if (isSleep) {
        if (lcdSetting.isInSleep) return 0;                            // スリープ中なら何もしない
        iSendBytes += iRet;
        iRet = lcd_send_byte(LCDCommands.IS1_FOLLOWERCTRL);        // ボルテージフォロア回路オフ、増幅率はゼロ
        iSendBytes += iRet;
        iRet = lcd_send_byte(LCDCommands.IS1_POWERICONCTRL);       // アイコン表示オフ、電圧増幅回路オフ、コントラスト上位２ビットをゼロ
        iSendBytes += iRet;
        lcdSetting.isInSleep = true;
    } else {
        if (lcdSetting.isInSleep==false) return 0;                     // ウェイク中なら何もしない
        iRet = lcd_FollowerControlSet(true,lcdSetting.followerAmpRatio);   // ボルテージフォロア回路オン、増幅率をもとの値に戻す
        iSendBytes += iRet;
        uint8_t otherBit = 0;
        iRet = lcd_ContrastPowerIconSet(lcdSetting.uiContrast, lcdSetting.isPowerIconOn,lcdSetting.isPowerBoost); // コントラストを元に戻す
        iSendBytes += iRet;
        lcdSetting.isInSleep = false;
    }
    iRet = lcd_NormalMode();
    iSendBytes += iRet;
    return iSendBytes;
}
/**
 * @brief 外字を設定する
 * 
 * @param charNo 0～8までの文字番号。それぞれが、キャラクターコード０～８に相当する
 * @param aryPattern 8バイトの配列。フォントは5x8ドットなので、各バイトに５ビット分を格納する。最後の１行は、カーソルに使われるので0x00にしてお食方が良い。
 * @param size 配列のサイズ
 * @return int i2cで送信したバイト数。-1のときはエラー
 * 
 */
int lcd_CGRAMSet(uint8_t charNo , uint8_t *aryPattern, int size)
{
    int iRet;
    int iSendBytes = 0;
    // カーソルを一時的に非表示にする
    iRet = lcd_send_byte(LCDCommands.DISPLAYONOFF | (lcdSetting.isDisplayOn ? LCDCommands.DisplayOnOffOpt.DISPLAY_ON:0));
    iSendBytes+=iRet;
    uint8_t addr = (charNo << 3);
    // i2c_write_byte((0x40 | addr),true);
    iRet = lcd_send_byte(LCDCommands.IS0_SETCGRAM | (LCDCommands.SetCGRAMOpt.SETCGRAM_MASK & addr));
    iSendBytes+=iRet;
    for (int i=0;i<size;i++) {
        iRet = i2c_write_DataByte(LCD_CHARACTER,*aryPattern);
        iSendBytes+=iRet;
        aryPattern++;
    }
    // カーソル表示を元に戻す
    iRet = lcd_CursorMode(lcdSetting.isDisplayOn , lcdSetting.isUnderLine,lcdSetting.isBlink);
    iSendBytes+=iRet;
    lcd_CursorPosition(lcdSetting.curPosLine,lcdSetting.curPosColumn);
    return iSendBytes;
}

/*
 * @brief 液晶関連の初期化処理。この関数を呼び出すと、各種初期化が行われ、画面消去、カーソルを左上、アイコン全非表示となる。
 *  この関数が呼び出される前に、I2Cの初期化処理が済んでいる必要がある。
 * @details 
 * 例えば、次のような処理になる。
 *  i2c_init(使用するi2c回路（i2c0またはi2c1）, I2Cの速度); \n
 *  gpio_set_function(SDAとして使用するポート番号, GPIO_FUNC_I2C);\n
 *  gpio_set_function(SCLとして使用するポート番号, GPIO_FUNC_I2C);\n
 *  gpio_pull_up(I2C_SDA); \n
 *  gpio_pull_up(I2C_SCL); \n
 *  
 * 
 */
void lcd_init() 
{
    volatile int iRet;
    // 設定保存領域の初期化
    lcdSetting.isFunc_ISMode = false;
    lcdSetting.isFunc_2LINE = true;
    lcdSetting.isFunc_DoubleHeight = false;
    lcdSetting.isFunc_8Bit = true;
    lcdSetting.isDisplayToLeft = false;
#if LCD_ICONEXIST
    for (int i =0 ; i< sizeof(lcdSetting.aryIconValue) ; i++) {lcdSetting.aryIconValue[i] = 0;}
#endif
    lcdSetting.isFollowerOnOff = true;
    lcdSetting.followerAmpRatio = 0;
    lcdSetting.isPowerIconOn = false;
    lcdSetting.isPowerBoost = false;
    lcdSetting.uiContrast = 0;
    lcdSetting.isInSleep = false;
    lcdSetting.isBias1By4 = false;
    lcdSetting.OSCFreq = 0x04;
    lcdSetting.curPosLine = 0;
    lcdSetting.curPosColumn = 0;

    iRet = lcd_send_byte(0x03);
    iRet = lcd_send_byte(0x03);
    iRet = lcd_send_byte(0x03);
    iRet = lcd_send_byte(0x02);



    iRet = lcd_FunctionSet(LCDCommands.FuncSetOpt.EIGHTBITMODE | LCDCommands.FuncSetOpt.DOUBLELINE);
    iRet = lcd_FunctionSet(LCDCommands.FuncSetOpt.EIGHTBITMODE | LCDCommands.FuncSetOpt.DOUBLELINE | LCDCommands.FuncSetOpt.INSTRUCTIONTABLE);
    iRet = lcd_InternalOSCSet(false, 0x04);              // 1/5 bias , 183 Hz
    iRet = lcd_ContrastPowerIconSet(DEFAULT_CONTRAST,true , true);
    iRet = lcd_FollowerControlSet(true,4);

    //iRet = lcd_FunctionSet(LCD_FUNC_8BITMODE | LCD_FUNC_2LINE);
    lcd_EntryModeSet(false);
    lcd_CursorMode(true,true,true);
    lcd_CursorDisplay(true);
    lcd_ClearDisplay();
#if LCD_ICONEXIST
    lcd_IconSetAll(false);
#endif
    lcd_CursorPosition(0,0);




}


