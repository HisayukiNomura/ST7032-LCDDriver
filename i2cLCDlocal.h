/**
 * @file i2cLCDlocal.h
 * @author Hisayuki Nomura
 * @brief i2cLCD.cppのインクルードファイル。i2cLCD.cpp内部だけで使用されているヘッダファイルであり、ほかのプログラムがincludeする必要はない。
 * @version 0.1
 * @date 2024-10-22
 * 
 * @copyright Copyright (c) 2024
 * @details i2cLCD.cpp内部で使用している変数、マクロ定義などを格納しているヘッダファイル。単にファイルが長くなってきたので分割しただけであり、ファイルが分かれている必要性はない。\n
 * 液晶ライブラリを使用するプログラムが必要なヘッダファイルは、i2cLCD.hだけなので、このヘッダファイルはinclude しないこと。
 */
#ifndef __i2cLCDLocal_h__
#define __i2cLCDLocal_h__

#ifndef __i2clcd_h__
#include "i2clcd.h"
#endif


/// @brief 接続されている液晶の最大行数
#define MAX_LINES      2
/// @brief 接続されている液晶の最大カラム数
#define MAX_CHARS      16




/// @brief LCDに送信する際のコマンドコード。データ（文字列）を送信する。
/// @details ライブラリを使うだけの場合はこのコマンドを直接使用することは無い。
const static uint8_t LCD_CHARACTER = 0x40;
/// @brief LCDに送信する際のコマンドコード。コマンドを送信する。
/// @details ライブラリを使うだけの場合はこのコマンドを直接使用することは無い。
const static uint8_t LCD_COMMAND = 0x0;



/**
 * @brief ST7032のコマンド定義。
 * @details ライブラリを使うだけの場合はここで定義されているコマンドは使用せず、機能別の関数を使用する。
 */
