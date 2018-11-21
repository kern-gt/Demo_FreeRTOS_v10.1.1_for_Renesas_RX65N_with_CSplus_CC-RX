# FreeRTOS Ver10.1.1 DemoProject for Renesas RX65N (CS+,CC-RX)

## LED点滅サンプルVer1.00
　このデモはFreeRTOSv10.1.1をRenesas RX65Nマイコン用に移植したものです。評価ボード上の2つのLEDを2タスクでLチカするだけの簡単なサンプルです。そのままビルドしてデバッグモードで動きます。
 
　2018年春にルネサスから発売された[Target Board for RX family](https://www.renesas.com/ja-jp/products/software-tools/boards-and-kits/cpu-mpu-boards/rx-family-target-board.html)(RX65N)を手に入れたのでFreeRTOSを動かしてみました。
秋葉原の[マルツ](https://www.marutsu.co.jp/pc/i/953239/)で2,980円（税抜き）で売られています。デバッガのE2Liteがボード上に搭載されてこの値段は安いと思います。

　なお、素人の学生が趣味で作成したものですので、もしご利用の際はくれぐれも自己責任でお願いします。
 
参考にさせていただいたもの  
　<https://blog.goo.ne.jp/lm324/e/99f735aef942ce6b965cd2f985acdf73>  
　<http://be-con.jp/shiryo/renesas-rx62-freertos-csprj.html>  
　<https://www.freertos.org/RX64M_RTOS_Renesas_GCC_e2studio.html>  
大変参考になりました。ありがとうございました。

文字コードは __UTF-8__ を使用しています。

## 動作環境
* FreeRTOS:v10.1.1 (RX600 RXv2)  
* 開発環境：CS+forCC V7.00.00  
* コンパイラ：CC-RX V2.08.00 (C99)  
* CPUボード：TARGET BOARD for RX65N (RTK5RX65N0C00000BR)  
* CPU(ボード上)：R5F565NEDDFP (100-pin LFQFP,120MHz,RAM 640KB,ROM 2MB+32KB)  
* "エミュレータ"(ボード上) ~~"E2エミュレータLite"~~：CS+環境ではデバッグに使用できました。E2Liteとして認識しますが、ドキュメントの方では「エミュレータ」としか書いてないので厳密にはE2Liteではないようです(チップの見た目はRX231?)。また、RFPv3.05以降によるプログラム書き込みは可能です。


## サンプルコード内容
CPUボード上のLED0(PD6)を1Hz、LED1(PD7)を5Hzで点滅させる2つのタスクを動かします。

　クロック発生回路とポート初期化をスマートコンフィグレータで設定しています。 
FreeRTOSではカーネルタイマにコンペアマッチタイマ0(CMT0),コンテキストスイッチにソフトウェア割込み(SWINT)を使用しているので、その周辺機能は使用しないでください。カーネルタイマなどはソース改変すればCMT0以外のタイマも使えるはずです。

## プロジェクト作成法
　自力でFreeRTOSプロジェクトを作るためのメモになります。サンプルコードの`main.c`と`ApplicationHook.c`は公式サンプルコードを参考に作成しました。

  1. CS+でプロジェクト新規作成します。ここで自分はビルド設定(CC-RXのプロパティ)で文字コードをUTF-8に変更してしまいますが、SHIFT-JISのままでいけるかどうかは未検証です。ちなみにUTF-8の変更箇所はコンパイル・オプションで2か所、アセンブル・オプションで1か所です。
  1. スマートコンフィグレータで周辺機能の設定を必要があれば設定してください。このデモでは自動コード生成のPORT設定でLED端子を出力にしています。ただし、コンペアマッチタイマ(CMT0)、ソフトウェア割込み(SWINT)はFreeRTOS側が使うので何もせずほっといてください。ただし、CMT0はRTOSにとってのカーネルタイマとなります。CMT0の供給クロックPCLKB=60MHzを前提としていますので、これ以外の周波数に変更する際はCMT0の再設定をしてください。なお、FreeRTOSは起動時にユーザ側にCMT0の設定をする処理を要求してきます。`main.c`内の`vApplicationSetupTimerInterrupt()`でCMT0の初期設定を行っていますので、そちらを参考にしてください。
  1. サンプルコードのFreeRTOSフォルダ以下をそのままプロジェクトフォルダにコピーしてからプロジェクトに登録してください。IDEのプロジェクトツリーにフォルダごとドラッグ&ドロップして出てきたダイアログの階層を１０とかに増やしてあげれば登録が楽です。
  1. サンプルコードの`ApplicationHook.c`の中身はフック関数類を定義してあります。`FreeRTOSConfig.h`でフック関数の有無を設定できますが、取りあえず最小限のものだけ定義してあります。取りあえずこれもファイルごとコピーしてプロジェクトに登録してください。また、必要に応じて自力で関数の追加記述をしてください。
  1. サンプルコードのmain.cにある`vApplicationSetupTimerInterrupt()`はカーネルタイマの初期設定で必ず必要です。関数を新規の`main.c`にコピーするなり新たにソースファイル作るなり任意の場所に定義してください。
  1. `/FreeRTOS/FreeRTOSConfig.h`を目的に合わせて設定してください。
  1. 各ユーザソースファイルで`FreeRTOS.h, task.h, queue.h`など適切なヘッダをインクルードしてください。
  1. ビルドします。おそらく通るはずです。
  1. 任意でデバッグなどなど楽しんでください。

## 注意点
* ソースコードの文字エンコードに __UTF-8__ を使用するため、ビルド設定をShift-JISからUTF-8に変更してあります。
* FreeRTOSのメモリ管理ファイルは「heap_1.c」を使用しています。目的に応じて変更してください。`/FreeRTOS/portable/heap_1.c`に置いてあります。