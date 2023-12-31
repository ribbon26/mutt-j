		Mutt 1.14.6用日本語版パッチリリースノート

							2021-11-21
							Ver 2.1.3-j1
							mutt-j 開発チーム

Copyright (C)2000-2021 Oota Toshiya <ribbon@users.osdn.me>

Copyright (C)2000-2001 Kawaguti Ginga <ginga@ginganet.org>

Copyright (C)2000-2004 TAKIZAWA Takashi <taki@cyber.email.ne.jp>

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 

目次

1. Mutt日本語版について

2. オリジナル版との違い
2.1 オリジナル版のバグの修正
2.2.1 カプセル化した転送メッセージの取り扱い
2.2 日本語回りの実用上の問題の修正
2.4 ディストリビューション等からのパッチ取り込み
2.4.1 Fedoraからのパッチの取り込み
2.5 FreeBSD Ports

3. 動作確認環境

4. インストールと設定
4.1 ソースからのインストール
4.2 FreeBSDでのインストール
4.3 CentOSでのRPMインストール
4.3.1 ベースとなるSRPMの入手
4.3.2 ファイルの展開
4.3.3 日本語版パッチ、SPECファイルの展開
4.3.4 コンパイル
4.4 分かっている問題

5. 文字化け対策

6. 関連リンク

7. その他
7.1 著作権等
7.1.1 日本語パッチ
7.1.2 メッセージカタログの邦訳
7.1.3 マニュアルの邦訳
7.1.4 マニュアル以外の文書の邦訳・英訳
7.2 その他
7.2.1 一次配布先
7.2.2 ライセンス
7.2.3 メーリングリスト


━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 

1. Mutt日本語版について

Mutt 日本語版とは、高速に動き、スレッド表示も可能、カスタマイズや
POP/IMAPも利用可能な Mutt を日本語が使えるようにしたバージョンです。本
家の Mutt に対して日本語化パッチを適用することで Mutt 日本語版になりま
す。Mutt 日本語版は、本家のMuttと上位互換の機能を提供します。

Mutt の先の開発系列である mutt-1.3.x シリーズにおいて国際化され日本語
パッチがなくとも日本語のメールが取り扱えるようになりました。しかし、日
本語特有の事情（いわゆる生JIS問題など）があるため、実用上はそのままで
は使いにくいものです。そこで、特に日本語回りでの実用上の各種問題を修正
しほぼ通常利用に差し支えなくし、さらにいくつかの便利な機能を追加したも
のが Mutt 日本語版です。

日本語パッチとしては 1.2.x までのバージョンとは基本的に独立に開発され
ているため、従来あった日本語版独自の機能などは一部省略されて、また新た
に1.3.x 系以降の独自の新機能などが追加されています(日本語化の実装は
1.2.x とは全く別物になっています)。

1.6.1 以降の日本語版パッチは、今までの日本語版パッチを整理し、FreeBSD
ports や Fedora RPM 由来のパッチを一旦削除し、日本語パッチのみの提供と
しました。

1.11.4 以降は、メーリングリストの Subject prefix の削除機能を外しまし
た。また、FreeBSD向けパッチを新たに提供しています。

1.14.6 以降は、rfc_2047_parameters 機能を外しました。このパッチがなく
ても運用上特に問題が発生していないためです。FreeBSD向けパッチも整理
しています。

なお、このドキュメントおよび同梱されているファイルについては、古いまま
のものがあります。ご了承ください。

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 

2. オリジナル版との違い

ここで言うオリジナル版とは、http://www.mutt.org で配布されているソース
パッケージのことを指します。

2.1 オリジナル版のバグの修正

2.2.1 カプセル化した転送メッセージの取り扱い

  mutt-1.5.6 までは日本語版パッチが存在し、file_charsetパラメータを追
加していましたが、mutt-1.5.9からattach_charset パラメータがこの機能を
引き継いだため、日本語版パッチはなくなりました。attach_charsetパラメー
タを設定しない場合には、日本語メッセージを含むメールデータを添付すると、
文字コードがus-asciiと見なされ、文字化けを生じますので注意してください。

ステータス行のマルチバイト文字に対する処理が不完全であるため、これを修
正しました。