const static struct LCD_COMMANDS {
    /// @brief ディスプレイの消去。lcd_ClearDisplay()で使用されている
    uint8_t CLEARDISPLAY = 0x01;
    /// @brief カーソルを(0,0)に移動。lcd_ReturnHome()で使用されている
    uint8_t RETURNHOME = 0x02;
    /// @brief 文字の表示方向指定。lcd_EntryModeSet(bool isDisplayToLeft)で使用されている
    uint8_t ENTRYMODESET = 0x04;
    /// @brief ディスプレイやカーソルの表示制御。int lcd_CursorDisplay(bool isDisp)などで使用されている。
    uint8_t DISPLAYONOFF = 0x08;         
    /// @brief ディスプレイの機能設定  lcd_FunctionSet()、lcd_NormalMode()、int lcd_ExtendMode()等から使用される。
    uint8_t FUNCTIONSET = 0x20;               
    /// @brief 操作するディスプレイメモリ（文字やアイコン）のアドレス設定。
    uint8_t SETDDRAMADDR = 0x80;              
    
    /// @brief 標準(IS=0)コマンド。カーソルの移動方向指定。lcd_DisplayShift(int8_t ShiftCnt)などで使用されている。
    uint8_t IS0_CURDISPSHIFT = 0x10;
    /// @brief 標準(IS=0)コマンド。外字のフォント設定
    uint8_t IS0_SETCGRAM = 0x40;

    
    /// @brief 内部オシレータ設定。lcd_InternalOSCSet(bool  isBs, uint8_t OSCFreq)関数で使用されている。
    uint8_t IS1_INTOSC = 0x10; 
    /// @brief 表示するアイコンのアドレス設定。lcd_IconSet(bool isDisp ,LCD_ICON icon)関数で使用されている。
    uint8_t IS1_SETICON = 0x40;
    /// @brief アイコン表示や電源ブースト設定など。lcd_ContrastSetやlcd_Sleepなどで使用されている。
    uint8_t IS1_POWERICONCTRL = 0x50;
    /// @brief ボルテージフォロア回路の設定。lcd_FollowerControlSet(bool isOnOff,uint8_t ampRatio)で使用されている。
    uint8_t IS1_FOLLOWERCTRL = 0x60;
    /// @brief 液晶コントラストの設定 lcd_ContrastSet関数で使用されている。実際の液晶の設定はPOWERICONCTRLなども組み合わされるので、これだけでは使えない。
    uint8_t IS1_FOLLOWERCONTRAST = 0x70;


    /**
    * @brief LCD_FUNCTIONSETコマンドを使用するときのオプションフラグ
    * 
    */
    const struct  LCDFUNCSET_Option {
        /// @brief ８ビットモードで動作させる。このライブラリは４ビットモードをサポートしていないので、常に指定する。int lcd_FunctionSet()などで使用されている。
        uint8_t EIGHTBITMODE = 0x10;

        /// @brief LCD_FUNCTIONSETのときのオプション。２行モードを指定する。これを指定しない場合、１行目だけが使用される。
        /// int lcd_FunctionSet(bool is8Bit , bool is2Line ,bool isExtInstruction)などで使用されている。
        uint8_t DOUBLELINE = 0x08;

        /// @brief LCD_FUNCTIONSETのときのオプション。倍高フォントモードを指定する。これを指定し、１行モードを使用すると２倍の高さの文字が１行で表示される。
        /// int lcd_FunctionSet(bool is8Bit , bool is2Line ,bool isExtInstruction);などで使用されている。
        uint8_t DOUBLEHEIGHT = 0x04;

        /// @brief LCD_FUNCTIONSETのときのオプション。Extention instruction table(IS=1)を使用する。
        /// int lcd_FunctionSet(bool is8Bit , bool is2Line ,bool isExtInstruction)、lcd_NormalMode()、lcd_ExtendMode()などで使用される。
        uint8_t INSTRUCTIONTABLE = 0x01;
    } FuncSetOpt;


    /**
    * @brief LCD_ENTRYMODESETコマンドを使用するときのオプションフラグ
    * 
    */
    const  struct ENTRYMODESET_OPTION {
        /// @brief LCD_ENTRYMODESETコマンドの時使用するオプション。文字の書き込み方向を設定する。
        /// @details このフラグがオンでDDRAM書き込みを行った場合、I/D値（I/D="1"：左シフト、I/D="0"：右シフト）により表示全体のシフトとなる。
        ///  このオプションは使用せず、lcd_EntryModeSet(bool isDisplayToLeft)を使用する。
        uint8_t SHIFTINCREMENT = 0x01;

        /// @brief LCD_ENTRYMODESETコマンドの時使用するオプション。 
        /// @details このフラグがオンでDDRAM書き込みを行った場合、I/D値（I/D="1"：左シフト、I/D="0"：右シフト）により表示全体のシフトとなる。
        ///  このオプションは使用せず、lcd_EntryModeSet(bool isDisplayToLeft)を使用する。
        uint8_t LEFT = 0x02;             
    }  EntryModeOpt;

    /**
    * @brief LCD_DISPLAYONOFFコマンドを使用するときのオプションフラグ
    * 
    */
    const  struct DISPLAYONOFF_OPTION {
        /// @brief カーソル位置の文字が点滅する。（■と文字が交互に表示される）lcd_CursorModeなどから使用されている。
        uint8_t CURBLINK_ON= 0x01;
        /// @brief カーソル位置に下線が表示される。lcd_CursorModeなどから使用されている。
        uint8_t  CURSOR_ON = 0x02;
        /// @brief LCD_DISPLAYONOFFコマンド時のオプション。ディスプレイ全体が表示される。
        uint8_t  DISPLAY_ON = 0x04;
    } DisplayOnOffOpt;

    /// @brief SETDDRAMADDRの際のオプション
    const  struct  SETDDRAM_OPTION {
        /// @brief LCD_SETDRAMADDRのときのフラグ。
        /// @details ライブラリを使うだけの場合はこのコマンドを直接使用せず、アイコンの設定関数や、文字列の表示関数を使用する。
        uint8_t LCD_SETDDRAM_MASK = 0b01111111;
    } DDRAMOpt;

    /**
     * @brief LCD_TBL0_CURDISPSHIFTコマンドのときのオプションフラグ
     * 
     */
    const  struct CURDISPSHIFT {
        /// @brief カーソルを左に移動させる。
        uint8_t CURSOR_LEFT  = 0b00000000;

        /// @brief カーソルを右に移動させる。
        uint8_t CURSOR_RIGHT = 0b00000100;

        /// @brief 画面全体を左に移動させる。
        uint8_t DISPLAY_LEFT = 0b00001000;

        /// @brief 画面全体を右に移動させる。
        uint8_t DISPLAY_RIGHT= 0b00001100;
    } CurDispShiftOpt;

    
    /**
     * @brief LCD_TBL0_SETCGRAMコマンドのときのオプションフラグ
     * 
     */
    const  struct SETCGRAM {
        /// @brief LCD_TBL0_SETCGRAMの際に、外字のアドレス（0b00000000～0b00111000 ８キャラクタでそれぞれ縦８ドット)のマスク。
        uint8_t SETCGRAM_MASK = 0b00111111;
    } SetCGRAMOpt;

    /**
     * @brief LCD_TBL1_INTOSCコマンドの際のオプション。
     * 
     */
    const  struct INTOSC {
        /// @brief LCD_TBL1_INTOSCの際のオプション。1/4バイアスか、1/5バイアスかを指定する。このオプションをつけると1/4バイアスになる。
        /// @details ライブラリを使うだけの場合は直接使用せず、lcd_InternalOSCSet(bool  isBs, uint8_t OSCFreq)を使用する。
        uint8_t BIAS1BY4 = 0x08;

        /// @brief LCD_TBL1_INTOSCの際のオプション。このマスク（下位３ビット）内蔵オシレータの調整値を指定する。
        /// @details ライブラリを使うだけの場合は直接使用せず、lcd_InternalOSCSet(bool  isBs, uint8_t OSCFreq)を使用する。
        uint8_t FREQMASK = 0b00000111;
    } IntOSCOpt;
    /**
     * @brief IS1_SETICON コマンドを使用するときのオプションフラグ
     * 
     */
    const  struct SETICON {
        /// @brief IS1_SETICON の際の、アイコンビット指定。アドレスは0～16、ビットは各5ビット、16x5=80種類のアイコンが制御できる。このビット指定のビットマスク。
        /// @details 本ライブラリではこのマスクは使用していない。
        uint8_t SETICON_MASK = 0b00011111;        
    } SetIconOpt;
    /**
     * @brief TS1_POWERICONCTRLコマンドを使用する場合のオプション。
     * @details 通常、TS1_POWERICONCTRLは単体で呼ばれず、TS1_FOLLOWERCONTRAST とセットで呼ばれる。\n 
     * 液晶コントローラで文字のコントラストは、TS1_POWERICONCTRLで上位２ビット、IS1_FOLLOWERCONTRASTで下位４ビットを指定する。\n
     * そのため、lcd_ContrastSet(int contrast , bool is_PowerIconCtrl_IconOn , bool is_PowerIconCtrl_Boost)関数などで、
     * コントラストとアイコン表示、電源ブーストなど、IS1_FOLLOWERCTRLとIS1_FOLLOWERCONTRASTをセットにして処理している。   
     */
    const  struct POWERICON {
        /// @brief  IS1_POWERICONCTRLを使用する際のオプション。アイコン表示を行うかどうかの指定。
        /// @details lcd_ContrastSet(int contrast , bool is_PowerIconCtrl_IconOn , bool is_PowerIconCtrl_Boost)の関数で使用されている。
        uint8_t ICON_ON = 0x08;

        /// @brief  IS1_POWERICONCTRLを使用する際のオプション。電源ブーストを行うかどうかを指定する。
        /// @details lcd_ContrastSet(int contrast , bool is_PowerIconCtrl_IconOn , bool is_PowerIconCtrl_Boost)の関数で使用されている。
        uint8_t POWERBOOST = 0x04;

        /// @brief IS1_POWERICONCTRLを使用する際のオプション。液晶コントラスト指定の上位２ビット用のビットマスク。
        /// @details lcd_ContrastSet(int contrast , bool is_PowerIconCtrl_IconOn , bool is_PowerIconCtrl_Boost)などで使用されている。
        uint8_t CONTRASTUPPER_MASK = 0b00000011;
    } PowerIconOpt;

    /**
     * @brief IS1_FOLLOWERCTRLコマンドを使用するときのオプション
     */
    const  struct FOLLOWER {
        /// @brief IS1_FOLLOWERCTRLを使用する際のオプション。電圧フォロア回路をオンにする。通常はオン指定。
        /// @details ライブラリを使うだけの場合は直接使用せず、int lcd_FollowerControlSet(bool isOnOff,uint8_t ampRatio)を使用し、第１引数に指定する。
        uint8_t ON = 0x08;

        /// @brief IS1_FOLLOWERCTRLを使用する際のオプション。増幅調整値を指定するためのマスク。
        /// @details ライブラリを使うだけの場合は直接使用せず、int lcd_FollowerControlSet(bool isOnOff,uint8_t ampRatio)を使用し、第２引数に指定する。
        uint8_t AMPRATIO_MASK = 0b00000111;

    } FollowerOpt;

    /**
     * @brief IS1_FOLLOWERCONTRASTコマンドを使用する時のオプション。
     * @details 通常、IS1_FOLLOWERCONTRASTは単体で呼ばれず、IS1_FOLLOWERCTRLとセットで呼ばれる。\n 
     * 液晶コントローラで文字のコントラストは、IS1_FOLLOWERCTRLで上位２ビット、IS1_FOLLOWERCONTRASTで下位４ビットを指定する。\n
     * そのため、lcd_ContrastSet(int contrast , bool is_PowerIconCtrl_IconOn , bool is_PowerIconCtrl_Boost)関数などで、
     * コントラストとアイコン表示、電源ブーストなど、IS1_FOLLOWERCTRLとIS1_FOLLOWERCONTRASTをセットにして処理している。
     */
    const  struct CONTRAST {
        // IS1_FOLLOWERCTRL　を使用する際のオプション。液晶のコントラストの下位4ビットを指定するためのマスク。（POWERICONCTRLで、上位2ビットを指定）
        /// @details ライブラリを使うだけの場合は直接使用せず、lcd_ContrastSet(int contrast , bool is_PowerIconCtrl_IconOn , bool is_PowerIconCtrl_Boost)を使用し、第３引数に指定する。
        uint8_t CONTRASTLOWER_MASK = 0b00001111;
    } FollowerContrastOpt;
} LCDCommands;


