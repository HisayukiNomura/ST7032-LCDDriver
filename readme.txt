/**  
@mainpage ST7032をコントローラーに使用した液晶用のライブラリ

@section main 概要

Strawberry Linux の[I2C低電圧キャラクタ液晶モジュール(SB1602B)](https://strawberry-linux.com/catalog/items?code=27001)をはじめとして、多くのI2C液晶モジュールは、液晶コントローラにST7032を使用している。
使用方法については多くの情報があるとはいえ、Raspberry pi Picoについては、Arduinoフレームワークや、Pythonのプログラム中心で、C/C++ SDKを使用したものは少ないようだった。
また、多くがコマンドを直接叩くようなサンプルなので、もう少し抽象化した関数を持ったものが欲しいと思い、ライブラリを作成した。PICなどと異なり、Raspberry pi PICOはフラッシュメモリ、RAMとも広大で、使いやすさのために多少のフットプリントを割いてもよいと考えた。

<hr/>
## このライブラリについて

Strawberry Linux の[I2C低電圧キャラクタ液晶モジュール(SB1602B)](https://strawberry-linux.com/catalog/items?code=27001)を、Raspberry PI Picoに接続して使用する際のライブラリ。
ほかの、ST7032iを液晶コントローラにしている液晶モジュールでも使用できるが、SB1602BのLCDで文字領域の上に表示できるアイコンについては動作しない。（液晶にアイコン自体が無いため）
Strawberry Linux の[I2C低電圧キャラクタ液晶モジュール(SB1602B)](https://strawberry-linux.com/catalog/items?code=27001) と、
秋月電子の[Raspberry Pi キャラクター液晶ディスプレイモジュールキット(AE-AQM0802+PCA9515)](https://akizukidenshi.com/catalog/g/g111354/)で動作を確認したが、
その他、ST7032をコントローラーにしたもの、例えば秋月のI2C接続小型キャラクターLCDモジュール(AQM1602XA-RN-GBW)等でも使用できる可能性がある。

<hr/>
## ソースコード

ソースコードはGitHubで公開してある。\n
[ST7032-LCDDriver](https://github.com/HisayukiNomura/ST7032-LCDDriver)\n
GitHubは、そのままVSCodeのフォルダとして取り込み、Raspberry pi PicoのSDKでビルド/実行できる。\n

Githubには、ライブラリの本体として必須のファイルと、それ以外のファイルがある。
ソースコードには、doxygenコメントが含まれており、ソースコード上でマウスをホバーしたり、入力時に説明が表示される。
また、doxygenを使用することで、プログラムのドキュメントを生成できる。作成済みドキュメントが、LCDDriver_Document.zipとして公開されている。

### mandatory ライブラリを使用するのに必須なファイル

以下の３ファイルは、ライブラリを使用する際に必須のファイル。実際のプロジェクトの一部として組み込む必要がある。
- i2cLCD.cpp　ライブラリ本体
- i2cLCD.h　ライブラリを使うプログラムがincludeする、関数のプロトタイプなどが行われているヘッダファイル
- i2cLCDlocal.h　ライブラリ本体が使うヘッダファイル。使用するだけであればincludeする必要はない

### その他のファイル

- LCDDriver.cpp 関数の使い方が書いてあるサンプル。実際のプロジェクトに組み込むことはできないが、ソースコードを参照してライブラリの使用方法を確認することができる
- LCDDriver_Document.zip　ドキュメントファイル。解凍し、index.htmlをブラウザで表示させるとプログラムの詳細なドキュメントが表示される
- CMakeLists.txt　サンプルプログラムをビルドする際に必要なファイル。Raspberry PI picoのSDKでプロジェクトを作成すると、自動的に作成されるが、必要に応じて変更が必要
- pico_sdk_import.cmake Raspberry pi picoのSDKを使用するためのファイル。Raspberry PI picoのSDKでプロジェクトを作成すると、自動的に作成される
- readme.txt ドキュメントファイルのトップページ記述のファイル

<hr/>
## 特徴

- ST7032iのコマンドやi2cのプロトコルが隠ぺいされており、関数の呼び出しだけでコントロールが可能
- SB1602Bに実装されているアイコン表示（液晶上部にあるアンテナや電話、バッテリなどのアイコン）も制御可能
- 実装（主に液晶とPico本体の結線）にあわせて変更が必要な個所はヘッダファイルにまとまっており変更が容易

<hr/>
## ST7032を使用した液晶ディスプレイの操作
ST7032を使用し、i2c接続で使用する際には、コマンドやデータを、正しく構成して正しい順番で送信する必要があり、そのまま使うには毎回データシートと首っ引きで送信するコマンドやオプションを設定する必要がある。

例えば初期化処理などでは次のような一連のコマンドをi2cを使って送信する。

@code
	i2c_cmd(0b00111000); // function set
	i2c_cmd(0b00111001); // 命令を拡張モード(IS=1)に設定
	i2c_cmd(0b00010100); // オシレータの設定
	i2c_cmd(0b01110000 | (contrast & 0xF)); // 液晶コントラストの下位
	i2c_cmd(0b01011100 | ((contrast >> 4) & 0x3)); // 液晶コントラストの上位２と、それ以外の設定
	i2c_cmd(0b01101100); // 電圧フォロア回路の設定
	wait_ms(300);
	i2c_cmd(0b00111000); // 命令を標準モード(IS=0)に戻す
	i2c_cmd(0b00001100); // ディスプレイをオンにする
	i2c_cmd(0b00000001); // 画面を消去する
@endcode 

これら一連の処理を、このライブラリでは lcd_init() などの関数にまとめて、簡単に使用できるようにしてある。\n
初期化処理では、コントラストは中間程度、２行表示など「ありがちな」設定に初期化され文字が表示できる状態になるため、
細かい設定について初期化完了後に変更すればよい。例えば、コントラストを変更したい場合などは次のようになる。

@code 
    lcd_init();
    lcd_ContrastSet(変更後のコントラスト,true , true);
@endcode

<hr/>
## 実装にあわせた変更

このライブラリでは、実装にあわせて一部のマクロで変更が必要。変更箇所は、i2cLCD.hに集約されている。

### 変更頻度の高いと思われるマクロ 

マクロ名  | デフォルト|説明
--------- | ---------|----
I2C_PORT  | i2c0     |使用するI2Cのハードウェアブロック番号。i2c0 か i2c1を指定する。
I2C_SDA   | 8        |SDAポートとして使用するポート番号。ピン番号ではない。例えば8の場合、GP8を意味するので１１番ピンとなる。
I2C_SCL   | 9        |SCLポートとして使用するポート番号。ピン番号ではない。例えば9の場合、GP9を意味するので１２番ピンとなる。
DEFAULT_CONTRAST|0b00101000|デフォルトのコントラスト。使用する液晶により適正値は異なる。strawberry linuxのSB1602Bにあわせてある。


### 変更頻度は低いが変更可能なマクロ 

マクロ名  | デフォルト|説明
--------- | ---------|----
I2C_SPEED|100 * 1000|I2Cのボーレート(HZ)。100*1000の場合は、100KHzとなる。命令の取りこぼしがある場合などに調整する
I2C_ADDRESS|0b0111110|I2Cのスレーブi2cアドレス。ST7032の場合、常に0b0111110。異なるコントローラの場合などに変更する。
MAX_LINES|2         |接続されている液晶の最大表示行数
MAX_CHARS|16        |接続されている液晶の最大表示桁数
CMD_DELAY|30        |一般コマンドの実行後の短い待ち時間
CMD_DELAY|16        |一部のコマンドの実行後の長い待ち時間
LCD_ICONEXIST|true| 接続されいている液晶にアイコン表示機能があるか

<hr/>
## 使用するまでの手順

###　ハードウェア
- RSTは必ず電源に接続する
- SCL/SDAはプルアップが必要だが、Raspberry PI PICOでは本体側にプルアップの制御があるので、内蔵のプルアップを使用するのであれば必要ない
- Raspberry pi PICOにはGNDが複数あるが、今回のテスト/開発用途では１か所接続すれば構わない

#### Strawberry Linux SB1602Bでの結線例

<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/f85d164e-c94f-2cd5-d2a3-5c58ff7198ca.png" width =50%>

#### 秋月 AE-AQM0802+PCA9515での結線例

LEDの端子をVDDに接続するとバックライトが点灯するので視認性が高くなる。バックライトは動作には必須ではない。抵抗液晶に内蔵されているので不要。\n
<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/b7288f03-f7e7-aa77-f17c-efaa5e385d03.png" width=50%>


### ソフトウェア

#### Visual Studioへの環境構築（初回のみ）とプロジェクト作成
-# VSCodeで、拡張機能からRaspberry pi Picoをインストールし、SDK環境を構築する<br/>
<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/b3ebfcb4-aff5-049f-33d1-b49b657aa639.png" width=50%>
-# プロジェクトウイザードで、i2cにチェックを入れてプロジェクトを新規作成する<br/>
<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/0861b13a-2025-ca59-c35d-2a6eafd397b3.png" width=50%><br/>
初回は、ツールチェインのダウンロードに非常に長い時間がかかるので、焦らず待つ。ダウンロード中は右下に進行状況が表示される。<br/>
<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/b3f2debd-255b-5027-bc32-8581fe232dae.png" width=50%>
-# プロジェクトが生成されると、自動的にプロジェクトが開くのでメインプロジェクトをクリックする。<br/>
<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/1c3d027e-a1f7-78c0-21be-981b69a032bc.png" width=50%><br/>
I2Cの初期化だけ行うようなコードが表示される。<br/>
自動的にプロジェクトが表示されない場合、左上のエクスプローラーのボタンを押してプロジェクトを開く。それでも表示されない場合は、FILE > フォルダを開くから、作成したプロジェクトのフォルダを開く。


#### ライブラリの組み込み
-# i2cLCD.cpp、i2cLCD.h、i2cLCDlocal.hの３本のファイルを、Windowsのエクスプローラなどを使って、作成したプロジェクトのフォルダにコピーする<br/>
<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/61bf5f1c-5d93-2ce5-bcb2-37e797546636.png" width=50%><br/>
この操作でVSCodeにライブラリが取り込まれる。VSCode側の処理は必要ない<br/>
<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/b678f9a4-4b5c-08a5-d272-8d737a1f7f85.png" width=50%>
-# プロジェクトの CMakeLists.txtを開き、add_executableに i2cLCD.cppを追加する　（例：　add_executable(プロジェクト名 メインプログラム.cpp i2cLCD.cpp)　）<br/>
<img src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/8421cace-4de7-4729-8bbe-2fc051b3650b.png" width=70%>


#### メインプログラムからライブラリを使用
1. LCDDriver.cppを参考に、メインプログラム内でi2cと液晶ライブラリを初期化し、使用する。自動生成されたメインプログラムから行う、最低限の変更は、次のようになる。　

@code
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "i2cLCD.h"   // 追加
// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
//#define I2C_PORT i2c0     // 削除(i2clcd.hで定義されるため)
//#define I2C_SDA 8         // 削除(i2clcd.hで定義されるため)
//#define I2C_SCL 9         // 削除(i2clcd.hで定義されるため)
int main()
{
    stdio_init_all();
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c
    lcd_init();                     // 追加
    lcd_string("Hello, World!");    // 追加
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
@endcode

#### 応用プログラム

文字が出力されたことを確認したら、githubで同梱されているLCDDriver.cppのプログラムなどを参考に、より複雑なプログラムを作成する。


## 主な関数


画面に文字を表示するときに、最低限知っておく必要がある関数は次の通り。機能については、名前から想像するか、i2clcd.cppの説明を参照する。
それ以外の関数については、i2clcd.cppの説明を参照する。

- void lcd_init();   			初期化処理。最初に１度必ず実行する 
- int lcd_ClearDisplay(void);	画面の消去
- lcd_ReturnHome(void);			カーソルを左上に移動
- lcd_CursorPosition(int line, int position) ;　カーソルを指定した位置に移動
- lcd_CursorDisplay(bool);		カーソルを表示/非表示にする
- lcd_string(const char *s);	文字列を画面に出力する
- lcd_printf(const char *format, ...);	フォーマット付きで文字列を画面に出力する

@section 外部情報

[Strawberry Linux I2C低電圧キャラクタ液晶モジュール(SB1602B)](https://strawberry-linux.com/catalog/items?code=27001)\n
[秋月電子 Raspberry Pi キャラクター液晶ディスプレイモジュールキット(AE-AQM0802+PCA9515)](https://akizukidenshi.com/catalog/g/g111354/)\n
[ST７０３２iデータシート](https://strawberry-linux.com/pub/ST7032i.pdf)\n
[Qiitaの説明ページ](https://qiita.com/BUBUBB/items/e43e1915a2631a859b42)\n

*/ 

  


