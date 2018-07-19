# Nixie Clock
![](/img/nixie_clock.png)
## English
A nixie clock inspired by the divergence meter in Steins;Gate, an anime with quite a lot of old school electronics serving as critical objects to the plots. Nixie clocks also appeared in other creations, but few people use them for actual purpose nowadays. They requires power supply with high voltage, as well as relatively more space for a single digit. However, people tend to define objects which do not think about these vacuum tubes with their own feelings. They just look pretty cool. :)

My design of the display module turned out to be quite different. It is based on a bigger PCB with messsier layouts with nine ICs due to the high cost of SMT in a small production, which induced the replacement of ICs with DIP package and resistors with AXIAL package. Eight IN-14's are used for a lower cost and the display of commas. each of the eight 4-to-16 decoders with latches is used for the control of the display, and one 3-to-8 decoder serves as the enable signals for each of the eight decoders. As a result, the ports for communication with Arduino are limited to eight (further using a serial-to-parallel IC can help further lower that number). The power supply with 170V voltage for nixie tubes was designed to be provided externally for flexibility.

Other modules used include a real time clock module, which provides the time for display and the temperature (DS3231), a GPS module, which helps correct the real time and provides the current location, and a IR module for changing the modes of display.

The execution provides several modes of display, including current time, current date, flush (prevention of cathode poisoning), divergence of world line (a definition from Steins;Gate), temperature, location, et cetera.

This repo includes the source files, and a schematic diagram of the digital circuit of the display module. External header files used:
* [DS3231.h](http://www.rinkydinkelectronics.com/library.php?id=73 "Rinky Dink Electronics")
- [IRremote.h](http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html "Ken Shirriff")

El Psy Congroo

## 中文 (Chinese)
這是一個由命運石之門中世界綫變動率探測儀所啓發的輝光鈡。在這個中二動畫裏面還有別的很多老舊電子元件的出現。同時，輝光鈡也在別的作品裏面出現過。不過現在已經沒有多少人出於實際目的而去使用這種老派電子原件了。高工作電壓與較大的空間占用是他們的特點。然而，這種真空管卻有人們賦予的物件本身并不會帶有的情感。又或者只是看上去特別中二罷了。

這個設計跟原作挺不一樣。由於小規模生產中的SMT貼片的高成本，最終的PCB設計較大，佈綫較亂，帶有9塊DIP封裝的IC，以及AXIAL封裝的電阻。出於成本及小數點顯示的原因，模組采用8只IN-14。同時，得益於多塊解碼器，信號接口被限制在8個。用於輝光管的170V供電需要外部提供。

其他用到的模組包括提供實時時間與帶有溫度檢測的時鐘模組（DS3231），用於校對時間以及獲取地理位置的GPS模組，和用於變換顯示模式的紅外模組.

運行提供顯示模式包括，當下時間與日期，冲洗（防止陰極中毒），世界綫變動率（命運石之門中的定義），溫度，地理位置等等。

本倉庫包括源文件，以及顯示模組之電路原理圖。

El Psy Congroo

## 日本語 (Japanese)
（日本語のプロではなくて間違う意味があるかもしれないがご注意ください）

こちらのはシュタインズ・ゲートのダイバージェンスメーターに啓発されるニキシー管時計。この作品にはレトロな電気設備いろいろ出る。他の作品にもニキシー管時計が見つかれる。しかし今は実際に使われることはほとんどない。高い電圧が必要なので、大きいである。でも人間は自分の気持ちが感情のないアイテムに。あるいはただの中二病であるかも(笑)。

デザインはよく違う。ＳＭＴの値段の故に、より大きくてレイアウトがちょっと混乱なプリント基板の上にDIPパッケージのICが九つあってAXIALパッケージの抵抗器がある。値段と小数点のデイスプレイのためにIN-14を八つ使ってる。さらに信号ポートは八つある。ニキシー管の電源回路はないので外部からのは必要だ。

他のモジュールは時間や温度や提供する時計モジュール（DS3231）、時間を訂正して経緯度を提供するGPSモジュール、デイスプレイモードを変更する赤外線通信モジュールを含めてる。

プログラムのデイスプレイモードは時間や日付やフラッシュ（カソード中毒の予防）や世界線変動率（シュタインズ・ゲートより）や温度や経緯度やエトセトラ。

このレポジトリーにはソースファイルとデイスプレイモジュールの回路図がある。

エル・プサイ・コングルゥ