/**
 * @brief 現在のLCDに対する設定値を保存する構造体。
 * @details 液晶に関するコマンドは、１つのコマンドで複数のオプション設定がある。一部分だけ設定を変えたいとき、
 * この構造体に含まれる設定を使用して、現在の設定を変えないまま、変更する。\n 
 * 例えば、命令モードを変えたい場合、LCD_FUNCTIONSET　で、最下位ビット（LCD_FUNC_INSTTBL_SELECT)を変更する必要がある。
 * しかし、LCD_FUNCTIONSETには、ほかにも８ビットモード切替、２行表示モード切替などほかのビットも含まれている。
 * 何もしていないと、現在の液晶の設定を読みだせないので、最下位ビットだけを変更することができない。
 * そこで、このライブラリで最後に設定した、８ビットモードや２行表示モードの指定を保存しておき、これらの設定を使用する。
 */
struct LCDSetting {
    /// @brief 現在行われているLCDへの設定を保存するための変数。現在のインストラクションテーブル。trueの場合はIS=1（Extention Mode)
    bool isFunc_ISMode; 
    /// @brief 現在の表示行数。trueの場合は２ｘ１６表示。
    bool isFunc_2LINE; 
    /// @brief 現在の文字の高さ。trueの場合は１行x16のときに使用される倍の高さ文字
    bool isFunc_DoubleHeight;
    /// @brief 現在のデータビット数。trueの場合はデーターは８ビット。このライブラリでは常にtrue
    bool isFunc_8Bit;
    /// @brief 現在の文字の表示方向。右⇒左の場合はtrue。
    bool isDisplayToLeft;
#if LCD_ICONEXIST
    /// @brief 現在のアイコンの表示状態
    uint8_t aryIconValue[16];
#endif
    /// @brief 現在の電圧フォロア設定。 lcd_FollowerControlSet()関数で設定される
    bool isFollowerOnOff;
    /// @brief 現在のV0 ジェネレータの増幅率調整値 lcd_FollowerControlSet()関数で設定される
    uint8_t followerAmpRatio;
    /// @brief  現在のアイコン表示。lcd_ContrastSet()で設定される。
    bool isPowerIconOn;
    /// @brief 現在のPowerBoost の値。lcd_ContrastSet()で設定される。
    bool isPowerBoost;
    /// @brief 現在のコントラスト値。lcd_ContrastSet()で設定される。
    uint8_t uiContrast;
    /// @brief 現在スリープモード中かを示すフラグ。int lcd_Sleep(bool isSleep)で設定される。
    bool isInSleep;
    /// @brief 画面全体を表示しているかどうかのフラグ
    bool isDisplayOn;
    /// @brief カーソル位置の下線表示がオンかどうかのフラグ
    bool isUnderLine;
    /// @brief カーソル位置の文字を点滅させるかどうかのフラグ
    bool isBlink;
    /// @brief オシレータ調整値
    uint8_t OSCFreq;
    /// @brief 1/4バイアスかのフラグ
    bool isBias1By4;
    /// @brief カーソル表示のオンオフ切り替え。カーソルの表現方法はisUnderlineや、isBrinkに従う。
    bool isCursorDisplay;
    /// @brief 現在のカーソルの行
    uint8_t curPosLine;
    /// @brief 現在のカーソルのカラム
    uint8_t curPosColumn;
};
/**
 * @brief 現在のLCDに対する設定値を保存する構造体の実体。Strawberry 液晶は現在の状態を読みだすことができないので、このライブラリで行った設定を保存しておく
 * @details 詳細は、データ構造を参照。
*/
struct LCDSetting lcdSetting;

#endif