スレッド表示や添付ファイル画面で用いられるツリーの罫線(ACS)文字がマル
チバイト文字であるときの処理がオリジナルでは全く行なわれていないため、
その修正を行ないました。

2.2 日本語回りの実用上の問題の修正

オリジナル版でも MIME 関連の RFC に従っている日本語のメッセージは問題
なく取り扱えます。しかし、機種依存文字があると文字化けしたり、非 MIME
生JIS message body,生JIS subject などは表示用の文字符号化方式に変換し
てくれないため読むことができません。そのため、日本語版では、それらのメッ
セージを読むことができる次のような設定オプションを追加しました。詳細は
manual-ja-patch.ja.html および usage-japanese.ja.html をご覧下さい。

 a)  機種依存文字対策
   
     ● sanitize_ja_chars

この機能は sanitize_ja_charsパッチで実現しています。
また、これらの機能を既定値で有効にするため、assumed_charset の
既定値を

"iso-2022-jp:utf-8:euc-jp:shift_jis"

に変更しています。

 b) UCS文字の文字幅対策

UTF-8ロケールにおいてはJIS X 0208の記号・ギリシャ文字・キリル文字は、1
桁幅の文字として扱われます。しかし、mlterm上でUCSフォントでない従来の
フォントを用いた場合にはこれらの文字は2桁幅のJIS X 0208のフォントが使
われます。また、一部の記号については、文字幅が決まっていないものもあり
ます。そのため、不整合が生じ、表示が乱れます。これを防ぐために次の設定
変数

 ● cjk_width
   
を用意し、この値をyesに設定することにより2桁幅の文字として扱うようにな
り、表示が乱れるのを防ぎます。ただし、この設定変数はMutt内蔵のワイド文
字関数を使うときのみ有効(--enable--cjk-ambiguous-widthオプションを
configure時に指定したとき)です。文字幅が決まっていない文字については以
下の文字をパッチ中で定義し、2バイト文字幅として使えるようにしています。

Ex. 0x3000から0x3020の範囲

「　」、「、」、「。」、「〃」、「々」、「〆」、「〇」、「〈」、「〉」、「《」、
「》」、「「」、「」」、「『」、「』」、「【」、「】」、「〒」、「〓」、「〔」、
「〕」、「〝」、「〟」

ただし、mutt でワイド文字関数の処理を行っていても、それだけではうまく
表示されない場合があります。その理由は、画面表示を行うライブラリが、
cjk_widthと同じような処理をしていないからです。詳しくは 5. 文字化け対
策を参照してください。

Content-Type: text/enriched; charset=iso-2022-jp なメッセージでは、根
本的に全くマルチバイト文字に対応していないため、文字化けする可能性があ
ります。しかし、運用で対処できるため、今のところは対応しません。詳細は
usage-japanese.ja.html をご覧下さい。

   
2.4 FreeBSD ports

  1.14.6 でのFreeBSD ports は、Webサイト上に置いています。

　FreeBSD の port では、portsのMakefile自身で、mutt-1.14.6の
contrib配下にあるMakefileを書き換えます。しかし、日本語パッチを適用
した場合、Makefile.amも書き換えるため、再度 autotools関連の処理が走
ります。そのため、書き換えが無効になってしまいます。そこで、FreeBSD
が行うとの同じようなことをするパッチを新たに作っています。

3. 動作確認環境

現在のバージョンでは以下の環境で動作確認をしています。

1) FreeBSD 11.4(32bit)       iw	     Oota Tochiya

オプションの意味

 ● I: C ライブラリに実装されている iconv
 ● i: Bruno 氏の libiconv
 ● W: C ライブラリに実装されているワイド文字関数
 ● w: Mutt 付属のワイド文字関数 (--without-wc-funcs)
 ● s: libslang2ライブラリ

ja-libslangではUTF-8環境において正常な動作をしません。

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 

4. インストールと設定

4.1 ソースからのインストール

インストールに関しては INSTALL.JA-PATCH をお読み下さい。なお、mutt-j
ML のメンバーの手により FreeBSD、NetBSD、Linux(RedHat系、Debian)用のパッ
ケージが作成されています。詳しくは
http://www.emaillab.org/mutt/download15.htmlのページをご覧ください。

また、インストールや設定に関して役に立つ文書へのリンクが http://
www.emaillab.org/mutt/のドキュメントのコーナーにありますので、一度ご覧
ください。

4.2 分かっている問題

　ある特定の条件で文字化けすることがあります。詳しくは 5. を参照して
ください。
　X環境下ではターミナルソフトによって文字が化けることがあります。
openSUSE 42.2 でのテスト結果は以下のとおりです。

1) Xterm         ○
2) QT-Terminal   ×
3) mlterm        ×

5. 文字化け対策

　localeがUTF-8の場合、muttでUnicodeの記号等を表示させようとすると、カー
ソル位置が正常に移動しなかったり、文字が化けたりすることがあります。こ
れは、Unicodeの仕様で、いくつかの文字について ambiguous character
widthという特別な定義がなされているからです。特に、FreeBSDでは顕著に発
生します。
　この問題を解決するためには、現在の所UNICODEテーブルそのものを書き換
える必要があります。UTF-8.txtにその記述方法を記載していますので参照し
てください。　なお、表示文字コードをEUCにしている場合には問題は発生し
ません。その代わり、UNICODEで定義されている文字を表示できなくなります。


6. 関連リンク

 ● Mutt Home Page
   
     ○ http://www.mutt.org/
       
     ○ Mutt 本家のページ
       
 ● Mutt Japanese Edition
   
     ○ http://www.emaillab.org/mutt/
       
     ○ Mutt 日本語版のページ
       

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 

7. その他

7.1 著作権等

Mutt 日本語版は以下に挙げる人だけでなく mutt-j ML メンバの協力により
開発が行なわれています。なお、現在の取りまとめの中心者にはメールアド
レスの後ろに '*'を付けました。バグやおかしい点がありましたら、mutt-j
ML あるいは取りまとめの中心者にメールをお送り下さい。

7.1.1 日本語パッチ

 ・ Copyright (C) 2000-2014 TAKIZAWA Takashi <taki@cyber.email.ne.jp>
   
 ・ Copyright (C) 2000 Yukinori YOSHIDA <yyo@pjs.nevt.nec.co.jp>
   
 ・ Copyright (C) 2000 Kawaguti Ginga <ginga@ginganet.org>
   
 ・ Copyright (C) 2012-2021 Oota Toshiya <ribbon@users.osdn.me> *
   
7.1.2 メッセージカタログの邦訳

 ・ Kikutani Makoto <kikutani@galaxy.net>
   
 ・ Oota Toshiya <ribbon@users.osdn.me>
   
 ・ "TAKAHASHI、Tamotsu" <tamo@momonga-linux.org> *
   
7.1.3 マニュアルの邦訳

 ・ Takuhiro Nishioka <takuhiro@super.win.ne.jp>
   
 ・ Kohtaro Hitomi <hitomi@hiei.kit.ac.jp>

 ・ Oota Toshiya <ribbon@users.osdn.me> *
   
7.1.4 マニュアル以外の文書の邦訳・英訳

 ・ TAKIZAWA Takashi <taki@cyber.email.ne.jp>
   
 ・ "TAKAHASHI、Tamotsu" <tamo@momonga-linux.org>
   
 ・ Kawaguti Ginga <ginga@ginganet.org>
   
 ・ SHIOZAKI Takehiko <takehiko.shiozaki@reuters.co.jp>

7.2 その他

日本語パッチの付属文書の DocBook/XML 化は SHINGUU Atsuhiro
<icebell@fujii-kiso.co.jp> によって行われました。
また、1.5.21において、FreeBSD portsから日本語版portsへの移植は、
力武健次氏(kenji.rikitake@acm.org)によるものです。ご協力に感謝
します。

7.2.1 一次配布先

日本語化パッチの一次配布先は https://osdn.net/projects/mutt-j/ です。

7.2.2 ライセンス

日本語化パッチはオリジナルの Mutt と同様に無保証であり、Free
Software Foundation の GNU General Public License Version 2 に従って
配布および修正を行うことができます。GNU General Public License
Version 2 に関してはオリジナルのMuttのパッケージに含まれている GPL を
お読み下さい。

7.2.3 メーリングリスト

本家のメーリングリストは http://www.mutt.org/ をご覧ください。

日本語でのやりとりを行う mutt-j ML は osdn.me 上に説明がありますのでそ
ちらを参照してください。
