/*
 * Copyright (C) 1996-2002,2007,2010,2012-2013,2016 Michael R. Elkins <me@mutt.org>
 * Copyright (C) 2004 g10 Code GmbH
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef _MAKEDOC
# include "config.h"
# include "doc/makedoc-defs.h"
#else
# include "sort.h"
#endif

#include "buffy.h"

#ifndef _MAKEDOC
/* If you add a data type, be sure to update doc/makedoc.c */
#define DT_MASK		0x0f
#define DT_BOOL		1 /* boolean option */
#define DT_NUM		2 /* a number (short) */
#define DT_STR		3 /* a string */
#define DT_PATH		4 /* a pathname */
#define DT_QUAD		5 /* quad-option (yes/no/ask-yes/ask-no) */
#define DT_SORT		6 /* sorting methods */
#define DT_RX		7 /* regular expressions */
#define DT_MAGIC	8 /* mailbox type */
#define DT_SYN		9 /* synonym for another variable */
#define DT_ADDR	       10 /* e-mail address */
#define DT_MBCHARTBL   11 /* multibyte char table */
#define DT_LNUM        12 /* a number (long) */

#define DTYPE(x) ((x) & DT_MASK)

/* subtypes */
#define DT_SUBTYPE_MASK	0xff0
#define DT_SORT_ALIAS	0x10
#define DT_SORT_BROWSER 0x20
#define DT_SORT_KEYS	0x40
#define DT_SORT_AUX	0x80
#define DT_SORT_SIDEBAR	0x100

/* flags to parse_set() */
#define MUTT_SET_INV	(1<<0)	/* default is to invert all vars */
#define MUTT_SET_UNSET	(1<<1)	/* default is to unset all vars */
#define MUTT_SET_RESET	(1<<2)	/* default is to reset all vars to default */

/* forced redraw/resort types */
#define R_NONE		0
#define R_INDEX		(1<<0)  /* redraw the index menu (MENU_MAIN) */
#define R_PAGER		(1<<1)  /* redraw the pager menu */
#define R_PAGER_FLOW    (1<<2)  /* reflow lineInfo and redraw the pager menu */
#define R_RESORT	(1<<3)	/* resort the mailbox */
#define R_RESORT_SUB	(1<<4)	/* resort subthreads */
#define R_RESORT_INIT	(1<<5)  /* resort from scratch */
#define R_TREE		(1<<6)  /* redraw the thread tree */
#define R_REFLOW        (1<<7)  /* reflow window layout and full redraw */
#define R_SIDEBAR       (1<<8)  /* redraw the sidebar */
#define R_MENU          (1<<9)  /* redraw all menus */
#define R_BOTH		(R_INDEX | R_PAGER)
#define R_RESORT_BOTH	(R_RESORT | R_RESORT_SUB)

struct option_t
{
  char *option;
  short type;
  short flags;
  union pointer_long_t data;
  union pointer_long_t init; /* initial value */
};
#endif /* _MAKEDOC */

#ifndef ISPELL
#define ISPELL "ispell"
#endif

struct option_t MuttVars[] = {
  /*++*/
  { "abort_noattach", DT_QUAD, R_NONE, {.l=OPT_ABORTNOATTACH}, {.l=MUTT_NO} },
  /*
  ** .pp
  ** メッセージの本体が $$abort_noattach_regexp に一致し、かつ、添付がない場合、
  ** この4択は目セージ送信を中止するかどうかを制御します。
  */
  { "abort_noattach_regexp",  DT_RX,  R_NONE, {.p=&AbortNoattachRegexp}, {.p="attach"} },
  /*
  ** .pp
  ** メッセージの本体に対して一致する正規表現を指定し、添付ファイルが言及されたが
  ** 間違って忘れられたかを決定します。一致した場合、 $$abort_noattach は、
  ** メッセージ送信を中止するかを決めるために考慮します。
  ** .pp
  ** Mutt 中の他の正規表現と同じように、パターンの中に少なくとも1つの大文字が
  ** ある場合は大文字小文字を区別し、それ以外は大文字小文字を区別しません。
  */
  { "abort_nosubject",	DT_QUAD, R_NONE, {.l=OPT_SUBJECT}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** \fIyes\fP に設定した場合、メッセージを編集中の、題名プロンプトに
  ** 題名を指定しない場合、編集が中止されます。\fIno\fP に設定した場合は、
  ** 題名プロンプトに題名が指定されなくても中止しません。
  */
  { "abort_unmodified",	DT_QUAD, R_NONE, {.l=OPT_ABORT}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** \fIyes\fP に設定すると、メッセージ本体の編集後に、ファイルへの変更がなければ
  ** 自動的に編集を中止します(これは、そのファイルの\fI最初の\fP編集後にのみ
  ** 発生します)。\fIno\fP に設定すると、編集は決して中止しません。
  */
  { "alias_file",	DT_PATH, R_NONE, {.p=&AliasFile}, {.p="~/.muttrc"} },
  /*
  ** .pp
  ** \fC$<create-alias>\fP 機能で作成された別名を保存するための既定のファイル。
  ** このファイルに追加されるエントリは \fIset\fP が設定されている場合、
  ** $$config_charset によって指定された文字セットで、そうでない
  ** 場合は現在の文字セットでエンコードされます。
  ** .pp
  ** \fBNote:\fP Mutt は自動的にこのファイルを読み込ません。このオプションが
  ** 専用の別名ファイルを指定している場合、``$source'' コマンドを明示的に
  ** 実行する必要があります。
  ** .pp
  ** このオプションの既定値は、現在使っている muttrc ファイルか、ユーザの
  ** muttrc ファイルがない場合は ``~/.muttrc'' となります。
  */
  { "alias_format",	DT_STR,  R_NONE, {.p=&AliasFmt}, {.p="%4n %2f %t %-10a   %r"} },
  /*
  ** .pp
  ** ``$alias'' メニューで表示されるデータの書式を指定します。以下の、
  ** \fCprintf(3)\fP 形式のものが有効です。
  ** .dl
  ** .dt %a .dd 別名
  ** .dt %f .dd フラグ - 現時点では削除予定の別名に ``d'' が付くだけ
  ** .dt %n .dd インデックス番号
  ** .dt %r .dd 別名展開後のアドレス
  ** .dt %t .dd 別名にタグが付いている場合を示す文字
  ** .de
  */
  { "allow_8bit",	DT_BOOL, R_NONE, {.l=OPTALLOW8BIT}, {.l=1} },
  /*
  ** .pp
  ** メール送信時に Quoted-Printable か Base64 を使って 8 ビットデータを
  **  7 ビットに変換するかどうかを制御します。
  */
  { "allow_ansi",      DT_BOOL, R_NONE, {.l=OPTALLOWANSI}, {.l=0} },
  /*
  ** .pp
  ** メッセージ中の ANSI 色コード (と、リッチテキストメッセージの色タグ) を解釈するか
  ** どうかを制御します。こうしたコードを含むメッセージは稀ですが、 このオプションが
  ** \fIset\fP されていれば、相応に着色されます。これは、自分の色選択を上書きしかねない
  ** という点や、セキュリティ上の問題さえ引き起こしかねないという点に注意する必要が
  ** あります。たとえば、以下のような行がメッセージに含まれているとします。
  ** .ts
  ** [-- PGP output follows ...
  ** .te
  ** .pp
  ** そして、添付の色と同じ色になっている場合です($$crypt_timestampを参照してください)。
  */
  { "arrow_cursor",	DT_BOOL, R_MENU, {.l=OPTARROWCURSOR}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fPされている場合、行全体をハイライトする代わりに、矢印(``->'') で、
  ** メニュー中での現在のエントリ位置を表示します。これを使うことで、遅いネットワークや
  ** モデム回線で、メニュー中で、前後のエントリに移動するとき、画面の再描画量が
  ** 少なくて済むという理由で、反応を早くすることが出来ます。
  */
  { "ascii_chars",	DT_BOOL, R_BOTH, {.l=OPTASCIICHARS}, {.l=0} },
  /*
  ** .pp
  ** fIset\fPした場合、Mutt はスレッドや添付ファイルの木構造を表示する際に、既定の
  ** fIACS\fP 文字ではなくプレーン ASCII 文字を使います。
  */
  { "askbcc",		DT_BOOL, R_NONE, {.l=OPTASKBCC}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fPされていると、送信メッセージ編集前にブラインドカーボンコピー (Bcc) の
  ** 宛先を入力するよう Mutt が問合せしてきます。
  */
  { "askcc",		DT_BOOL, R_NONE, {.l=OPTASKCC}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fPされていると、Mutt は送信メッセージの編集前にカーボンコピー (Cc) の
  ** 宛先を問合せしてきます。
  */
  { "assumed_charset", DT_STR, R_NONE, {.p=&AssumedCharset}, {.p=0} },
  /*
  ** .pp
  ** この変数は、文字エンコーディングの指定がないメッセージに対するエンコーディング
  ** 方式の一覧をコロンで区切ったものです。文字エンコーディングの指定がない、ヘッダ
  ** フィールドの値とメッセージ本体の内容は、このリストのどれかで記述されていると
  ** 仮定されます。既定では、文字セットの指定がない、すべてのヘッダフィールドと
  ** メッセージ本体は ``us-ascii'' であると見なされます。
  ** .pp
  ** たとえば、日本のユーザは以下のようにするのがよいかもしれません:
  ** .ts
  ** set assumed_charset="iso-2022-jp:euc-jp:shift_jis:utf-8"
  ** .te
  ** .pp
  ** しかし、メッセージ本体に対しては、最初のもののみが有効です。
  */
  { "attach_charset",    DT_STR,  R_NONE, {.p=&AttachCharset}, {.p=0} },
  /*
  ** .pp
  ** この変数は、テキストファイルの添付に対する、コロンで区切られた、文字
  ** エンコーディング方式の一覧です。Mutt は $$send_charset で指定された適切な
  ** 文字セットに変換するため、添付されているエンコードされたファイルのエンコード
  ** 状態を推測するために、これを使います。
  ** .pp
  ** もし、 \fIunset\fPであれば、 $$charset の値がそのかわりに使われます。
  ** たとえば、以下の設定で、日本語のテキスト処理が動きます。
  ** .ts
  ** set attach_charset="iso-2022-jp:euc-jp:shift_jis:utf-8"
  ** .te
  ** .pp
  ** 日本のユーザ向けには、``iso-2022-*'' を含める場合には、上記のように値の
  ** 先頭に配置する必要があります。
  */
  { "attach_format",	DT_STR,  R_NONE, {.p=&AttachFormat}, {.p="%u%D%I %t%4n %T%.40d%> [%.7m/%.10M, %.6e%?C?, %C?, %s] "} },
  /*
  ** .pp
  ** この値は  ``添付'' メニューでのフォーマットを記述します。以下の
  ** \fCprintf(3)\fP 形式が指定できます。
  ** .dl
  ** .dt %C  .dd 文字セット
  ** .dt %c  .dd 文字セット変換が必要 (``n'' 又は ``c'')
  ** .dt %D  .dd 削除予定フラグ
  ** .dt %d  .dd 説明 (ない場合には%F が使われます)
  ** .dt %e  .dd MIME content-transfer-encoding
  ** .dt %F  .dd content-disposition ヘッダ中のファイル名r (ない場合は %f が使われます)
  ** .dt %f  .dd ファイル名
  ** .dt %I  .dd disposition (インラインなら ``I'' 、 添付なら ``A'')
  ** .dt %m  .dd メジャー MIME タイプ
  ** .dt %M  .dd MIME サブタイプ
  ** .dt %n  .dd 添付ファイル番号
  ** .dt %Q  .dd MIME パートが添付ファイルカウントの対象になる場合  ``Q''
  ** .dt %s  .dd サイズ ($formatstrings-size を参照)
  ** .dt %t  .dd タグフラグ
  ** .dt %T  .dd 木構造を表示するグラフィカルな文字
  ** .dt %u  .dd unlink (=削除対象) フラグ
  ** .dt %X  .dd このパート中の、修飾 MIME パートの数とその子
  **             (取り得るスピード効果については ``$attachments'' 節を参照してください)
  ** .dt %>X .dd 文字列の残りを右寄せし、文字 ``X'' で埋めます
  ** .dt %|X .dd 文字列の最後まで文字 ``X'' で埋めます
  ** .dt %*X .dd 文字 ``X'' を埋め草として soft-fill します
  ** .de
  ** .pp
  ** ``soft-fill'' の説明については $$index_format のドキュメントを参照してください。
  */
  { "attach_sep",	DT_STR,	 R_NONE, {.p=&AttachSep}, {.p="\n"} },
  /*
  ** .pp
  ** タグが付いた添付の一覧を操作するとき(セーブ、印刷、パイプなど)
  ** 添付間に追加するためのセパレータ。
  */
  { "attach_split",	DT_BOOL, R_NONE, {.l=OPTATTACHSPLIT}, {.l=1} },
  /*
  ** .pp
  ** この変数が\fIunset\fPで、タグが付いた添付の一覧を操作するとき(セーブ、印刷、
  ** パイプなど)、Mutt は添付を結合し、1つの添付として処理します。$$attach_sep
  ** セパレータが各添付の後に追加されます。\fIset\fPの場合には、Mutt は
  ** 添付を1つ1つ処理します。
  */
  { "attribution",	DT_STR,	 R_NONE, {.p=&Attribution}, {.p="On %d, %n wrote:"} },
  /*
  ** .pp
  ** これは、返信で引用されるメッセージの前に付く文字列です。定義されている\fCprintf(3)\fP
  ** 風シーケンスの完全な一覧は $$index_format の節にあります。
  */
  { "attribution_locale", DT_STR, R_NONE, {.p=&AttributionLocale}, {.p=0} },
  /*
  ** .pp
  ** $attribution 文字列中で、日付をフォーマットするために\fCstrftime(3)\fP
  ** で使われるロケール。正しい値は、システムが使うロケール環境変数
  **  \fC$$$LC_TIME\fP のものになります。
  ** .pp
  ** この値は、フックで使う、受信者又フォルダでカスタマイズされる
  ** 日付フォーマット属性を許可するためのものです。既定では、Mutt は
  ** 使っているロケール環境をを使うので、既定値を上書きする場合以外では、
  ** これを設定する必要はありません。
  */
  { "auto_subscribe",	DT_BOOL, R_NONE, {.l=OPTAUTOSUBSCRIBE}, {.l=0} },
  /*
  ** .pp
  **  \fIset\fPの場合、Mutt は List-Post ヘッダの存在が、受信者がメーリングリストに
  ** 登録されていることを意味すると仮定します。メーリングリストが
  ** ``unsubscribe'' 又は ``unlist'' リスト中にいない限り、``$subscribe''
  ** リストに追加されます。これらを操作し、解釈するとヘッダを読み込む時間が遅くなるので、
  ** 既定ではこのオプションは無効になっています。
  */
  { "auto_tag",		DT_BOOL, R_NONE, {.l=OPTAUTOTAG}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fPの場合、メッセージに影響する\fIindex\fPメニュー中の機能は
  ** (もしもあれば)すべてのタグ付きメッセージに適用されます。設定しない場合、
  ** 最初に \fC<tag-prefix>\fP機能(既定で``;''に割り当てられています)を使い、
  ** すべてのタグ付きメッセージに次の機能を実行させるようにしなければなりません。
  */
#ifdef USE_AUTOCRYPT
  { "autocrypt",	DT_BOOL, R_NONE, {.l=OPTAUTOCRYPT}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fPの場合で、autocrypt を有効にし、ヘッダ経由で交換されるキーを
  ** 使って受動的な暗号化による保護を提供します。
  ** 詳細は``$autocryptdoc''を参照してください。
  ** (これは Autocrypt のみです)
  */
  { "autocrypt_acct_format", DT_STR, R_MENU, {.p=&AutocryptAcctFormat}, {.p="%4n %-30a %20p %10s"} },
  /*
  ** .pp
  ** この変数は、``autocrypt account''メニューのフォーマットを決めます。
  ** 以下の、\fCprintf(3)\fP風のものが使えます。
  ** .dl
  ** .dt %a  .dd メールアドレス
  ** .dt %k  .dd gpg keyid
  ** .dt %n  .dd 現在のエントリ番号
  ** .dt %p  .dd prefer-encrypt フラグ
  ** .dt %s  .dd ステータスフラグ (active/inactive)
  ** .de
  ** .pp
  ** (Autocrypt のみです)
  */
  { "autocrypt_dir",	DT_PATH, R_NONE, {.p=&AutocryptDir}, {.p="~/.mutt/autocrypt"} },
  /*
  ** .pp
  ** この変数は、GPG キーリングと sqlite データベースを含む、autocrypt ファイルが
  ** どこに格納されるかを設定します。詳細は``$autocryptdoc''を参照してください。
  ** (Autocrypt のみです)
  */
  { "autocrypt_reply",	DT_BOOL, R_NONE, {.l=OPTAUTOCRYPTREPLY}, {.l=1} },
  /*
  ** .pp
  ** fIset\fPの場合、autocrypt メールに対して返信すると、自動的に返信中で
  ** autocrypt を有効にします。通常の web-ofstrust で autocrypt に同じキーを使いたい
  ** 場合、これを無効にしたいかもしれません。このため、autocrypt はすべての暗号化した
  ** 返信に対して強制的に有効にはなりません。
  ** (Autocrypt のみです)
  */
#endif
  { "autoedit",		DT_BOOL, R_NONE, {.l=OPTAUTOEDIT}, {.l=0} },
  /*
  ** .pp
  ** $$edit_headers といっしょに \fIset\fPとした場合、Mutt は最初の送信メニュー
  ** (題名と受信者の要求)を飛ばして、すぐにメッセージ本文を編集し始めるようになります。
  ** 送信メニューは、メッセージの本文を編集した後に、引き続きアクセス出来ます。
  ** .pp
  ** .pp
  ** \fBNote:\fP このオプションを\fIset\fPにした場合、初期受信者リストが空なので
  ** 新しい(返信でない)メッセージを編集するとき、受信者に依存する send-hooks を使う事が
  ** 出来ないことに注意してください。
  ** .pp
  ** $$fast_reply も参照してください。
  */
  { "beep",		DT_BOOL, R_NONE, {.l=OPTBEEP}, {.l=1} },
  /*
  ** .pp
  ** この変数が\fIset\fPの時、エラー発生時に Mutt が音を鳴らします。
  */
  { "beep_new",		DT_BOOL, R_NONE, {.l=OPTBEEPNEW}, {.l=0} },
  /*
  ** .pp
  ** この変数が \fIset\fP の時、mutt は新着メールを知らせるメッセージを表示するたびに
  ** 音を鳴らします。これは$$beep 変数の設定とは独立しています。
  */
  { "bounce",	DT_QUAD, R_NONE, {.l=OPT_BOUNCE}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** メッセージ中継時に、確認のための質問をするかどうかを制御します。これを
  ** \fIyes\fPにした場合、本当に中継するかどうかを聞いてきません。\fIno\fPに
  ** することは一般的に無益であり、メッセージの中継が出来なくなるため推奨しません。
  */
  { "bounce_delivered", DT_BOOL, R_NONE, {.l=OPTBOUNCEDELIVERED}, {.l=1} },
  /*
  ** .pp
  ** この変数を\fIset\fPにした場合、mutt はメッセージ中継を行うときに、
  ** Delivered-To ヘッダを付与します。Postfix の利用者は、この変数が\fIunset\fP
  ** にしたいでしょう。
  */
  { "braille_friendly", DT_BOOL, R_NONE, {.l=OPTBRAILLEFRIENDLY}, {.l=0} },
  /*
  ** .pp
  ** この変数が、\fIset\fPの時、mutt は$$arrow_cursor 変数が\fIunset\fPであっても、
  ** 目の不自由な人が点字ディスプレイを使ってメニューを追えるように、カーソルを
  ** メニューの先頭に置きます。このオプションは、多くの画面端末がカーソルを非表示に
  ** 出来ないため、既定で\fIunset\fP になっています。
  */
  { "browser_abbreviate_mailboxes", DT_BOOL, R_NONE, {.l=OPTBROWSERABBRMAILBOXES}, {.l=1} },
  /*
  ** .pp
  ** この変数が\fIset\fPの場合、mutt は  '~' と '=' ショートカットを使って、
  ** ブラウザメールボックスリスト中で、メールボックス名を省略形で表示します。
  ** .pp
  ** $$sort_browser での既定の\fC"alpha"\fP 設定は、いくつかの区切り文字を
  ** 無視する(\fCstrcoll(3)\fPを使う)ロケールベースの整列を使います。これにより
  ** 整列順が直感的でない場合が発生する可能性があります。そのような場合、
  ** この変数を\fIunset\fPにした方がよいでしょう。
  */
  { "browser_sticky_cursor", DT_BOOL, R_NONE, {.l=OPTBROWSERSTICKYCURSOR}, {.l=1} },
  /*
  ** .pp
  ** この変数を \fIset\fPにした場合、ブラウザは、種々の機能を実行するときに
  ** 同じメールボックス上でカーソルを保持するように試みます。これには、
  ** ディレクトリ上位への移動、メールボックスとディレクトリ一覧の切り替え、
  ** メールボックスの作成/削除、見ているメールボックスの切り換えと
  ** 新しいマスクの入力が含まれます。
  */
#if defined(USE_SSL)
  { "certificate_file",	DT_PATH, R_NONE, {.p=&SslCertFile}, {.p="~/.mutt_certificates"} },
  /*
  ** .pp
  ** この変数は、信頼している証明書が保存されているファイルを指定します。
  ** 不明な証明書が見つかった場合、それを受け入れるか否かが聞かれます。
  ** 受け入れる場合、証明者はこのファイルに格納され、その後の接続は自動的に
  ** 許可されます。
  ** .pp
  ** このファイルに CA 証明書を手動で追加することも出来ます。CA 証明書の1つを
  ** 持っている署名された任意のサーバ証明書も自動的に許可されます。
  ** .pp
  ** 例:
  ** .ts
  ** set certificate_file=~/.mutt/certificates
  ** .te
  **
  */
#endif
  { "change_folder_next", DT_BOOL, R_NONE, {.l=OPTCHANGEFOLDERNEXT}, {.l=0} },
  /*
  ** .pp
  ** この変数を\fIset\fPにした場合、\fC<change-folder>\fP機能のメールボックス候補は
  ** リスト中の最初のフォルダから始まるのではなく、``$mailboxes'' リスト中の
  ** 次のフォルダから始まります。
  */
  { "charset",		DT_STR,	 R_NONE, {.p=&Charset}, {.p=0} },
  /*
  ** .pp
  ** テキストデータを端末で表示したり入力したりするのに使う文字セットです。
  ** $$send_charset でも使います。
  ** .pp
  ** 起動時に、Mutt は、\fC$$$LC_CTYPE\fP や \fC$$$LANG\fP のような環境変数から
  ** この値を利用しようとします。
  ** .pp
  ** \fBNote:\fP Mutt が使用する文字セットを正しく決められない可能性がある場合にのみ
  ** これを設定すべきです。
  */
  { "check_mbox_size",	DT_BOOL, R_NONE, {.l=OPTCHECKMBOXSIZE}, {.l=0} },
  /*
  ** .pp
  ** 変数を\fIset\fPにした場合、mutt は mbox と mmdf フォルダの新規メールチェック時に
  ** アクセスタイムの代わりにファイルサイズ属性を使います。
  ** .pp
  ** この変数は既定値で\fIunset\fPで、これは、新規メールの検出が、前述のフォルダ
  ** タイプで信頼できないか動かない場合にのみ有効にすべきです。
  ** .pp
  ** muttは、定義されている場合、高速メールボックススキャンを実行することにより、
  ** そのようなメールボックスの初期新規メールステータスを決定するために必要とするため、
  ** mbox 又は mmdfフォルダに対して、設定ファイル中で``$mailboxes'' ディレクティブが
  ** 出てくる前に、この変数を有効にする必要があることに注意が必要です。
  ** その後、新規メールステータスはファイルサイズ変更で検出されます。
  */
  { "check_new",	DT_BOOL, R_NONE, {.l=OPTCHECKNEW}, {.l=1} },
  /*
  ** .pp
  ** \fBNote:\fP このオプションは\fImaildir\fP と \fIMH\fP 形式のメールボックス
  ** のみ有効です。
  ** .pp
  ** \fIset\fP の場合、Mutt は開いている最中のメールボックスに着信する新着メールを
  ** 調べます。特に、MH メールボックスにおいては、この操作は、ディレクトリをスキャンし、
  ** 各ファイルについてすでに見たかどうかをチェックするため、かなりの時間がかかる
  ** 可能性があります。この変数を\fIunset\fPにすると、メールボックスが開いている間は
  ** 新規メールのチェックを行いません。
  ** 
  */
  { "collapse_unread",	DT_BOOL, R_NONE, {.l=OPTCOLLAPSEUNREAD}, {.l=1} },
  /*
  ** .pp
  ** \fIunset\fP の場合、Mutt は未読メッセージがあるスレッドを折りたたみません。
  */
  { "compose_format",	DT_STR,	 R_MENU, {.p=&ComposeFormat}, {.p="-- Mutt: Compose  [Approx. msg size: %l   Atts: %a]%>-"} },
  /*
  ** .pp
  ** Controls the format of the status line displayed in the ``compose''
  ** menu.  This string is similar to $$status_format, but has its own
  ** set of \fCprintf(3)\fP-like sequences:
  ** ``編集'' メニューで表示されているステータス行のフォーマットを制御します。
  ** この文字列は$$status_formatと似ていますが、\fCprintf(3)\fP風の、固有の
  ** パラメータを使います:
  ** .dl
  ** .dt %a .dd 添付の総数
  ** .dt %h .dd ローカルホスト名
  ** .dt %l .dd 現在のメッセージの(バイト単位での)おおよそのサイズ($formatstrings-sizeを参照)
  ** .dt %v .dd Mutt バージョン文字列
  ** .de
  ** .pp
  ** どのように$$compose_format. を設定したらいいかについての情報は、
  ** $$status_formatオプションの記述を参照してください。
  */
  { "config_charset",	DT_STR,  R_NONE, {.p=&ConfigCharset}, {.p=0} },
  /*
  ** .pp
  ** 定義した場合、Mutt は このエンコーディングから$$charsetによって指定された
  ** 現在の文字セットに、rc ファイル中のコマンドを再コーディングし、
  ** 現在の文字セットから $$alias_file に、別名を書き込みます。
  ** .pp
  ** $$charset が設定されている場合、$$config_charset の前に置かなければ
  ** ならないことに注意してください。
  ** .pp
  ** 再エンコーディングは、変換できない文字を疑問符にしてしまう場合があり、
  ** 予期せぬ副作用(たとえば正規表現で)が発生しうるので、避けるべきです。
  ** 
  */
  { "confirmappend",	DT_BOOL, R_NONE, {.l=OPTCONFIRMAPPEND}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fPの場合、Mutt がメッセージを既存メールボックスに追加するときに確認をしてきます。
  */
  { "confirmcreate",	DT_BOOL, R_NONE, {.l=OPTCONFIRMCREATE}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fPの場合、Mutt は存在しないメールボックスにメッセージを保存する際、
  ** メールボックスを作成する前に確認をしてきます。
  */
  { "connect_timeout",	DT_NUM,	R_NONE, {.p=&ConnectTimeout}, {.l=30} },
  /*
  ** .pp
  ** この秒数でネットワーク接続 (IMAP、POPとSMP) が確立できなければ Mutt はタイムアウトと
  ** します。負の数の場合、接続が成功するまで永遠に待ち続けます。
  */
  { "content_type",	DT_STR, R_NONE, {.p=&ContentType}, {.p="text/plain"} },
  /*
  ** .pp
  ** 新規作成メッセージ本文における既定の Content-Type を設定します。
  */
  { "copy",		DT_QUAD, R_NONE, {.l=OPT_COPY}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** $$save_name, $$force_name and ``$fcc-hook''.
  ** この変数は、送信メッセージのコピーを 後で参照するために保存しておくかどうかを制御します。
  ** $$record,$$save_name, $$force_name と ``$fcc-hook'' も参照してください。
  */
  { "count_alternatives", DT_BOOL, R_NONE, {.l=OPTCOUNTALTERNATIVES}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は添付の検索とカウント($attachmentsを参照)を実行するときに、
  ** multipart/alternatives内を再帰処理します。
  ** .pp
  ** 従来、 multipart/alternative パートは、メールの主な部分とは異なったエンコーディング
  ** を単に表現するものでした。残念ながらいくつかのメールクライアントはalternative の
  ** 1つの中にメールの添付を置くようになってきています。これを設定すると、Mutt は
  ** そこに隠された、一致する添付を見つけ、カウントし、%X 経由か ~X によるパターン
  ** マッチングを使ってインデックス中に含めます。
  */
  { "pgp_autoencrypt",		DT_SYN,  R_NONE, {.p="crypt_autoencrypt"}, {.p=0} },
  { "crypt_autoencrypt",	DT_BOOL, R_NONE, {.l=OPTCRYPTAUTOENCRYPT}, {.l=0} },
  /*
  ** .pp
  ** この変数を設定すると、Mutt は送信メッセージを常時 PGP で暗号化しようとします。
  ** これは``$send-hook'' コマンドでの接続中にのみ便利です。これは、暗号化が
  ** 不要な場合や同様に署名が要求されている場合は PGPメニューを使う事で上書きできます。
  ** $$smime_is_default が\fIset\fP の時は、 S/MIME メッセージを作成するのに
  ** OpenSSL が代わりに使われ、smime メニューを使うことで代わりに設定を上書きできます。
  ** (Crypto のみです)
  */
  { "crypt_autopgp",	DT_BOOL, R_NONE, {.l=OPTCRYPTAUTOPGP}, {.l=1} },
  /*
  ** .pp
  ** この変数は、Mutt がメッセージに自動で PGP 暗号化/署名を有効にしても 良いかどうかを制御
  ** します。$$crypt_autoencrypt,$$crypt_replyencrypt,$$crypt_autosign, $$crypt_replysign と
  ** $$smime_is_default も参照してください。
  */
  { "pgp_autosign", 	DT_SYN,  R_NONE, {.p="crypt_autosign"}, {.p=0} },
  { "crypt_autosign",	DT_BOOL, R_NONE, {.l=OPTCRYPTAUTOSIGN}, {.l=0} },
  /*
  ** .pp
  ** この変数を設定すると、Mutt は送信メッセージを常時暗号で署名しようとします。
  ** これは、署名が不要な場合や、同様に暗号化が必要な場合に pgp メニューを使うことで
  ** 上書きできます。$$smime_is_default が \fIset\fP の場合は、S/MIME メッセージを
  ** 作成するのにOpenSSL が代わりに使われ、pgp メニューの代わりに smime メニューを
  ** 代わりに使う事で上書きできます。
  ** (Cryptoのみです)
  */
  { "crypt_autosmime",	DT_BOOL, R_NONE, {.l=OPTCRYPTAUTOSMIME}, {.l=1} },
  /*
  ** .pp
  ** この変数は、Mutt がメッセージに対して自動的に S/MiME 暗号化/署名を有効にするかを
  ** 指定します。$$crypt_autoencrypt,$$crypt_replyencrypt,$$crypt_autosign,
  ** $$crypt_replysign と $$smime_is_default も参照してください。 
  */
  { "crypt_confirmhook",	DT_BOOL, R_NONE, {.l=OPTCRYPTCONFIRMHOOK}, {.l=1} },
  /*
  ** .pp
  ** 設定した場合、\fIcrypt-hook\fP コマンドを使うときにキーの確認を求めるように
  ** なります。設定しない場合、確認は求められません。これは、タイプミスが考えられる
  ** 場合には、一般的に安全ではないと考えられます。
  */
  { "crypt_opportunistic_encrypt", DT_BOOL, R_NONE, {.l=OPTCRYPTOPPORTUNISTICENCRYPT}, {.l=0} },
  /*
  ** .pp
  ** この変数を設定すると、Mutt によってすべてのメッセージ受信者のキーが
  ** 見つけられるかによって、Mutt は暗号化を自動的に有効または無効にします。
  ** .pp
  ** このオプションが有効な場合、Mutt は TO, CC, と BCC の一覧が編集されるごとに
  ** 暗号化を有効/無効にします。$$edit_headers が設定されている場合、Mutt は
  ** メッセージが編集される毎にも行います。
  ** .pp
  ** これが設定されている場合は、暗号化は手動で有効/無効に出来ません。pgp 又は smime
  ** メニューは、現在のメッセージに対してこのオプションを一時的に無効にするための
  ** 選択を提供します。
  ** .pp
  ** $$crypt_autoencrypt 又は $$crypt_replyencrypt がメッセージに対して暗号化を
  ** 有効にしている場合、このオプションはメッセージに対しては無効となります。
  ** pgp 又は smime メニューで手動で再度有効に出来ます。
  ** (Cryptoのみです)
   */
  { "crypt_protected_headers_read", DT_BOOL, R_NONE, {.l=OPTCRYPTPROTHDRSREAD}, {.l=1} },
  /*
  ** .pp
  ** 設定した場合、Mutt はページャ中に保護されたヘッダ("メモリホール")を表示し、
  ** インデックスとヘッダキャッシュを変更されたヘッダで更新します。
  **
  ** 保護されたヘッダはメール中の暗号化または署名されたパート中に格納され、
  ** 漏洩や改ざんを防ぎます。
  ** 詳細については https://github.com/autocrypt/memoryhole を参照してください。
  ** 現時点で、Mutt は題名ヘッダのみをサポートします。
  ** .pp
  ** 保護されたヘッダを使う暗号化メッセージは、しばしば、表示されている題名
  ** ヘッダをダミー値を含むものに差し替えます($crypt_protected_headers_subjectを
  ** 参照)。Mutt はメッセージを開いた\fB後\fP、すなわち、\fC<display-message>\fP 機能を
  ** 使って、正しい題名の内容に差し替えます。開く前にメッセージに返信する場合は、
  ** Mutt は結局ダミーの題名ヘッダを使うため、そのようなメッセージは最初に
  ** オープンするようにしてください。
  ** (Cryptoのみです)
   */
  { "crypt_protected_headers_save", DT_BOOL, R_NONE, {.l=OPTCRYPTPROTHDRSSAVE}, {.l=0} },
  /*
  ** .pp
  ** $$crypt_protected_headers_read を設定した場合で、保護された題名があるメッセージを
  ** 開いている場合、Mutt は既定でヘッダキャッシュに更新された題名を保存します。
  ** これにより、メッセージを毎回再オープンすることなしに、メールボックスが
  ** 再オープンされた場合に保護された題名ヘッダをベースにした検索/制限が
  ** 出来るようになります。しかし、mbox/mh メールボックスか、ヘッダキャッシュが
  ** 設定されていない場合、再度保護された題名を表示あるいは検索/制限する前に
  ** メールボックスが再度開かれる時はメッセージを毎回再オープンする必要があります。
  ** .pp
  ** この変数が設定されている場合、Mutt は保護された題名を、
  ** \fB平文テキストメッセージヘッダ中に\fP追加で保存します。これにより
  ** 使い勝手が向上しますが、セキュリティが低下することとのトレードオフになります。
  ** 以前に暗号化された可能性のある保護された題名ヘッダは現在メッセージヘッダに
  ** 平文で格納されるようになりました。Mutt や外部ツールでメッセージをどこかにコピー
  ** する場合、以前に暗号化されたデータが見えてしまう可能性があります。
  ** この変数を有効にする前に、これがどういう結果になるかを十分に理解してください。
  ** (Crypto のみです)
   */
  { "crypt_protected_headers_subject", DT_STR, R_NONE, {.p=&ProtHdrSubject}, {.p="Encrypted subject"} },
  /*
  ** .pp
  ** When $$crypt_protected_headers_write を設定した場合で、メッセージに暗号化がマークされて
  ** いる場合、これがメッセージヘッダの題名フィールドに置き換わります。
  **
  ** 題名が置換されないようにするには、この値を設定しないか、空の文字列を設定します。
  ** (Crypto のみです)
   */
  { "crypt_protected_headers_write", DT_BOOL, R_NONE, {.l=OPTCRYPTPROTHDRSWRITE}, {.l=0} },
  /*
  ** .pp
  ** 設定する場合、Mutt は署名および暗号化したメールに対して、保護されたヘッダ
  ** ("メモリホール")を生成します。
  **
  ** 保護されたヘッダはメール中の暗号化または署名されたパート中に格納され、
  ** 漏洩や改ざんを防ぎます。詳細については
  ** https://github.com/autocrypt/memoryhole を参照してください。
  **
  ** 現時点でMutt は題名ヘッダのみサポートします。
  ** (Cryptoのみです)
   */
  { "pgp_replyencrypt",		DT_SYN,  R_NONE, {.p="crypt_replyencrypt"}, {.p=0} },
  { "crypt_replyencrypt",	DT_BOOL, R_NONE, {.l=OPTCRYPTREPLYENCRYPT}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、暗号化されたメッセージに対して、自動的に PGP または OpenSSL で
  ** 暗号化した返信を行います。
  ** (Crypto のみです)
  */
  { "pgp_replysign",	DT_SYN, R_NONE, {.p="crypt_replysign"}, {.p=0} },
  { "crypt_replysign",	DT_BOOL, R_NONE, {.l=OPTCRYPTREPLYSIGN}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、署名されたメッセージに対して、自動的に PGP または OpenSSL で
  ** 署名した返信を行います。
  ** .pp
  ** \fBNote:\fP これは、暗号化され、\fIかつ\fP 署名されているメッセージに
  ** 対しては動作しません。
  ** (Crypto のみです)
  */
  { "pgp_replysignencrypted",   DT_SYN,  R_NONE, {.p="crypt_replysignencrypted"}, {.p=0} },
  { "crypt_replysignencrypted", DT_BOOL, R_NONE, {.l=OPTCRYPTREPLYSIGNENCRYPTED}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、暗号化れたメッセージに対して、自動的に PGP または OpenSSL で
  ** 署名した返信を行います。これは、自動的に暗号化されるすべてのメッセージに署名
  ** 出来るという理由で、$$crypt_replyencrypt と組み合わせると意味があります。
  ** これは、Mutt が暗号化されたメッセージが署名されているかどうかを見つけられない
  ** という、$$crypt_replysign の所で触れた問題を回避します。
  ** (Cryptoのみです)
  */
  { "crypt_timestamp", DT_BOOL, R_NONE, {.l=OPTCRYPTTIMESTAMP}, {.l=1} },
  /*
  ** .pp
  ** (Crypto only)
  ** \fIset\fP の場合、Mutt は PGP 又は S/MIME 出力を囲む行中にタイムスタンプを
  ** 含めるので、そのような行のスプーフィングはより困難になります。
  ** それらの行にマークを付けるために色設定を使い、それらに依存している場合、
  ** この設定を \fIunset\fP できます。
  ** (Cryptoのみです)
  */
  { "crypt_use_gpgme",  DT_BOOL, R_NONE, {.l=OPTCRYPTUSEGPGME}, {.l=0} },
  /*
  ** .pp
  ** この変数は GPGMEが有効の crypto バックエンドの使用を制御します。\fIset\fPで、
  ** Mutt が gpgme サポートを有効にしてビルドされている場合、 S/MIME と PGP 用の
  ** gpgme コードが、旧来のコードの代わりに使われます。このオプションを .muttrc
  ** 中で設定しなければならないことに注意してください。対話的に使用しても効果は
  ** ありません。
  ** .pp
  ** GPGME バックエンドは古い形のインライン(従来型の) PGP で暗号化された、あるいは
  ** 署名されたメッセージ作成のサポートがないことに注意してください($$pgp_autoinlineを
  ** 参照してください)。
   */
  { "crypt_use_pka", DT_BOOL, R_NONE, {.l=OPTCRYPTUSEPKA}, {.l=0} },
  /*
  ** .pp
  ** Mutt が署名の検証中(GPGME バックエンドでのみサポート)に PKA
  ** (http://www.g10code.de/docs/pka-intro.de.pdf を参照) を使うかどうかを制御
  ** します。
  */
  { "pgp_verify_sig",   DT_SYN,  R_NONE, {.p="crypt_verify_sig"}, {.p=0} },
  { "crypt_verify_sig",	DT_QUAD, R_NONE, {.l=OPT_VERIFYSIG}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** \fI``yes''\fPならば、常時 PGPまたは S/MIME 署名の検証を試みます。
  ** \fI``ask-*''\fPならば、署名を検証するか否かを聞いてきます。
  ** \fI``no''\fPならば、暗号署名の検証を行いません。
  ** (Cryptoのみです)
  */
  { "date_format",	DT_STR,	 R_MENU, {.p=&DateFmt}, {.p="!%a, %b %d, %Y at %I:%M:%S%p %Z"} },
  /*
  ** .pp
  ** この変数は、$$index_format 中の ``%d'' シーケンスによって表示される日付の書式を
  ** 制御します。これは日付を処理するために\fCstrftime(3)\fP 機能に渡されます。
  ** 正しい構文についてはマニュアルページを参照してください。
  ** .pp
  ** 文字列の最初の文字が感嘆符 (``!'')でない限り、月と曜日の名前はロケールに合わせて
  ** 展開されます。文字列の最初の文字が感嘆符だった場合、感嘆符は無視され、
  ** 残りの文字列中の月と曜日の名前は \fIC\fP ロケール(すなわち US英語)に展開されます。
  */
  { "default_hook",	DT_STR,	 R_NONE, {.p=&DefaultHook}, {.p="~f %s !~P | (~P ~C %s)"} },
  /*
  ** .pp
  ** この変数は、一致用のパターンの代わりに単純な正規表現のみが指定された場合に、
  ** ``$message-hook'', ``$reply-hook'', ``$send-hook'',
  ** ``$send2-hook'', ``$save-hook'', と ``$fcc-hook'' をどのように解釈するかを制御します。
  ** フックは、それが定義されたときに展開され、そのため、フックが定義された時にこの変数の
  ** 値が解釈されます。
  ** .pp
  ** 既定値は、メッセージが与えられた正規表現に一致するユーザから来たものか、
  ** 自分自身から来たもの(fromアドレスが ``$alternates''に一致する場合)で、
  ** 指定された正規表現が To または Cc のユーザに一致する場合に一致します。
  */
  { "delete",		DT_QUAD, R_NONE, {.l=OPT_DELETE}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** メールボックスを同期するかクローズしたときに、実際に削除するかどうかを制御します。
  ** \fIyes\fPに設定した場合、削除マークが付けられたメッセージは、問い合わせなく
  ** 自動的に削除されます。\fIno\fPに設定された場合、削除マーク付きのメッセージは
  ** メールボックスに残ったままになります。
  */
  { "delete_untag",	DT_BOOL, R_NONE, {.l=OPTDELETEUNTAG}, {.l=1} },
  /*
  ** .pp
  ** このオプションが\fIset\fPの場合、mutt はメッセージに削除マークを付けるときに
  ** タグを外します。これは、明示的にメッセージを削除するか、他のフォルダに
  ** セーブする際に適用されます。
  */
  { "digest_collapse",	DT_BOOL, R_NONE, {.l=OPTDIGESTCOLLAPSE}, {.l=1} },
  /*
  ** .pp
  ** このオプションが\fIset\fPの場合、mutt の受信添付メニューは、multipart/digest中の
  ** 個々のメッセージの副パートを表示しません。それらサブパートを見るためには、メニュー中で
  ** ``v'' を押します。
  */
  { "display_filter",	DT_PATH, R_PAGER, {.p=&DisplayFilter}, {.p=0} },
  /*
  ** .pp
  ** 設定されている場合、指定されたコマンドが、メッセージのフィルタに使われます。
  ** メッセージが表示されている場合、それが$$display_filter の標準入力に渡され、
  ** フィルタされたメッセージは標準出力から読み出されます。
  */
#if defined(DL_STANDALONE) && defined(USE_DOTLOCK)
  { "dotlock_program",  DT_PATH, R_NONE, {.p=&MuttDotlock}, {.p=BINDIR "/mutt_dotlock"} },
  /*
  ** .pp
  ** Mutt によって使われる  \fCmutt_dotlock(8)\fP の実行形式へのパスが含まれています。
  */
#endif
  { "dsn_notify",	DT_STR,	 R_NONE, {.p=&DsnNotify}, {.p=0} },
  /*
  ** .pp
  ** この変数は、通知が戻ってくる際の要求を設定します。文字列はカンマで分離された
  ** 一覧(スペースがあってはなりません)で構成されています:
  ** \fInever\fP は、通知要求を行いません、\fIfailure\fP は、転送が失敗した
  ** 場合に通知を要求します、\fIdelay\fP は、メッセージが遅延したときに通知します、
  ** \fIsuccess\fP は、転送が成功したときに通知します。
  ** .pp
  ** 例:
  ** .ts
  ** set dsn_notify="failure,delay"
  ** .te
  ** .pp
  ** \fB注意:\fP 配送に$$sendmail を使う場合、Sendmail 8.8.x かそれ以降 又は
  ** DSN に対して\fC-N\fP オプションをサポートする\fCsendmail(1)\fP 互換のインタフェースを
  ** サポートするMTA のどちらかを使わない限り、有効にしてはなりません。
  ** SMTP 配送においては、DSN サポートは自動的に検出されるので、DSN を使うか否かは
  ** サーバに依存します。
  */
  { "dsn_return",	DT_STR,	 R_NONE, {.p=&DsnReturn}, {.p=0} },
  /*
  ** .pp
  ** この変数は、メッセージがどれだけDSN メッセージで帰ってくるかを制御します。
  ** \fIhdrs\fP に設定して、メッセージヘッダのみ返ってくるようにするか、
  ** \fIfull\fP に設定して、メッセージ全体が返ってくるかのどちらかにできます。
  ** .pp
  ** 例:
  ** .ts
  ** set dsn_return=hdrs
  ** .te
  ** .pp
  ** \fB注意:\fP 配送に$$sendmail を使う場合、Sendmail 8.8.x 以降又は
  ** DSN に対して\fC-R\fP オプションをサポートする\fCsendmail(1)\fP 互換のインタフェースを
  ** サポートするMTA のどちらかを使わない限り、有効にしてはなりません。
  ** SMTP 配送においては、DSN サポートは自動的に検出されるので、DSN を使うか否かは
  ** サーバに依存します。
  */
  { "duplicate_threads",	DT_BOOL, R_RESORT|R_RESORT_INIT|R_INDEX, {.l=OPTDUPTHREADS}, {.l=1} },
  /*
  ** .pp
  ** この変数は、$$sort が \fIthreads\fP に設定されているときに、Mutt が同じ Message-Id を
  ** をスレッドにするかどうかを制御します。\fIset\fP の場合、スレッドツリー中で、
  ** 等号 を使って他と重複していると考えていることを表示します。
  */
  { "edit_headers",	DT_BOOL, R_NONE, {.l=OPTEDITHDRS}, {.l=0} },
  /*
  ** .pp
  ** このオプションは、メッセージ本体といっしょに送信するメッセージのヘッダを編集
  ** 出来るようにします。
  ** .pp
  ** 編集メニューにはローカライズしたヘッダラベルを使う事が出来るにもかかわらず、
  ** エディタに渡されるラベルは標準の RFC 2822 ヘッダになります
  ** (たとえば To:, Cc:, Subject:)。エディタ中で追加されたヘッダは
  ** RFC 2822 ヘッダでなければならないか、``$edit-header'' 中にある
  ** 疑似ヘッダでなければなりません。Mutt はローカライズしたヘッダラベルを理解出来ず、
  ** 実際のメールを走査する場合とは異なります。
  ** .pp
  ** 相互運用性のために、References: と Date: ヘッダの変更は無視されることに
  ** \fB注意\fP してください。
  */
  { "edit_hdrs",	DT_SYN,  R_NONE, {.p="edit_headers"}, {.p=0} },
  /*
  */
  { "editor",		DT_PATH, R_NONE, {.p=&Editor}, {.p=0} },
  /*
  ** .pp
  ** この変数は Mutt で使うエディタを指定します。この既定値は環境変数
  ** \fC$$$VISUAL\fP, 又は \fC$$$EDITOR\fP の値か、それらが設定されていない場合は
  ** ``vi'' と言う文字列になります。
  ** .pp
  ** \fC$$editor\fP 文字列は、編集されるファイルの名前に置き換えられる \fI%s\fP エスケープ文字
  ** を含むことができます。\fC$$editor\fP中に \fI%s\fP がない場合は、空白と編集されるファイル
  ** 名が追加されます。
  ** .pp
  ** 結果の文字列は以下のように実行されます。
  ** .ts
  ** sh -c 'string'
  ** .te
  ** .pp
  ** ここで、\fIstring\fP は上記で説明したように、\fC$$editor\fP を展開したものです。
  */
  { "encode_from",	DT_BOOL, R_NONE, {.l=OPTENCODEFROM}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は行の先頭に文字列 ``From '' (末尾の空白に注意)を含む場合に
  ** メッセージを quoted-printable でエンコードします。これは、特定のメール配信および
  ** 転送エージェントがメッセージを処理する傾向を改ざんすることを防ぐのに有効です
  ** (ツールが、mbox セパレータとして行を間違って解釈することから防ぐため)。
  */
#if defined(USE_SSL_OPENSSL)
  { "entropy_file",	DT_PATH, R_NONE, {.p=&SslEntropyFile}, {.p=0} },
  /*
  ** .pp
  ** SSL ライブラリ機能を初期化するのに使う、ランダムデータを含むファイルです。
  */
#endif
  { "envelope_from_address", DT_ADDR, R_NONE, {.p=&EnvFrom}, {.p=0} },
  /*
  ** .pp
  ** 送信メッセージに対して\fIenvelope\fP の送信者を手動で設定します。
  ** この値は$$use_envelopeが\fIunset\fPの場合には無視されます。
  */
  { "error_history",	DT_NUM,	 R_NONE, {.p=&ErrorHistSize}, {.l=30} },
  /*
  ** .pp
  ** time this variable is set.
  ** この値はMutt によって表示されるエラーメッセージの大きさ(記憶されている
  ** 文字列の数)を制御します。これは\fC<error-history>\fP 機能で表示できます。
  ** 履歴はこの値が設定されている場合、毎回初期化されます。
  */
  { "escape",		DT_STR,	 R_NONE, {.p=&EscChar}, {.p="~"} },
  /*
  ** .pp
  ** 内蔵エディタ中での機能に使うエスケープ文字。
  */
  { "fast_reply",	DT_BOOL, R_NONE, {.l=OPTFASTREPLY}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、メッセージへ返信する際、最初に宛先と題名を入力するのを
  ** 省略します。メッセージを転送する場合は、題名の入力を省略します。
  ** .pp
  ** \fB注意:\fP この変数は $$autoedit 変数が \fIset\fP の場合は意味がありません。
  */
  { "fcc_attach",	DT_QUAD, R_NONE, {.l=OPT_FCCATTACH}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** この変数は送信メッセージの添付がメッセージ本体と共に保存されるかどうかを
  ** 制御します。
  */
  { "fcc_before_send",	DT_BOOL, R_NONE, {.l=OPTFCCBEFORESEND}, {.l=0} },
  /*
  ** .pp
  ** この値が \fIset\fP の場合、FCC はメッセージ送信前にが起こります。
  ** 送信前に、メッセージは操作できませんので、送信されたものと全く同じものが
  ** 保存されます。$$fcc_attach と $$fcc_clear は無視されます(それらの
  ** 既定値を使います)。
  ** .pp
  ** \fIunset\fPの場合は既定値で、FCC は送信後に起こります。変数
  ** $$fcc_attach と $$fcc_clear が使われ、必要に応じて添付あるいは暗号化/署名
  ** なしで保存することができます。
  */
  { "fcc_clear",	DT_BOOL, R_NONE, {.l=OPTFCCCLEAR}, {.l=0} },
  /*
  ** .pp
  ** この変数が \fIset\fP の場合、FCC は実際のメッセージが暗号化されているか又は
  ** 署名されているかにかかわらず、復号化し、かつ署名を外して保存されます。
  ** (PGP のみです)
  */
  { "flag_safe", DT_BOOL, R_NONE, {.l=OPTFLAGSAFE}, {.l=0} },
  /*
  ** .pp
  ** 設定されている場合、フラグ付きのメッセージは削除できません。
  */
  { "folder",		DT_PATH, R_NONE, {.p=&Maildir}, {.p="~/Mail"} },
  /*
  ** .pp
  ** メールボックスの既定の位置を指定します。パス名の先頭に付けた ``+'' 又は ``='' は
  ** この変数の値に展開されます。この変数の値を(既定値から)変更した場合、
  ** ``$mailboxes''コマンドを使うときに展開が起こるので、
  ** 割り当てが発生する\fI前に\fP何かの変数に対して``+'' 又は ``='' を使うようにしなければ
  ** ならないことに注意しなければなりません。
  */
  { "folder_format",	DT_STR,	 R_MENU, {.p=&FolderFormat}, {.p="%2C %t %N %F %2l %-8.8u %-8.8g %8s %d %f"} },
  /*
  ** .pp
  ** This variable allows you to customize the file browser display to your
  ** personal taste.  This string is similar to $$index_format, but has
  ** its own set of \fCprintf(3)\fP-like sequences:
  ** この変数で、好みに応じてファイルブラウザ表示をカスタマイズできます。
  ** この文字列は $$index_format と似ていますが、固有の\fCprintf(3)\fP風の書式があります。
  ** .dl
  ** .dt %C  .dd 現在のファイル番号
  ** .dt %d  .dd 最後に変更されたフォルダの日時/時間
  ** .dt %D  .dd $date_format を使って最後に変更されたフォルダの日時/時間
  ** .dt %f  .dd ファイル名 (ディレクトリ名には ``/'' が付与、
  **             シンボリックリンクには ``@'' が付与、
  **             実行ファイルには``*'' が付与)
  ** .dt %F  .dd ファイルのパーミッション
  ** .dt %g  .dd グループ名(あるいは存在しない場合は gid の値)
  ** .dt %l  .dd ハードリンク数
  ** .dt %m  .dd mailbox * 中のメッセージ数
  ** .dt %n  .dd mailbox * 中の未読メッセージ数
  ** .dt %N  .dd 新着メールがあれば N で、なければ空白
  ** .dt %s  .dd バイト単位のサイズ($formatstrings-size を参照)
  ** .dt %t  .dd タグが付いていれば ``*'' で、そうでなければ空白
  ** .dt %u  .dd owner の名前 (あるいは存在しない場合は uid)
  ** .dt %>X .dd 残りの文字列を右寄せにして、``X'' で埋める
  ** .dt %|X .dd 行末まで ``X'' で埋める
  ** .dt %*X .dd 文字 ``X'' を埋め草として soft-fill
  ** .de
  ** .pp
  ** ``soft-fill'' の説明については$$index_format のドキュメントを参照してください。
  ** .pp
  ** * = は、ゼロ以外の場合にオプションで表示されます
  ** .pp
  ** %m, %n, と %N は、モニタされたメールボックスでのみ動作します。
  ** %m は $$mail_check_stats が設定されていることが必要です。
  ** %n は $$mail_check_stats が設定されていることが必要です(IMAP メールボックスを除く)。
  */
  { "followup_to",	DT_BOOL, R_NONE, {.l=OPTFOLLOWUPTO}, {.l=1} },
  /*
  ** .pp
  ** メール送信時に``Mail-Followup-To:''ヘッダフィールドを生成するかどうかを
  ** 制御します。\fIset\fPならば、Mutt は``$subscribe'' 又は ``$lists'' コマンドで
  ** 指定した既存メーリングリストに返信する時にこのフィールドを生成します。
  ** .pp
  ** このフィールドには2つの目的があります。最初のものは、メーリングリストに
  ** 送ったメッセージへの返信の重複したコピーを受信しないことで、2番目は、
  ** 購読していない既存メーリングリストに送ったメッセージに対する返信を個別に
  ** 受け取ることです。
  ** .pp
  ** ヘッダは 購読したリストのアドレスと、購読していないメーリングリストに
  ** 対しては、リストのアドレスと自分のメールアドレスを含みます。このヘッダがない場合、
  ** 購読したメーリングリストの、自分のメッセージに対してグループ返信した場合、
  ** メーリングリストと自分のアドレス両方にメールが送信され、結果として
  ** 同じメールのコピーが2つ送られてくることになります。
  ** 
  */
  { "force_name",	DT_BOOL, R_NONE, {.l=OPTFORCENAME}, {.l=0} },
  /*
  ** .pp
  ** この変数は$$save_name と似ていますが、そのメールボックスが存在していなくても、
  ** アドレス中のユーザ名によって送信するメッセージのコピーを
  ** 保存するところが異なります。
  ** .pp
  ** $$record 変数も参照してください。
  */
  { "forward_attachments", DT_QUAD, R_NONE, {.l=OPT_FORWATTS}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** インラインで転送する時(すなわち、$$mime_forward \fIunset\fP 又は
  ** ``no'' と答えて、かつ、$$forward_decode が\fIset\fP)、
  ** 通常の方法でデコードできない添付は、この4択が\fIset\fP か
  ** ``yes'' と答えた場合、新しく編集するメッセージに添付されます。
  */
  { "forward_attribution_intro", DT_STR, R_NONE, {.p=&ForwardAttrIntro}, {.p="----- Forwarded message from %f -----"} },
  /*
  ** .pp
  ** これは、メッセージの主要な本体($$mime_forward が未設定の場合)で転送された
  ** メッセージの前にある文字列です。\fCprintf(3)\fP風の書式で定義されているものの
  ** 完全な一覧は、$$index_format 節を参照してください。$$attribution_locale も
  ** 参照してください。
  */
  { "forward_attribution_trailer", DT_STR, R_NONE, {.p=&ForwardAttrTrailer}, {.p="----- End forwarded message -----"} },
  /*
  ** .pp
  ** これは、メッセージの主要な本体($$mime_forward が未設定の場合)で転送された
  ** メッセージの後にある文字列です。\fCprintf(3)\fP風の書式で定義されているものの
  ** 完全な一覧は、$$index_format 節を参照してください。$$attribution_locale も
  ** 参照してください。
  */
  { "forward_decode",	DT_BOOL, R_NONE, {.l=OPTFORWDECODE}, {.l=1} },
  /*
  ** .pp
  ** メッセージの転送時に、複雑な MIME メッセージを \fCtext/plain\fP に復号化するかを
  ** 制御します。メッセージヘッダもRFC2047で復号化されます。
  ** この変数は、$$mime_forward が\fIunset\fPの時にのみ使われ、
  ** その他の場合は$$mime_forward_decode が代わりに使われます。 
  */
  { "forw_decode",	DT_SYN,  R_NONE, {.p="forward_decode"}, {.p=0} },
  /*
  */
  { "forward_decrypt",	DT_BOOL, R_NONE, {.l=OPTFORWDECRYPT}, {.l=1} },
  /*
  ** .pp
  ** メッセージを転送する時の暗号化メッセージの取り扱いを制御します。
  ** \fIset\fP の時、暗号化の外側のレイヤは削除されます。この変数は、
  ** $$mime_forward が\fIset\fP で、$$mime_forward_decode が\fIunset\fP
  ** の時にのみ使われます。
  ** (PGP のみです)
  */
  { "forw_decrypt",	DT_SYN,  R_NONE, {.p="forward_decrypt"}, {.p=0} },
  /*
  */
  { "forward_edit",	DT_QUAD, R_NONE, {.l=OPT_FORWEDIT}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** この4択は、メッセージを転送する時に、エディタに自動的に移行するかどうかを
  ** 制御します。常時変更なしに転送したい場合は、この設定を ``no'' にします。
  */
  { "forward_format",	DT_STR,	 R_NONE, {.p=&ForwFmt}, {.p="[%a: %s]"} },
  /*
  ** .pp
  ** この変数はメッセージを転送する時の既定の題名を制御します。
  ** これは、$$index_format 変数と同じ書式を使います。
  */
  { "forw_format",	DT_SYN,  R_NONE, {.p="forward_format"}, {.p=0} },
  /*
  */
  { "forward_quote",	DT_BOOL, R_NONE, {.l=OPTFORWQUOTE}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、メッセージの主要な本体($$mime_forward が\fIunset\fPの時)に
  ** 含まれる転送メッセージは$$indent_string を使って引用されます。
  */
  { "forw_quote",	DT_SYN,  R_NONE, {.p="forward_quote"}, {.p=0} },
  /*
  */
  { "from",		DT_ADDR, R_NONE, {.p=&From}, {.p=0} },
  /*
  ** .pp
  ** \fIset\fP の時、この変数は既定の発信アドレスを含みます。これは、
  ** ``$my_hdr'' (``$send-hook''からを含みます)や$$reverse_nameを使って上書き
  ** することができます。この変数は $$use_from が\fIunset\fPの時に無視されます。
  ** .pp
  ** この設定の既定値は環境変数 \fC$$$EMAIL\fPの値です。
  */
  { "gecos_mask",	DT_RX,	 R_NONE, {.p=&GecosMask}, {.p="^[^,]*"} },
  /*
  ** .pp
  ** 別名を展開するときにパスワードエントリのGECOS フィールドをMutt が操作するときに使われる
  ** 正規表現です。既定値は最初に``,'' が見つかるまでの文字列を返します。
  ** GECOS フィールドが``lastname, firstname''のような文字列を含む場合、
  ** これを``\fC.*\fP'' とすべきです。
  ** .pp
  ** これは、以下のような動作の時に便利です: ある人の完全な名前が ``Steve Franklin''である
  ** ユーザID ``stevef'' にメールを送るとします。Mutt が``stevef'' を
  ** ``Franklin" stevef@foo.bar'' に展開する場合、完全な名前に一致する正規表現に、
  ** $$gecos_mask を設定すれば、Mutt は``Franklin'' を``Franklin, Steve'' に展開します。
  */
  { "hdr_format",	DT_SYN,  R_NONE, {.p="index_format"}, {.p=0} },
  /*
  */
  { "hdrs",		DT_BOOL, R_NONE, {.l=OPTHDRS}, {.l=1} },
  /*
  ** .pp
  ** \fIunset\fP の場合、通常``$my_hdr''コマンドによって追加されるヘッダフィールドが
  ** 作成されません。この変数の効果を発揮するためには、新規メッセージの編集か、返信時
  ** より前に設定解除\fIしなければなりません\fP。\fIset\fPの場合、
  ** ユーザ定義のヘッダフィールドはすべての新規メッセージに追加されます。
  */
  { "header",		DT_BOOL, R_NONE, {.l=OPTHEADER}, {.l=0} },
  /*
  ** .pp
  ** fIset\fPの場合、この変数は編集バッファ中に返信するメッセージのヘッダを
  ** 含めるようにします。$$weed の設定が適当されます。
  */
#ifdef USE_HCACHE
  { "header_cache", DT_PATH, R_NONE, {.p=&HeaderCache}, {.p=0} },
  /*
  ** .pp
  ** この変数はヘッダキャッシュデータベースの場所を指定します。ディレクトリを示して
  ** いれば、Mutt はフォルダ単位のヘッダキャッシュデータベースがあり、ファイルを示して
  ** いれば、全体の単一ヘッダキャッシュとなります。既定では\fIunset\fPなので、
  ** ヘッダキャッシュは使われません。
  ** .pp
  ** ヘッダキャッシュは、POP, IMAP, MH および Maildir フォルダを開くときに
  ** 大幅なスピードの改善が出来ます。詳細については ``$caching'' を参照してください。
  */
#if defined(HAVE_QDBM) || defined(HAVE_TC) || defined(HAVE_KC)
  { "header_cache_compress", DT_BOOL, R_NONE, {.l=OPTHCACHECOMPRESS}, {.l=1} },
  /*
  ** .pp
  ** Mutt が qdbm,tokyocabinet, 又は kyotocabinet を、ヘッダキャッシュのバックエンドとして
  ** 使うようにコンパイルされている場合、このオプションは、データベースを圧縮するか否かを
  ** 指定します。データベース中のファイルを圧縮すると、大きさがおおよそ通常の1/5に
  ** なりますが、展開を行うと、キャッシュされたフォルダを開くのが遅くなりますが、
  ** ヘッダキャッシュがないフォルダを開くよりは、まだ高速に開けます。
  */
#endif /* HAVE_QDBM */
#if defined(HAVE_GDBM) || defined(HAVE_DB4)
  { "header_cache_pagesize", DT_LNUM, R_NONE, {.p=&HeaderCachePageSize}, {.l=16384} },
  /*
  ** .pp
  ** ヘッダキャッシュのバックエンドとして、 gdbm 又は bdb4 のどちらかを使うように
  ** Mutt がコンパイルされている場合、このオプションはデータベースのページサイズを変更します。
  ** 値が大きすぎたり小さすぎたりすると、余分なスペース、メモリ、CPU 時間を消費します。
  ** 既定値はほとんどの場合において大きすぎず、小さすぎずに最適化されているはずです。
  */
#endif /* HAVE_GDBM || HAVE_DB4 */
#endif /* USE_HCACHE */
  { "header_color_partial", DT_BOOL, R_PAGER_FLOW, {.l=OPTHEADERCOLORPARTIAL}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、カラーヘッダの正規表現はカラーボディの正規表現と同じように
  ** 振る舞います。色は正規表現によって一致した正確なテキストに適用されます。
  ** \fIunset\fP の場合は、色はヘッダ全体に適用されます。
  ** .pp
  ** このオプションの1つの使い方は、ヘッダラベルにのみ色を適用することです。
  ** .pp
  ** 詳細は ``$color'' を参照してください。
  */
  { "help",		DT_BOOL, R_REFLOW, {.l=OPTHELP}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、各メニューで提供される、主要な機能に対するバインディングの説明を
  ** するヘルプ行は画面の最初の行に表示されます。
  ** .pp
  ** \fB注意:\fP 単一のキーストロークではなく、一連のキーストロークに機能がバインド
  ** されていると、バインディングは正確には表示されません。同様に、Mutt が動作中に
  ** バインディングが変更されてもヘルプ行は更新されません。この変数は主に新規
  ** ユーザを対象としているので、上記のどちらも大きな問題ではありません。
  */
  { "hidden_host",	DT_BOOL, R_NONE, {.l=OPTHIDDENHOST}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は、アドレスにドメイン部分を付加するときに、$$hostname
  ** 変数のホスト名部分をスキップします。この変数は Message-ID の生成には影響を
  ** 与えず、ドメインの最初の部分を切り取ることはありません。
  */
  { "hide_limited",	DT_BOOL, R_TREE|R_INDEX, {.l=OPTHIDELIMITED}, {.l=0} },
  /*
  ** .pp
  **  \fIset\fP の場合、Mutt は、スレッドツリー中で limit 機能により隠されている
  ** メッセージの存在を表示しません。
  */
  { "hide_missing",	DT_BOOL, R_TREE|R_INDEX, {.l=OPTHIDEMISSING}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt はスレッドツリー中で存在しないメッセージの存在を
  ** 表示しません。
  */
  { "hide_thread_subject", DT_BOOL, R_TREE|R_INDEX, {.l=OPTHIDETHREADSUBJECT}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt  はスレッドツリー内で、親と同じか以前に表示された
  ** もっとも近い sibling と同じ題名を持つメッセージの題名を表示しません。
  */
  { "hide_top_limited",	DT_BOOL, R_TREE|R_INDEX, {.l=OPTHIDETOPLIMITED}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は スレッドツリー中の最上位に、limit 機能によって隠されて
  ** いるメッセージの存在を表示しません。$$hide_limited が \fIset\fP の場合、
  ** このオプションは何の効果もないことに注意してください。
  */
  { "hide_top_missing",	DT_BOOL, R_TREE|R_INDEX, {.l=OPTHIDETOPMISSING}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt はスレッドツリーのスレッドの上部に、メッセージがないことの
  ** 表示をしません。$$hide_missing が \fIset\fP の場合、このオプションは何の効果も
  ** ないことに注意してください。
  */
  { "history",		DT_NUM,	 R_NONE, {.p=&HistSize}, {.l=10} },
  /*
  ** .pp
  ** この変数はカテゴリ毎の文字列ヒストリバッファのサイズ(記憶される文字列の数)を
  ** 制御します。バッファは変数が設定される度毎に消去されます。
  */
  { "history_file",     DT_PATH, R_NONE, {.p=&HistFile}, {.p="~/.mutthistory"} },
  /*
  ** .pp
  ** Mutt がそのヒストリをセーブするファイルです。
  ** .pp
  ** $$save_history も参照してください。
  */
  { "history_remove_dups", DT_BOOL, R_NONE, {.l=OPTHISTREMOVEDUPS}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、新しいエントリが追加されるときに、重複するかどうかを、すべての
  ** 文字列ヒストリでスキャンします。$$history_file 中の重複するエントリは
  ** 定期的に圧縮される時にも削除されます。
  */
  { "honor_disposition", DT_BOOL, R_NONE, {.l=OPTHONORDISP}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt はそのパートが平文で描画可能だったとしても、インラインで
  ** ``attachment''という配置の添付を表示しません。MIME パートは添付メニュー
  ** でのみ表示できます。
  ** .pp
  ** \fIunset\fP の場合、Mutt は適切に平文に変換できる場合、すべての MIME パートを
  ** 描画します。
  */
  { "honor_followup_to", DT_QUAD, R_NONE, {.l=OPT_MFUPTO}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** この変数は Mail-Followup-To ヘッダが、メッセージに対してグループ返信するときに
  ** 信頼できるか否かを制御します。
  */
  { "hostname",		DT_STR,	 R_NONE, {.p=&Fqdn}, {.p=0} },
  /*
  ** .pp
  ** Mutt が動いているシステムでホスト名とDNS ドメイン名が存在している場合、
  ** 完全修飾ホスト名(FQDN)を指定します。これは、ローカルメールアドレスのドメイン部分
  ** (``@'' の後)と、Message-Id ヘッダとして使われます。
  ** .pp
  ** この値は以下のようにして起動時に決まります。ノードのホスト名は \fCuname(3)\fP
  ** 機能によって最初に決まります。ドメインは次に\fCgethostname(2)\fP と
  ** \fCgetaddrinfo(3)\fP 機能によって検索されます。もしそれらの呼び出しで
  ** ドメインを決定できない場合、uname の結果の完全な値が使われます。オプションで、
  ** Mutt が固定したドメインでコンパイルできますが、この場合、検出された値は
  ** 使われません。
  ** .pp
  **  $$use_domain と $$hidden_host も参照してください。
  */
#if defined(HAVE_LIBIDN) || defined(HAVE_LIBIDN2)
  { "idn_decode",	DT_BOOL, R_MENU, {.l=OPTIDNDECODE}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は国際化ドメイン名をデコードして表示します。
  ** 注意: これが\fIunset\fP だったとしても、アドレスに対して IDN を使う事が出来ます。
  ** この値はデコードのみに影響します(IDN のみです)。
  */
  { "idn_encode",	DT_BOOL, R_MENU, {.l=OPTIDNENCODE}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は IDN を使って国際化ドメイン名をエンコードします。
  ** 使用する SMP サーバが新しい (RFC6531) UTF-8 エンコードされたドメインを扱える
  ** 場合は、これを設定しません。(IDN のみです)
  */
#endif /* defined(HAVE_LIBIDN) || defined(HAVE_LIBIDN2) */
  { "ignore_linear_white_space",    DT_BOOL, R_NONE, {.l=OPTIGNORELWS}, {.l=0} },
  /*
  ** .pp
  ** このオプションは エンコードされた単語とテキストの間の linear-white-space を
  ** MIME エンコードされた ``Subject:'' フィールドの表示が複数行に分割されてしまうことを
  ** 防ぐために単一の空白に置き換えます。
  */
  { "ignore_list_reply_to", DT_BOOL, R_NONE, {.l=OPTIGNORELISTREPLYTO}, {.l=0} },
  /*
  ** .pp
  ** メーリングリスト(``$subscribe'' 又は ``$lists'' コマンドで定義されたもの)
  ** からメッセージに返信する時に\fC<reply>\fP 機能の動作に影響します。
  ** \fIset\fPの場合で、``Reply-To:'' フィールドが ``To:'' フィールドの値と
  ** 同じに設定されている場合、Mutt は、自動的にメーリングリストへの返信を
  ** 自動化するために、``Reply-To:'' フィールドが、メーリングリストによって
  ** 設定されたと仮定し、このフィールドを無視します。このオプションが\fIset\fP
  ** の場合、メーリングリストに対する直接の返信は \fC$<list-reply>\fP 機能を
  ** 使います。\fC<group-reply>\fP は送信者とメーリングリスト両方に返信します。
  */
#ifdef USE_IMAP
  { "imap_authenticators", DT_STR, R_NONE, {.p=&ImapAuthenticators}, {.p=0} },
  /*
  ** .pp
  ** これはコロンで区切られた、認証方法の一覧で、Mutt はここに並んでいる順で、
  ** これらの方式を使って、 IMAP サーバにログインしようとします。認証方式は
  ** ``login'' か、IMAP のケーパビリティ文字列 'AUTH=xxx' の右辺、たとえば
  ** ``digest-md5'', ``gssapi'' 又は``cram-md5'' を使います。このオプションは
  ** 大文字小文字を無視します。\fIunset\fPの場合(既定値)、Mutt はすべての有効な
  ** 方式を、もっともセキュアな方法からセキュアでない方法の順で試みます。
  ** .pp
  ** 例:
  ** .ts
  ** set imap_authenticators="gssapi:cram-md5:login"
  ** .te
  ** .pp
  ** \fB注意:\fP Mutt は前のメソッドが無効な場合にのみ、他の認証方式にフォールバック
  ** します。メソッドが有効だが認証が失敗した場合、Mutt は IMAP サーバには
  ** 接続しません。
  */
  { "imap_check_subscribed",  DT_BOOL, R_NONE, {.l=OPTIMAPCHECKSUBSCRIBED}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は接続時に、購読しているフォルダを読み出しして、
  ** 独立した ``$mailboxes'' コマンドを発行したのと同様に、ポーリングする
  ** メールボックスのセットにそれらを追加します。
  */
  { "imap_condstore",  DT_BOOL, R_NONE, {.l=OPTIMAPCONDSTORE}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は、サーバにより広告がある場合、CONDSTORE 拡張
  ** (RFC 7162) を使います。Mutt の現在の実装は基本的なものであり、
  ** 初期メッセージのフェッチとフラグの更新のみを使います。
  ** .pp
  ** いくつかの IMAP サーバでは、これを有効にすると、初期メッセージのダウンロードが
  ** 若干早くなります。残念なことに、Gmail はそのうちの1つには入らず、有効にすると
  ** 表示が遅くなります。受け取るメリットは変わる場合があります。
  */
  { "imap_delim_chars",		DT_STR, R_NONE, {.p=&ImapDelimChars}, {.p="/."} },
  /*
  ** .pp
  ** これには IMAP パスを表示する際にフォルダの区切りとして扱いたい 文字の一覧が
  ** 入ります。特にこれが役立つのは、 \fIfolder\fP 変数へのショートカットのために
  ** ``='' を使うときです。
  */
  { "imap_fetch_chunk_size",	DT_LNUM, R_NONE, {.p=&ImapFetchChunkSize}, {.l=0} },
  /*
  ** .pp
  ** この値を 0 より大きくすると、新しいヘッダはこのサイズのセットでダウンロード
  ** されます。非常に大きなメールボックスを使っている場合、すべての新しいヘッダを
  ** 単一の取得操作で行う代わりに、このサイズのセット毎に取得コマンドを送ることで、
  ** メールボックスをオープンするときにタイムアウトと切断を防ぐことが出来ます。
  */
  { "imap_headers",	DT_STR, R_INDEX, {.p=&ImapHeaders}, {.p=0} },
  /*
  ** .pp
  ** Mutt がインデックスメニューを表示する前に、IMAP サーバから既定のヘッダ
  ** (``Date:'', ``From:'', ``Sender:'', ``Subject:'', ``To:'', ``Cc:'', ``Message-Id:'',
  ** ``References:'', ``Content-Type:'', ``Content-Description:'', ``In-Reply-To:'',
  ** ``Reply-To:'', ``Lines:'', ``List-Post:'', ``X-Label:'') に追加で、ここで指定する
  ** ヘッダを要求します。スパム検出のためのヘッダを追加したいときに使います。
  ** .pp
  ** \fB注意:\fP これは空白で分離されたリストで、各項目はすべて大文字で
  ** コロンを含んではいけません。たとえば、``X-Bogosity:'' と ``X-Spam-Status:''
  ** は ``X-BOGOSITY X-SPAM-STATUS'' となります。
  */
  { "imap_idle",                DT_BOOL, R_NONE, {.l=OPTIMAPIDLE}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt  は現在のメールボックスで、新規メールのチェックのために
  ** IMAP IDLE 拡張を使おうとします。いくつかのサーバ(このオプションに対しては
  ** dovecot が想起されます)は、Mutt の実装に対して不正確に反応します。
  ** 接続が定期的にフリーズするように見える場合は、この設定の解除を試してみてください。
  */
  { "imap_keepalive",           DT_NUM,  R_NONE, {.p=&ImapKeepalive}, {.l=300} },
  /*
  ** .pp
  ** この変数は、Mutt が接続を終了する前に、サーバから接続をクローズするのを
  ** 防ぐために、オープンしている IMAP 接続をポーリングする前に Mutt が待つ
  ** 最大時間を秒数で指定します。既定値は、サーバがこれを実行出来るようになるまで、
  ** RFC の指定による最低限の時間(30分)内ですが、実際にはRFC はそこかしこで
  ** 破られています。接続していないとみなされて IMAP サーバから切断されるようで
  ** あれば、この数値を減らしてください。
  */
  { "imap_list_subscribed",	DT_BOOL, R_NONE, {.l=OPTIMAPLSUB}, {.l=0} },
  /*
  ** .pp
  ** この変数は、IMAP フォルダ一覧表示で検索するのを購読フォルダのみにするか、すべての
  ** フォルダにするかを制御します。これは、\fC<toggle-subscribed>\fP 機能を使って
  ** IMAP ブラウザ内でON/OFFできます。
  */
  { "imap_login",	DT_STR,  R_NONE, {.p=&ImapLogin}, {.p=0} },
  /*
  ** .pp
  ** IMAP サーバへのログイン名。
  ** .pp
  ** この変数は既定で$$imap_user の値となります。
  */
  { "imap_oauth_refresh_command", DT_STR, R_NONE, {.p=&ImapOauthRefreshCmd}, {.p=0} },
  /*
  ** .pp
  ** IMAP サーバの接続を認証するための OAUTH リフレッシュトークンを生成するために
  ** 実行するコマンド。このコマンドはOAUTHBEARER 認証メカニズムを使った接続を試みる
  ** 度毎に実行されます。詳細については``$oauth'' を参照してください。
  */
  { "imap_pass", 	DT_STR,  R_NONE, {.p=&ImapPass}, {.p=0} },
  /*
  ** .pp
  ** IMAP アカウントのパスワードを指定します。\fIunset\fP の場合、Mutt は
  ** \fC<imap-fetch-mail>\fP機能を起動した場合又はIMAP フォルダを開こうとするときに
  ** パスワードを要求してきます。
  ** .pp
  ** \fB警告\fP: このオプションは、十分に安全なマシン上でのみ使うべきです。
  ** なぜなら、muttrc ファイルを自分以外が読めないように したつもりでも、スーパユーザは
  ** 読めるからです。
  */
  { "imap_passive",		DT_BOOL, R_NONE, {.l=OPTIMAPPASSIVE}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は新規メールチェックのために新規のIMAP 接続を開くことは
  ** しません。Mutt は新規メールを既存のIMAP 接続でのみ確認することになります。これが
  ** 有用となるのは、Mutt 起動時にユーザ名とパスワードの入力を要求されたくない場合や、
  ** 接続するのに時間がかかる場合などです。
  */
  { "imap_peek", DT_BOOL, R_NONE, {.l=OPTIMAPPEEK}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt はサーバからメッセージを読み出すときはいつでも
  ** メールを読んだというマークを暗黙で付けることを防ぎます。これは一般的に
  ** よい手法ですが、IMAP フォルダをクローズするときに少し遅くなることが
  ** あります。このオプションはスピード命の人をなだめるためにあります。
  */
  { "imap_pipeline_depth", DT_NUM,  R_NONE, {.p=&ImapPipelineDepth}, {.l=15} },
  /*
  ** .pp
  ** サーバに対して送られる前にキューにたまる IMAP コマンドの数を制御します。
  ** 深いパイプラインは、Mutt がサーバを待たなければならない大量の時間を節約し、
  ** IMAP サーバの応答性を大幅に向上させるようにできます。しかし、すべてのサーバが
  ** パイプライン化されたコマンドを正確に扱えるわけではないので、もしも問題が
  ** 発生した場合は、この変数を 0 にしてみてください。
  ** .pp
  ** \fB注意:\fP この変数を変更しても接続時には何の影響もありません。
  */
  { "imap_poll_timeout", DT_NUM,  R_NONE, {.p=&ImapPollTimeout}, {.l=15} },
  /*
  ** .pp
  ** この変数は、新規メールのためにIMAP 接続をポーリングする時、タイムアウトと
  ** 接続をクローズする前に、Mutt が応答を待つ最大の時間を指定します。
  ** 0 に設定すると、タイムアウトを無効にします。
  */
  { "imap_qresync",  DT_BOOL, R_NONE, {.l=OPTIMAPQRESYNC}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は、サーバから広告があった場合、QRESYNC 拡張(RFC 7162)を
  ** 使います。Mutt の現在の実装は基本的なものであり、
  ** 初期メッセージのフェッチとフラグの更新のみを使います。
  ** .pp
  ** 注意: この機能は現在実験的なものです。たとえば、メッセージの重複や欠落のような
  ** おかしな挙動があった場合には、バグとして是非ご連絡ください。
  */
  { "imap_servernoise",		DT_BOOL, R_NONE, {.l=OPTIMAPSERVERNOISE}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fPの場合、Mutt はエラーメッセージとして IMAP サーバからの警告メッセージを
  ** 表示します。そのようなメッセージはしばしば無害だったり、ユーザとは関係のない
  ** サーバ上の設定問題によって生成されたものなので、時には抑制したいかもしれません。
  */
  { "imap_user",	DT_STR,  R_NONE, {.p=&ImapUser}, {.p=0} },
  /*
  ** .pp
  ** IMAP サーバ上で扱うメールの所有ユーザ名。 
  ** .pp
  ** この変数は既定でローカルマシン上でのユーザ名となります。
  */
#endif
  { "implicit_autoview", DT_BOOL,R_NONE, {.l=OPTIMPLICITAUTOVIEW}, {.l=0} },
  /*
  ** .pp
  ** ``yes'' に設定した場合、Mutt は 内部ビューワが定義されていない \fIすべての\fP
  **  MIME 添付 に  ``\fCcopiousoutput\fP'' フラグがセットされた mailcap エントリを
  ** 探します。そのようなエントリが見つかった場合、Mutt は本文部分をテキスト形式に
  ** 変換するために、そのエントリに対して定義されているビューワを使います。
  */
  { "include",		DT_QUAD, R_NONE, {.l=OPT_INCLUDE}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** 返信しようとしている元のメッセージ内容を返信に含めるかどうかを制御します。
  */
  { "include_encrypted",	DT_BOOL, R_NONE, {.l=OPTINCLUDEENCRYPTED}, {.l=0} },
  /*
  ** .pp
  ** Mutt が、返信時に暗号化された添付内容を分離して含めるかどうかを制御します。
  ** .pp
  ** この変数は、攻撃者に対して返信する時に、暗号化された内容を予期せぬ形で公開される
  ** ことを防ぐために追加されました。以前に暗号化されたメッセージが攻撃者によって
  ** 添付された場合、不注意な受信者をだましてメッセージを解読し、返信に含めることが
  ** できます。
  */
  { "include_onlyfirst",	DT_BOOL, R_NONE, {.l=OPTINCLUDEONLYFIRST}, {.l=0} },
  /*
  ** .pp
  ** Mutt が返信時にメッセージの最初の添付のみを含めるかどうかを制御します。
  */
  { "indent_string",	DT_STR,	 R_NONE, {.p=&Prefix}, {.p="> "} },
  /*
  ** .pp
  ** 返信時に、メッセージ中に引用されているテキストの各行の先頭に付加する
  ** 文字列を指定します。より狂信的なネチズンを煽る傾向があるため、この値を
  ** 変更しないことを強く推奨します。
  ** .pp
  ** このオプションの値は、引用メカニズムが format=flowed に対して厳格に定義
  ** されているため、$$text_flowed が設定されている場合は無視されます。
  ** .pp
  ** このオプションは、書式付きの文字列で、サポートされる \fCprintf(3)\fP 形式の
  ** 書式については$$index_format を参照してください。
  */
  { "indent_str",	DT_SYN,  R_NONE, {.p="indent_string"}, {.p=0} },
  /*
  */
  { "index_format",	DT_STR,	 R_BOTH, {.p=&HdrFmt}, {.p="%4C %Z %{%b %d} %-15.15L (%?l?%4l&%4c?) %s"} },
  /*
  ** .pp
  ** この変数で、個人の好みによってメッセージインデックス表示をカスタマイズする
  ** ことができます。
  ** .pp
  ** ``Format strings'' はフォーマット出力のために使う C の\fCprintf(3)\fP 関数と
  ** 似ています(詳細はマニュアルページを参照してください)。%? の説明については
  ** $$status_format の説明を参照してください。以下は Mutt で定義されているものです。
  ** .dl
  ** .dt %a .dd 差出人のアドレス
  ** .dt %A .dd (もしあれば) reply-to アドレス (なければ差出人アドレス) 
  ** .dt %b .dd オリジナルのメッセージフォルダのファイル名(mailbox です)
  ** .dt %B .dd 送信された手紙のリストかフォルダ名 (%b).
  ** .dt %c .dd メッセージの(バイト単位の)サイズ ($formatstrings-size 参照)
  ** .dt %C .dd 現在のメッセージ番号
  ** .dt %d .dd メッセージの日付と時刻を、送信者の現地時間に変換して
  **            $$date_format の形式にしたもの
  ** .dt %D .dd メッセージの日付と時刻を、自分の地域の時間に変換して
  **            $$date_format の形式にしたもの
  ** .dt %e .dd スレッド内におけるメッセージ番号 
  ** .dt %E .dd スレッド内のメッセージ総数
  ** .dt %f .dd 送信者 (アドレス + 実名)、From: 又は Return-Path: のどちらか
  ** .dt %F .dd 差出人の名前か自分自身から出されたメールであれば受信者の名前
  ** .dt %H .dd メッセージの spam 属性
  ** .dt %i .dd 現在のメッセージの message-id
  ** .dt %l .dd 未処理のメッセージの行数(maildir, mh, とIMAP フォルダでは
  **            おそらく動作しない)
  ** .dt %L .dd ``To:'' 又は ``Cc:'' ヘッダフィールドがユーザの``$subscribe''
  **            コマンドで定義されたアドレスに一致する場合、これは "To <リスト名>"
  **            と表示し、その他の場合は %F と同じになります。 
  ** .dt %m .dd メールボックス中のメッセージ総数
  ** .dt %M .dd スレッドが折りたたまれている場合の非表示のメッセージ数
  ** .dt %N .dd メッセージのスコア
  ** .dt %n .dd 差出人の本名(あるいは存在しなければアドレス)
  ** .dt %O .dd Mutt がかつてメッセージを隠していたもとの保存ファイル:
  **            リスト名又はリストに送らない場合は受信者の名前
  ** .dt %P .dd ビルトインページャにおける処理インジケータ(どのくらいファイルが表示されたか)
  ** .dt %r .dd コンマで分離された ``To:'' 受信者のリスト
  ** .dt %R .dd コンマで分離された ``Cc:'' 受信者のリスト
  ** .dt %s .dd メッセージの題名
  ** .dt %S .dd 単一文字でのメッセージのステータス(``N''/``O''/``D''/``d''/``!''/``r''/``\(は'')
  ** .dt %t .dd ``To:'' フィールド(受信者)
  ** .dt %T .dd $$to_chars 文字列からの適切な文字
  ** .dt %u .dd 送信者のユーザ(ログイン)名
  ** .dt %v .dd 送信者のファーストネームか、メッセージが自分からのものであれば受信者
  ** .dt %X .dd 添付の数
  **            (可能な速度効果については ``$attachments'' の章を参照してください)
  ** .dt %y .dd 存在すれば ``X-Label:'' フィールド
  ** .dt %Y .dd \fI(1)\fP スレッドツリーの一部ではない、\fI(2)\fP スレッドの頂点である、
  **            \fI(3)\fP ``X-Label:'' が 前のメッセージの ``X-Label:'' と異なる、の
  **            いずれかで、存在していれば``X-Label:'' フィールド。
  ** .dt %Z .dd 3桁のメッセージ状態フラグ。
  **            最初の文字は new/read/replied フラグ (``n''/``o''/``r''/``O''/``N'')。
  **            2番目の文字は削除/暗号化フラグ (``D''/``d''/``S''/``P''/``s''/``K'')。
  **            3番目の文字はタグ/フラグのどちらか(``\(は''/``!'')か、$$to_chars 中にある
  **            文字のどれか。
  ** .dt %@name@ .dd ``$index-format-hook'' コマンドに一致する書式文字列を挿入して
  **                   評価
  ** .dt %{fmt} .dd メッセージの日付と時間が送信者のタイムゾーンに変換され、
  **                ``fmt'' がライブラリ関数\fCstrftime(3)\fP によって展開されます。
  **                先頭に感嘆符がつくものはロケールが無視されます。
  ** .dt %[fmt] .dd メッセージの日付と時間がローカルタイムゾーンに変換され、
  **                ``fmt'' がライブラリ関数\fCstrftime(3)\fP によって展開されます。
  **                先頭に感嘆符がつくものはロケールが無視されます。
  ** .dt %(fmt) .dd メッセージを受信したときのローカルの日付と時間。
  **                ``fmt'' はライブラリ関数\fCstrftime(3)\fP によって展開されます。
  **                先頭に感嘆符がつくものはロケールが無視されます
  ** .dt %<fmt> .dd 現在のローカルの日付と時刻。
  **                ``fmt'' はライブラリ関数\fCstrftime(3)\fP によって展開されます。
  **                先頭に感嘆符がつくものはロケールが無視されます
  ** .dt %>X    .dd 残りの文字列を右寄せして、間を ``X'' 部分の文字で詰めます。
  ** .dt %|X    .dd 行末まで ``X'' 部分の文字で詰めます。
  ** .dt %*X    .dd 文字 ``X'' を埋め草として、 soft-fillします。
  ** .de
  ** .pp
  ** mbox/mmdf では、``%l'' は非圧縮メッセージに対して適用され、maildir/mh では
  ** ``Lines:'' ヘッダフィールドが存在する場合に、そこから値は取得されることに
  ** 注意してください(意味は通常同じです)。従って、値は、異なったメッセージの
  ** パート中で使われるエンコーディングに依存し、実際にはほとんど意味がありません。
  ** .pp
  ** ``Soft-fill'' についてはいくつか説明が必要でしょう: 通常の右揃えは、``%>''
  ** の左側のものすべてをプリントし、余白がある場合にのみ、右側にあるものすべてと
  ** 埋め草文字を表示します。それと対照的に、soft-fill は右側を優先し、それを
  ** 表示する場所を保障し、まだ余裕がある場合にのみ、埋め草文字を表示します。
  ** 必要に応じて、soft-fill は右側のテキストのために空間を作るため、テキストの
  ** 左側を削除します。
  ** .pp
  ** これらの展開は``$save-hook'', ``$fcc-hook'', ``$fcc-save-hook'' と
  ** ``$index-format-hook'' でサポートされることに注意してください。
  ** .pp
  ** また、設定変数 $$attribution,
  ** $$forward_attribution_intro, $$forward_attribution_trailer,
  ** $$forward_format, $$indent_string, $$message_format, $$pager_format,
  ** と $$post_indent_string でもサポートされます。
  */
  { "ispell",		DT_PATH, R_NONE, {.p=&Ispell}, {.p=ISPELL} },
  /*
  ** .pp
  ** どのようにして ispell(GNU のスペリングチェックソフトウェア)を起動するかの指定です。
  */
  { "keep_flagged", DT_BOOL, R_NONE, {.l=OPTKEEPFLAGGED}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、フラグが付けられている既読メッセージはスプール
  ** メールボックスから $$mbox メールボックス、あるいは ``$mbox-hook'' コマンドの
  ** 結果に移動しません。
  */
  { "list_reply",	DT_QUAD, R_NONE, {.l=OPT_LISTREPLY}, {.l=MUTT_NO} },
  /*
  ** .pp
  ** 設定した場合、アドレスはもとのメッセージが来たメーリングリストに返信されます
  ** (代わりに作成者のみ)。この設定を ``ask-yes'' か ``ask-no'' にすると
  ** 本当に作成者のみに返信するかどうかを聞いてきます。
  */
  { "mail_check",	DT_NUM,  R_NONE, {.p=&BuffyTimeout}, {.l=5} },
  /*
  ** .pp
  ** この変数は、どのくらいの頻度(秒単位)で、Mutt が新規メールを探しに行くかを
  ** 設定します。$$timeout 変数も参照してください。
  */
  { "mail_check_recent",DT_BOOL, R_NONE, {.l=OPTMAILCHECKRECENT}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は、最後にメールボックスを開いたときから受け取った
  ** 新規メールについてのみ通知します。\fIunset\fP の場合、Mutt は、最近アクセスしたか否かに
  ** かかわらず、メールボックス中に新規メールが存在するかどうかを通知します。
  ** .pp
  ** fI$$mark_old\fP が設定されている場合、Mutt は、古いメッセージのみが存在している
  ** 場合、メールボックス中に新規メールが含まれているとは見なしません。
  */
  { "mail_check_stats", DT_BOOL, R_NONE, {.l=OPTMAILCHECKSTATS}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は新規メールのポーリング中に、定期的にメールボックスの
  ** メッセージ統計を計算します。未読、フラグ付き、および合計メッセージ数をチェック
  ** します。この操作は多くの能力を集中的に使うため、既定では \fIunset\fP であり、
  ** これらのカウントを更新する頻度を制御するための、$$mail_check_stats_interval
  ** という別のオプションがあります。
  ** .pp
  ** メッセージの統計情報は \fC<check-stats>\fP 機能を起動することにより、明示的に
  ** 計算することも出来ます。
  */
  { "mail_check_stats_interval", DT_NUM, R_NONE, {.p=&BuffyCheckStatsInterval}, {.l=60} },
  /*
  ** .pp
  ** $$mail_check_stats が \fIset\fP の場合、この変数はどのくらいの頻度(秒単位)で
  ** メッセージ数を Mutt が更新するかを設定します。
  */
  { "mailcap_path",	DT_STR,	 R_NONE, {.p=&MailcapPath}, {.p=0} },
  /*
  ** .pp
  ** この変数は、Mutt で直接サポートされない MIME の本文を表示しようとする際に、
  ** 参照するファイルを指定します。既定値は起動時に生成されます。マニュアルの
  ** ``$mailcap'' 節を参照してください。
  */
  { "mailcap_sanitize",	DT_BOOL, R_NONE, {.l=OPTMAILCAPSANITIZE}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は、%拡張を、明確に定義した安全な文字セットにすることで、
  ** メールボックス中で使用できる文字を制限します。これは安全な設定ですが、
  ** これにより高度な MIME 項目が壊れないとは断言できません。
  ** .pp
  ** \fIこの設定は、完全に理解することなく変更してはなりません!\fP
  */
#ifdef USE_HCACHE
  { "maildir_header_cache_verify", DT_BOOL, R_NONE, {.l=OPTHCACHEVERIFY}, {.l=1} },
  /*
  ** .pp
  ** ヘッダキャッシュが使われている時に、maildirファイルを変更した Mutt 以外の
  ** Maildir に対応しないプログラムを確認します。これは、フォルダのオープン毎に
  ** メッセージ毎に1回の\fCstat(2)\fP が発生します(これは NFS フォルダの場合には
  ** 非常に遅くなる可能性があります)。
  */
#endif
  { "maildir_trash", DT_BOOL, R_NONE, {.l=OPTMAILDIRTRASH}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、削除マークが付いたメッセージは、削除される代わりに
  ** maildir に ゴミ フラグを付けてセーブされます。\fB注意:\fP これは、
  ** maildir 形式のメールボックスに対してのみ適用されます。設定は他のメールボックス
  ** 形式には影響しません。
  */
  { "maildir_check_cur", DT_BOOL, R_NONE, {.l=OPTMAILDIRCHECKCUR}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は新規メッセージについて、maildir フォルダの new と
  ** cur ディスクの両方をポーリングします。これは、フォルダを操作する他のプログラム
  ** (例えば dovecot)が新規メッセージを cur ディレクトリに移動する場合に
  ** 便利です。このオプションを設定すると、大きなフォルダの場合には
  ** Mutt がすべての cur メッセージをスキャンするために、新規メッセージの
  ** ポーリングが遅くなる可能性があることに注意してください。
  */
  { "mark_macro_prefix",DT_STR, R_NONE, {.p=&MarkMacroPrefix}, {.p="'"} },
  /*
  ** .pp
  ** メッセージのマークに使う時に作成されるマクロ用のプリフィックス。
  ** \fI<mark-message>a\fP で、自動的に生成される新規マクロは
  ** このプレフィックスと文字  \fIa\fP から構成されます。
  */
  { "mark_old",		DT_BOOL, R_BOTH, {.l=OPTMARKOLD}, {.l=1} },
  /*
  ** .pp
  ** Mutt が \fI新規で\fP \fB未読の\fP メッセージを読まないでメールボックスから出るときに
  ** \fIold\fP を付けるかどうかを制御します。\fIset\fP の場合、次回 Mutt を起動した
  ** 時から、インデックスメニュー中で、メッセージの隣に ``O'' を付けて表示され、
  ** それが古いものであることを示します。 
  */
  { "markers",		DT_BOOL, R_PAGER_FLOW, {.l=OPTMARKERS}, {.l=1} },
  /*
  ** .pp
  ** 内部ページャで折り返される行の表示を制御します。設定した場合、
  ** ``+'' と言うマーカーが、折り返された行の先頭に表示されます。
  ** .pp
  **  $$smart_wrap 変数も参照してください。
  */
  { "mask",		DT_RX,	 R_NONE, {.p=&Mask}, {.p="!^\\.[^.]"} },
  /*
  ** .pp
  ** ファイルブラウザ中で使う正規表現で、\fInot\fP 演算子 ``!'' を先頭に
  ** 付けることも出来ます。この条件に名前が一致するファイルのみが表示されます。
  ** 一致条件は常時大文字小文字を区別します。
  */
  { "mbox",		DT_PATH, R_BOTH, {.p=&Inbox}, {.p="~/mbox"} },
  /*
  ** .pp
  ** これは、$$spoolfile フォルダ中の既読メールを追加するフォルダです。
  ** .pp
  ** $$move 変数も参照してください。
  */
  { "mbox_type",	DT_MAGIC,R_NONE, {.p=&DefaultMagic}, {.l=MUTT_MBOX} },
  /*
  ** .pp
  ** 新規フォルダを作る際の既定のメールボックスタイプ。``mbox'', ``MMDF'', ``MH'' と
  ** ``Maildir'' のどれかです。これは、\fC-m\fP コマンド行オプションで上書きできます。
  */
  { "menu_context",	DT_NUM,  R_NONE, {.p=&MenuContext}, {.l=0} },
  /*
  ** .pp
  ** この変数は、メニューをスクロールするときに表示されるコンテキストの行数を制御
  ** します($$pager_context に似ています)。
  */
  { "menu_move_off",	DT_BOOL, R_NONE, {.l=OPTMENUMOVEOFF}, {.l=1} },
  /*
  ** .pp
  ** \fIunset\fP の場合、行数よりもエントリ数が小さい場合を除き、メニュー最下部の
  ** エントリは、画面の最下部を越えてスクロールアップしません。\fIset\fPの場合、
  ** 最下部のエントリは最下部から動く可能性があります。
  ** 
  */
  { "menu_scroll",	DT_BOOL, R_NONE, {.l=OPTMENUSCROLL}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、画面の境界を越えて移動しようとする場合、メニューは1行ずつ
  ** 上下にスクロールします。\fIunset\fP の場合、画面は消去されて次又は前のメニューページ
  ** が表示されます(大量の再描画を防ぐために、遅い回線の場合には便利です)。
  */
#if defined(USE_IMAP) || defined(USE_POP)
  { "message_cache_clean", DT_BOOL, R_NONE, {.l=OPTMESSAGECACHECLEAN}, {.l=0} },
  /*
  ** .pp
  **  \fIset\fP の場合、Mutt はメールボックスが同期されたときに、メッセージキャッシュから
  ** 無効になったエントリを削除します。 (特に大きなフォルダでは)。若干遅くなるため、
  ** たまに設定したいと思う程度でしょう
  **
  */
  { "message_cachedir",	DT_PATH,	R_NONE,	{.p=&MessageCachedir}, {.p=0} },
  /*
  ** .pp
  ** これをディレクトリに設定すると、Mutt はIMAP と POP サーバからメッセージのコピーを
  ** ここにキャッシュします。このエントリはいつでも削除できます。
  ** .pp
  ** この変数をディレクトリに設定すると、Mutt はすべてのリモートメッセージを1回だけ
  ** フェッチする必要があり、ローカルフォルダと同じくらいの速さで正規表現による
  ** 検索を実行できます。
  ** .pp
  **  $$message_cache_clean 変数も参照してください。
  */
#endif
  { "message_format",	DT_STR,	 R_NONE, {.p=&MsgFmt}, {.p="%s"} },
  /*
  ** .pp
  ** これは、\fCmessage/rfc822\fP タイプの添付用 ``attachment'' メニューで
  ** 表示される文字列です。\fCprintf(3)\fP 風の定義されている完全な書式リストは
  ** $$index_format 節を参照してください。
  */
  { "msg_format",	DT_SYN,  R_NONE, {.p="message_format"}, {.p=0} },
  /*
  */
  { "meta_key",		DT_BOOL, R_NONE, {.l=OPTMETAKEY}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、最上位ビット(ビット 8) を設定したキーストロークを、ESC キーを
  ** 押し、残りのものはすべて最上位ビットを落とした形で Mutt に解釈させます。たとえば、
  ** 押されたキーが ASCII の値で \fC0xf8\fP の場合は、 ESC の次に ``x'' が押された
  ** ように扱われます。これは、最上位ビットを落とした \fC0xf8\fP が、\fC0x78\fP で、
  ** それは ASCII 文字 ``x'' だからです。
  */
  { "metoo",		DT_BOOL, R_NONE, {.l=OPTMETOO}, {.l=0} },
  /*
  ** .pp
  ** \fIunset\fP の場合、Mutt はメッセージ返信時に受信者リストからアドレスを
  ** 取り除きます(``$alternates'' コマンドを参照してください)。
  */
  { "mh_purge",		DT_BOOL, R_NONE, {.l=OPTMHPURGE}, {.l=0} },
  /*
  ** .pp
  ** \fIunset\fP の場合、Mutt は mh の挙動を偽装し、実際に削除する代わりに mh フォルダ中で
  ** 削除されたメッセージを \fI,<old file name>\fP に改名します。これは、メッセージを
  ** ディスク上に残しますが、プログラムがフォルダを読むときに無視させます。変数が
  ** \fIset\fP の場合、メッセージファイルは単に削除されます。
  ** .pp
  ** このオプションは、Maildir フォルダ用の $$maildir_trash と似ています。
  */
  { "mh_seq_flagged",	DT_STR, R_NONE, {.p=&MhFlagged}, {.p="flagged"} },
  /*
  ** .pp
  ** フラグ付きメッセージに使われる MH シーケンス名です。
  */
  { "mh_seq_replied",	DT_STR, R_NONE, {.p=&MhReplied}, {.p="replied"} },
  /*
  ** .pp
  ** 返信済みメッセージに使われる MH シーケンス名です。
  */
  { "mh_seq_unseen",	DT_STR, R_NONE, {.p=&MhUnseen}, {.p="unseen"} },
  /*
  ** .pp
  ** 未読メッセージに使われる MH シーケンス名です。
  */
  { "mime_forward",	DT_QUAD, R_NONE, {.l=OPT_MIMEFWD}, {.l=MUTT_NO} },
  /*
  ** .pp
  ** \fIset\fP の場合、転送するメッセージは、メッセージの本体中に含める代わりに、
  ** 分離された \fCmessage/rfc822\fP MIME パートとして添付されます。これは、
  ** MIME メッセージの転送に便利で、こうすることにより、受信者は自分に送られてきた
  ** のと同じように正しくメッセージを閲覧できます。MIME と 非MIME をメール毎に
  ** 切り換えたいのであれば、この変数を ``ask-no'' 又は ``ask-yes'' にします。
  ** .pp
  ** $$forward_decode と $$mime_forward_decode も参照してください。
  */
  { "mime_forward_decode", DT_BOOL, R_NONE, {.l=OPTMIMEFORWDECODE}, {.l=0} },
  /*
  ** .pp
  ** $$mime_forward が \fIset\fP の時にメッセージを転送する時、複雑な MIME メッセージを
  ** \fCtext/plain\fP にデコードするかを制御します。それ以外は、代わりに
  ** $$forward_decode が使われます。
  */
  { "mime_fwd",		DT_SYN,  R_NONE, {.p="mime_forward"}, {.p=0} },
  /*
  */
  { "mime_forward_rest", DT_QUAD, R_NONE, {.l=OPT_MIMEFWDREST}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** 添付メニューから、 MIME メッセージの複数の添付を転送する時、このオプションが
  ** \fIset\fP の時、合理的な方法でデコードできない添付は新しく編集された
  ** メッセージに添付されます。
  */
  { "mime_type_query_command", DT_STR, R_NONE, {.p=&MimeTypeQueryCmd}, {.p=0} },
  /*
  ** .pp
  ** これは、メッセージを編集するときに新しい添付の MIME タイプを決定するために
  ** 動かすコマンドを指定します。$$mime_type_query_first が設定されている場合を除き、
  ** これは mime.types ファイル中に添付の拡張子が見つからない場合にのみ実行されます。
  ** .pp
  ** 文字列には ``%s'' を含むことが出来、これは、添付ファイル名に置換されます。
  ** Mutt はシェルの引用ルールに沿って、自動的に ``%s'' を置き換えた文字列の回りに
  ** 引用符を追加するので、個別に囲むことは必要ありません。文字列中に ``%s'' が
  ** ない場合は、Mutt は文字列の最後に添付ファイル名を追加します。
  ** .pp
  ** コマンドは添付の MIME タイプを含む単一行を出力しなければなりません。
  ** .pp
  ** 推奨される値は、 ``xdg-mime query filetype'' または ``file -bi'' です。
  ** ``file -bi''.
  */
  { "mime_type_query_first", DT_BOOL, R_NONE, {.l=OPTMIMETYPEQUERYFIRST}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、$mime_type_query_command は mime.types 検索の前に
  ** 実行されます。
  */
#ifdef MIXMASTER
  { "mix_entry_format", DT_STR,  R_NONE, {.p=&MixEntryFormat}, {.p="%4n %c %-16s %a"} },
  /*
  ** .pp
  ** この変数は、mixmaster チェーン選択画面でのリメーラ行の書式を記述します。
  ** 以下の、\fCprintf(3)\fP 風の書式がサポートされます。
  ** .dl
  ** .dt %n .dd メニュー上での実行番号
  ** .dt %c .dd リメーラのケーパビリティ
  ** .dt %s .dd リメーラの短縮名
  ** .dt %a .dd リメーラのメールアドレス
  ** .de
  */
  { "mixmaster",	DT_PATH, R_NONE, {.p=&Mixmaster}, {.p=MIXMASTER} },
  /*
  ** .pp
  ** この変数は、システム上の Mixmaster バイナリへのパスを記述します。
  ** これは、種々のパラメータを付けた、既知のリメーラのリストを集めるためと、
  ** 最終的には、mixmaster チェーンを使ってメッセージを送信するのに使われます。
  */
#endif
  { "move",		DT_QUAD, R_NONE, {.l=OPT_MOVE}, {.l=MUTT_NO} },
  /*
  ** .pp
  ** Mutt が既読メッセージをスプールメールボックスから、$$mbox メールボックス
  ** か、``$mbox-hook'' コマンドの結果に移動するか否かを制御します。
  */
  { "narrow_tree",	DT_BOOL, R_TREE|R_INDEX, {.l=OPTNARROWTREE}, {.l=0} },
  /*
  ** .pp
  ** この変数は、\fIset\fPの時、深いスレッドを画面上に収まるように、スレッドツリーを
  ** 狭くします。
  */
#ifdef USE_SOCKET
  { "net_inc",	DT_NUM,	 R_NONE, {.p=&NetInc}, {.l=10} },
  /*
  ** .pp
  ** ネットワーク越しに大量のデータを転送することを予定している操作は、
  ** 処理状況を $$net_inc キロバイト毎に更新します。0 に設定すると、
  ** 処理状況メッセージは表示されません。
  ** .pp
  ** $read_inc, $$write_inc と $$net_inc も参照してください。
  */
#endif
  { "new_mail_command",	DT_PATH, R_NONE, {.p=&NewMailCmd}, {.p=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は新規メッセージを受信後にこのコマンドを呼び出します。
  ** このコマンド中にフォーマットできうる値については、$$status_formatの説明を
  ** 参照してください。
  */
  { "pager",		DT_PATH, R_NONE, {.p=&Pager}, {.p="builtin"} },
  /*
  ** .pp
  ** この変数は、メッセージを表示するのに使うページャを指定します。``builtin''
  ** と言う値は、内蔵ページャを使う事を意味し、それ以外は、使用したい外部ページャの
  ** パスを指定します。
  ** .pp
  ** 外部ページャを使う場合には若干弱点があります。ページャから直接 Mutt の
  ** 機能を呼び出せないために、追加のキー操作が必要で、さらに、画面のリサイズを
  ** すると、ヘルプメニュー中で、画面の幅より長い行は不正にレイアウトされてしまいます。
  ** .pp
  ** 外部ページャを使う場合は、既定で \fIset\fP になっている $$prompt_after も
  ** 参照してください。
  */
  { "pager_context",	DT_NUM,	 R_NONE, {.p=&PagerContext}, {.l=0} },
  /*
  ** .pp
  ** この変数は、内部ページャで、次または前のページを表示するときに、コンテキストと
  ** して残す行数を制御します。既定では、Mutt は画面上の最後の行の次の行を
  ** 次のページの最上位に表示します(コンテキストが 0行)。
  ** .pp
  ** この変数は検索の結果のコンテキスト量も指定します。値が正ならば、一致する前の
  ** 行数が表示され、0 の場合は、一致は上揃えとなります。
  */
  { "pager_format",	DT_STR,	 R_PAGER, {.p=&PagerFmt}, {.p="-%Z- %C/%m: %-20.20n   %s%*  -- (%P)"} },
  /*
  ** .pp
  ** この変数は、内部又は外部ページャ中の各メッセージの前に表示される、
  ** 1行の ``status'' メッセージの書式を制御します。正しい書式は $$index_format
  ** 節に記してあります。
  */
  { "pager_index_lines",DT_NUM,	 R_PAGER, {.p=&PagerIndexLines}, {.l=0} },
  /*
  ** .pp
  ** ページャ中に表示されるミニインデックスの行数を指定します。フォルダの
  ** 上端や下端にいる場合を除き、現在のメッセージは好みにインデックス画面の中で、
  ** おおよそ1/3の位置にあり、メッセージの前後に数行がコンテキストとして、
  ** メッセージを読むときに表示されます。これは、たとえば、現在のスレッドに
  ** どのくらい未読が残っているかを判断するのに便利です。行のうち1行は、
  ** インデックスのステータスバーとして予約されているので、6 に設定すると、
  ** 実際のインデックスでは 5 行のみが表示されます。現在のフォルダ中のメッセージ数が
  ** $$pager_index_lines より少ない場合は、インデックスは必要な数の行だけを
  ** 使用します。
  */
  { "pager_stop",	DT_BOOL, R_NONE, {.l=OPTPAGERSTOP}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、内部ページャは、メッセージの最後にいて、\fC<next-page>\fP 
  ** 機能を呼び出しても、次のメッセージに移動\fBしません\fP。
  */
  { "pgp_auto_decode", DT_BOOL, R_NONE, {.l=OPTPGPAUTODEC}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は、ユーザがメッセージの内容を操作するような通常の操作を
  ** 実行しようとすると、従来の PGP メッセージを自動的に復号化しようとします。たとえば、
  ** \fC$<check-traditional-pgp>\fP 機能を使って手動でチェックされていない
  ** 従来の PGP メッセージを表示した場合、Mutt は自動的に、メッセージが従来の PGP かを
  ** チェックします。
  */
  { "pgp_create_traditional",	DT_SYN, R_NONE, {.p="pgp_autoinline"}, {.p=0} },
  { "pgp_autoinline",		DT_BOOL, R_NONE, {.l=OPTPGPAUTOINLINE}, {.l=0} },
  /*
  ** .pp
  ** このオプションは、特定の環境で、古いスタイルのインライン(旧来の)PGP 暗号化または
  ** 署名されたメッセージを生成するかどうかを制御します。これは インラインが不要の場合、
  ** PGP メニューを使う事で上書きできます。GPGME バックエンドはこのオプションを
  ** サポートしません。
  ** .pp
  ** Mutt は、MIME パートが1つより多く存在している場合、メッセージに対して、
  ** 自動的に PGP/MIME を使用する場合があることに注意してください。Mutt は
  ** インライン(従来型) が動作しない場合に、PGP/MIME メッセージを送信する前に
  ** 問合せをするように設定できます。
  ** .pp
  ** $$pgp_mime_auto 変数も参照してください。
  ** .pp
  ** また、古い形式の PGP メッセージ形式を使う事は\fB強く\fP \fB非推奨に\fP
  ** なっていることにも注意してください。
  ** (PGP のみです)
  */
  { "pgp_check_exit",	DT_BOOL, R_NONE, {.l=OPTPGPCHECKEXIT}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は署名又は暗号化時に PGP サブプロセスの終了コードを
  ** チェックします。ゼロでない終了コードはサブプロセスが異常終了したことを
  ** 意味します。
  ** (PGP のみです)
  */
  { "pgp_check_gpg_decrypt_status_fd", DT_BOOL, R_NONE, {.l=OPTPGPCHECKGPGDECRYPTSTATUSFD}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fPの場合、Mutt は GnuPG ステータスコードが復号が成功したことを表示する
  **  $$pgp_decrypt_command と $$pgp_decode_command コマンドののステータスファイル記述子
  ** 出力をチェックします。これは DECRYPTION_OKAY の存在、DECRYPTION_FAILED の不在および
  ** BEGIN_DECRYPTION と END_DECRYPTION ステータスコードの間にすべての
  ** PLAINTEXT が存在することをチェックします。
  ** .pp
  ** \fIunset\fP の場合、Mutt は $$pgp_decryption_okay と fd 出力のステータスを
  ** 代わりに照合します。
  ** (PGP のみです)
  */
  { "pgp_clearsign_command",	DT_STR,	R_NONE, {.p=&PgpClearSignCommand}, {.p=0} },
  /*
  ** .pp
  ** このフォーマットは、旧形式の ``clearsigned'' な PGP メッセージを作成するのに
  ** 使います。このフォーマットの使用は \fB強く\fP \fB非推奨\fP であることに
  ** 注意してください。
  ** 
  ** .pp
  ** これはフォーマット文字列です。使える\fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** (PGP only)
  */
  { "pgp_decode_command",       DT_STR, R_NONE, {.p=&PgpDecodeCommand}, {.p=0} },
  /*
  ** .pp
  ** このフォーマット文字列は、application/pgp 添付を復号するために使われる
  ** コマンドを指定します。
  ** .pp
  ** PGP コマンドフォーマットは \fCprintf(3)\fP 風の固有の書式があります。
  ** .dl
  ** .dt %p .dd パスフェーズが必要な場合に 任意の空白文字列などをPGPPASSFD=0 に
  **            展開します。注意: これは %? も使うことができます。
  ** .dt %f .dd メッセージを含むファイルの名前を展開します。
  ** .dt %s .dd 検証するときに \fCmultipart/signed\fP 添付の署名パートを含む
  **            ファイル名を展開します。
  ** .dt %a .dd 設定されている場合、$$pgp_sign_as の値で、そのほかは、
  **            $$pgp_default_key の値。
  ** .dt %r .dd 1つ以上の キー ID (あるいはもしもあればフィンガープリント)
  ** .de
  ** .pp
  ** PGP の種々のバージョンに対してこれらのフォーマットをどのように設定するかの例は
  ** ドキュメントといっしょにシステムにインストールされている the \fCsamples/\fP
  ** サブディレクトリ にある、pgp と gpg 設定ファイルの例を参照してください。
  ** (PGP のみです)
  */
  { "pgp_decrypt_command", 	DT_STR, R_NONE, {.p=&PgpDecryptCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは PGP で暗号化されたメッセージの復号に使います。
  ** .pp
  ** これはフォーマット文字列で、使用できる \fCprintf(3)\fP 風の書式は
  ** $$pgp_decode_command を参照してください。
  ** (PGP のみです)
  */
  { "pgp_decryption_okay",	DT_RX,  R_NONE, {.p=&PgpDecryptionOkay}, {.p=0} },
  /*
  ** .pp
  ** この変数にテキストを割り当てた場合、暗号化された PGP メッセージは、
  ** $$pgp_decrypt_command からの出力がテキストを含んでいるときのみ、
  ** 復号が成功したとします。これは、multipart/encrypted ヘッダがあるが、
  ** ブロックが完全には暗号化されていないブロックを含む場合、暗号化された
  ** メッセージの盗聴を防ぐのに便利です(すなわち、単純に署名されて、ASCII で
  ** 防御されたテキスト)。
  ** .pp
  ** $$pgp_check_gpg_decrypt_status_fd が設定されている場合、この変数は
  ** 無視されることに注意してください。
  ** (PGP のみです)
  */
  { "pgp_self_encrypt_as",	DT_SYN,  R_NONE, {.p="pgp_default_key"}, {.p=0} },
  { "pgp_default_key",		DT_STR,	 R_NONE, {.p=&PgpDefaultKey}, {.p=0} },
  /*
  ** .pp
  ** これは PGP 操作に使う、既定のキーペアです。これは暗号化の時に使います
  ** ($$postpone_encrypt と $$pgp_self_encrypt を参照してください)。
  ** .pp
  ** $$pgp_sign_as が設定されていない限り、署名にも使われます。
  ** .pp
  ** The (now deprecated) \fIpgp_self_encrypt_as\fP is an alias for this
  ** variable, and should no longer be used.
  ** (現在は非推奨の) fIpgp_self_encrypt_as\fP はこの変数の別名ですが、
  ** もはや使われていません。
  ** (PGP のみです)
  */
  { "pgp_encrypt_only_command", DT_STR, R_NONE, {.p=&PgpEncryptOnlyCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、署名なしで本文を暗号化するために使います。
  ** .pp
  ** これはフォーマット文字列で、使用可能な \fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** (PGP のみです)
  */
  { "pgp_encrypt_sign_command",	DT_STR, R_NONE, {.p=&PgpEncryptSignCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、本文の署名と暗号化両方に使います。
  ** .pp
  ** これはフォーマット文字列で、使用可能な \fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** (PGP のみです)
  */
  { "pgp_entry_format", DT_STR,  R_NONE, {.p=&PgpEntryFormat}, {.p="%4n %t%f %4l/0x%k %-4a %2c %u"} },
  /*
  ** .pp
  ** この変数は、好みに応じて PGP キー選択メニューをカスタマイズ出来るようにします。
  ** この文字列は$$index_format と似ていますが、\fCprintf(3)\fP 風の固有の書式を
  ** 持っています。
  ** .dl
  ** .dt %n     .dd 番号
  ** .dt %k     .dd キー id
  ** .dt %u     .dd user id
  ** .dt %a     .dd アルゴリズム
  ** .dt %l     .dd キー長
  ** .dt %f     .dd フラグ
  ** .dt %c     .dd ケーパビリティ
  ** .dt %t     .dd key-uid アソシエーションの 信頼性/有効性
  ** .dt %[<s>] .dd \fCstrftime(3)\fP 形式のキーの日付
  ** .de
  ** .pp
  ** (PGP のみです)
  */
  { "pgp_export_command", 	DT_STR, R_NONE, {.p=&PgpExportCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、ユーザのキーリングから公開鍵をエクスポートするのに使われます。
  ** .pp
  ** これはフォーマット文字列で、使用可能な \fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** (PGP のみです)
  */
  { "pgp_getkeys_command",	DT_STR, R_NONE, {.p=&PgpGetkeysCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、Mutt が、メールアドレスに関連づけられた公開鍵をフェッチする
  ** 必要がある場合はいつでも起動されます。$$pgp_decode_command によってサポート
  ** される書式のうち、%r は、このフォーマットだけで使われる書式です。
  ** この場合、%r は メールアドレスに展開されますが、公開鍵のID ではないことに注意して
  ** ください(キーID は不明なので、このことが、Mutt がこのコマンドを起動する理由です)。
  ** (PGP のみです)
  */
  { "pgp_good_sign",	DT_RX,  R_NONE, {.p=&PgpGoodSign}, {.p=0} },
  /*
  ** .pp
  ** この変数にテキストを割り当てた場合、$$pgp_verify_command からの出力が
  ** テキストの場合にのみ、PGP 署名 は検証されていると見なされます。不正な
  ** 署名であっても、コマンドの終了コードが 0 の場合はこの変数を使います。
  ** (PGP のみです)
  */
  { "pgp_ignore_subkeys", DT_BOOL, R_NONE, {.l=OPTPGPIGNORESUB}, {.l=1} },
  /*
  ** .pp
  ** この変数を設定すると、Mutt は OpenPGP 副キーを無視するようになります。代わりに、
  ** プリンシパルキー に副キーのケーパビリティが継承されます。興味深いキー選択
  ** ゲームを楽しみたい場合は、これを \fIUnset\fP にします。
  ** (PGP のみです)
  */
  { "pgp_import_command",	DT_STR, R_NONE, {.p=&PgpImportCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、ユーザの公開キーリングにメッセージからキーをインポートする
  ** のに使います。
  ** .pp
  ** これはフォーマット文字列で、取り得る\fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** (PGP のみです)
  */
  { "pgp_list_pubring_command", DT_STR, R_NONE, {.p=&PgpListPubringCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、公開キーリングの内容を一覧表示するのに使います。
  ** 出力形式は以下で使用しているものに似ている必要があります。
  ** .ts
  ** gpg --list-keys --with-colons --with-fingerprint
  ** .te
  ** .pp
  ** このフォーマットは、Mutt 由来の \fCmutt_pgpring\fP ユーティリティによっても
  ** 生成されます。
  ** .pp
  ** 注意: gpg の \fCfixed-list-mode\fP オプションは使ってはなりません。これは、
  ** Mutt 中で不正なキー生成日付を表示る結果となる異なった日付形式を生成します。
  ** .pp
  ** これはフォーマット文字列で、取り得る\fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** この場合、 %r は、メールアドレス、名前、あるいはキーIDのような、1つ以上のクォート
  ** された文字列の一覧である検索文字列に展開されることに注意してください。
  ** (PGP のみです)
  */
  { "pgp_list_secring_command",	DT_STR, R_NONE, {.p=&PgpListSecringCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは秘密鍵リングの内容を一覧表示するのに使います。
  ** 出力形式は以下で使用しているものに似ている必要があります。
  ** .ts
  ** gpg --list-keys --with-colons --with-fingerprint
  ** .te
  ** .pp
  ** このフォーマットは、Mutt 由来の \fCmutt_pgpring\fP ユーティリティによっても
  ** 生成されます。
  ** .pp
  ** 注意: gpg の \fCfixed-list-mode\fP オプションは使ってはなりません。これは、
  ** Mutt 中で不正なキー生成日付を表示る結果となる異なった日付形式を生成します。
  ** .pp
  ** これはフォーマット文字列で、取り得る\fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** この場合、 %r は、メールアドレス、名前、あるいはキーIDのような、1つ以上のクォート
  ** された文字列の一覧である検索文字列に展開されることに注意してください。
  ** (PGP のみです)
  */
  { "pgp_long_ids",	DT_BOOL, R_NONE, {.l=OPTPGPLONGIDS}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、64ビットの PGP キーID を使い、\fIunset\fP の場合、通常の
  ** 32ビットキーIDを使います。注意:内部的に、Mutt はフィンガープリント(あるいは
  ** フォールバックとしての長いキーID)の使用に移行しました。このオプションは、
  ** キー選択メニューでの内容の表示やいくつかの他の場所での制御にのみ使われます。
  ** (PGP のみです)
  */
  { "pgp_mime_auto", DT_QUAD, R_NONE, {.l=OPT_PGPMIMEAUTO}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** このオプションは、インライン(従来型の)で(何らかの理由で)うまくいかなかった
  ** 場合、PGP/MIME を使う(署名/暗号化)メッセージの自動的な送信に対して
  ** Mutt が問合せをするかどうかを制御します。
  ** .pp
  ** 古い形式の PGP メッセージ形式の使用は \fB強く\fP \fB非推奨\fP であることにも
  ** 注意してください。
  ** (PGP のみです)
  */
  { "pgp_auto_traditional",	DT_SYN, R_NONE, {.p="pgp_replyinline"}, {.p=0} },
  { "pgp_replyinline",		DT_BOOL, R_NONE, {.l=OPTPGPREPLYINLINE}, {.l=0} },
  /*
  ** .pp
  ** この変数を設定すると、Mutt は、インラインでPGPで暗号化/署名されたメッセージに
  ** 返信するときに、インライン(従来型の)メッセージを作成しようとします。
  ** これは、インラインが不要だった場合、PGPメニューを使うことによって
  ** 上書きできます。このオプションは(返信された)メッセージがインラインかを
  ** 自動的には検出しません。そのかわり、以前にチェック/フラグを付けたメッセージの
  ** Mutt 内部状態に依存します。
  ** .pp
  ** Mutt は MIME パートが複数存在する場合、メッセージに対して 自動的に PGP/MIME
  ** を使う事があることに注意してください。Mutt は、インライン(従来型)が動かない
  ** 場合に、PGP/MIMEメッセージを送信する前に問合せをするように設定できます。
  ** .pp
  ** $$pgp_mime_auto 変数も参照してください。
  ** .pp
  ** 古い形式の PGP メッセージ形式の使用は \fB強く\fP \fB非推奨\fP であることにも
  ** 注意してください。
  ** (PGP のみです)
  **
  */
  { "pgp_retainable_sigs", DT_BOOL, R_NONE, {.l=OPTPGPRETAINABLESIG}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、署名かつ暗号化されたメッセージは、ネストされた
  ** \fCmultipart/signed\fP と \fCmultipart/encrypted\fP 本体部分で構成されます。
  ** .pp
  ** これは外部レイヤ(\fCmultipart/encrypted\fP)が簡単に削除でき、内部の
  ** \fCmultipart/signed\fP パートが残るので、暗号化され署名された
  ** メーリングリストのようなアプリケーションには便利です。
  ** (PGP のみです)
  */
  { "pgp_self_encrypt",    DT_BOOL, R_NONE, {.l=OPTPGPSELFENCRYPT}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、PGP 暗号化メッセージは $$pgp_default_key 中のキーを
  ** 使う事で暗号化も行います。
  ** (PGP のみです)
  */
  { "pgp_show_unusable", DT_BOOL, R_NONE, {.l=OPTPGPSHOWUNUSABLE}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は PGP キー選択メニュー上で使えないキーを表示します。
  ** これは、取り消されたもの、満了したもの、あるいは、ユーザによって、``disabled'' と
  ** マークを付けられたものを含みます。
  ** (PGP のみです)
  */
  { "pgp_sign_as",	DT_STR,	 R_NONE, {.p=&PgpSignAs}, {.p=0} },
  /*
  ** .pp
  ** 署名のための異なったキーペアがある場合、署名するキーを、これを使って指定
  ** しなければなりません。ほとんどの場合は、$$pgp_default_key を設定しなければ
  ** ならないだけです。キーを指定するためにキーID形式を使う事を推奨します
  ** (たとえば\fC0x00112233\fP)。
  ** (PGP のみです)
  */
  { "pgp_sign_command",		DT_STR, R_NONE, {.p=&PgpSignCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、\fCmultipart/signed\fP PGP/MIME 本体部分のために、PGP 分離署名を
  ** 作成するために使われます。
  ** .pp
  ** これはフォーマット文字列で、取り得る\fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** (PGP のみです)
  */
  { "pgp_sort_keys",	DT_SORT|DT_SORT_KEYS, R_NONE, {.p=&PgpSortKeys}, {.l=SORT_ADDRESS} },
  /*
  ** .pp
  ** PGP メニュー中のエントリをどのように整列するかを指定します。
  ** 取り得る値は以下の通りです:
  ** .dl
  ** .dt address .dd ユーザID のアルファベット順
  ** .dt keyid   .dd キーID のアルファベット順
  ** .dt date    .dd キー作成日順
  ** .dt trust   .dd キーの trust 順
  ** .de
  ** .pp
  ** 上記の値を逆順にしたい場合は、``reverse-'' という接頭辞を付けます。
  ** (PGP のみです)
  */
  { "pgp_strict_enc",	DT_BOOL, R_NONE, {.l=OPTPGPSTRICTENC}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は自動的に PGP/MIME 署名メッセージを quoted-printable で
  ** エンコードします。この変数を設定しない場合、検証できない PGP 署名で
  ** 問題が出てくるかもしれないので、変更することの内容が割っている場合にのみ
  ** 変更してください。
  ** (PGP のみです)
  */
  { "pgp_timeout",	DT_LNUM,	 R_NONE, {.p=&PgpTimeout}, {.l=300} },
  /*
  ** .pp
  ** 使用していない場合の、キャッシュされたパスフレーズが満了するまでの、
  ** 秒数です。
  ** (PGP のみです)
  */
  { "pgp_use_gpg_agent", DT_BOOL, R_NONE, {.l=OPTUSEGPGAGENT}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は、秘密鍵パスフレーズプロンプトを扱うために、
  ** \fCgpg-agent(1)\fP プロセスを想定します。\fIunset\fP の場合、Mutt は
  ** パスフレーズ用のプロンプトを表示し、pgp コマンドに標準入力経由で渡します。
  ** .pp
  ** バージョン 2.1 以降、GnuPG は自動的にエージェントを起動し、パスフレーズ管理に
  ** エージェントを使う事を要求することに注意してください。このバージョンがどんどん
  ** 流行しているので、この変数の既定値は現在 \fIset\fP となっています。
  ** .pp
  ** Mutt は GUI または curses な pinentry プログラムとして動作します。
  ** TTY 形式の pinentry は使ってはなりません。
  ** .pp
  ** エージェントを動かせない古いバージョンの GnuPG を使っているか、
  ** エージェントがない他の暗号化プログラムを使っている場合は、この変数を
  ** \fIunset\fP にする必要があります。
  ** (PGP のみです)
  */
  { "pgp_verify_command", 	DT_STR, R_NONE, {.p=&PgpVerifyCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは PGP 署名を検証するのに使います。
  ** .pp
  ** これはフォーマット文字列で、取り得る\fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** (PGP のみです)
  */
  { "pgp_verify_key_command",	DT_STR, R_NONE, {.p=&PgpVerifyKeyCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、キー選択メニューからキー情報を検証するのに使います。
  ** .pp
  ** これはフォーマット文字列で、取り得る\fCprintf(3)\fP 風の書式については
  ** $$pgp_decode_command コマンドを参照してください。
  ** (PGP のみです)
  */
  { "pipe_decode",	DT_BOOL, R_NONE, {.l=OPTPIPEDECODE}, {.l=0} },
  /*
  ** .pp
  ** \fC<pipe-message>\fP コマンドに関連して使われます。\fIunset\fP の場合、
  ** Mutt は何らの前処理もせずにメッセージをパイプします。\fIset\fP の場合、
  ** Mutt はヘッダを間引き、最初にメッセージをデコードしようとします。
  */
  { "pipe_sep",		DT_STR,	 R_NONE, {.p=&PipeSep}, {.p="\n"} },
  /*
  ** .pp
  ** 外部 Unix コマンドにタグが付いたメッセージの一覧をパイプするときにメッセージ間に
  ** 付与されるセパレータです。
  */
  { "pipe_split",	DT_BOOL, R_NONE, {.l=OPTPIPESPLIT}, {.l=0} },
  /*
  ** .pp
  ** \fC<tag-prefix>\fPがある\fC<pipe-message>\fP 機能に関連して使われます。
  ** この変数が\fIunset\fP の時、タグが付いたメッセージの一覧をパイプするとき、
  ** Mutt はメッセージを結合し、すべて結合した形でパイプします。\fIset\fP の場合、
  ** Mutt はメッセージを1つずつパイプします。両方とも、メッセージは現在整列されている
  ** 順でパイプされ、各メッセージの後に $$pipe_sep separator が付与されます。
  */
#ifdef USE_POP
  { "pop_auth_try_all",	DT_BOOL, R_NONE, {.l=OPTPOPAUTHTRYALL}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt はすべての有効な認証方法を試します。
  ** \fIunset\fP の場合、Mutt は、以前の手法が無効だった場合にのみ、他の認証方法
  ** にフォールバックします。手法が有効だが認証に失敗した場合、Mutt は POP
  ** サーバに接続しません。
  */
  { "pop_authenticators", DT_STR, R_NONE, {.p=&PopAuthenticators}, {.p=0} },
  /*
  ** .pp
  ** これは、Mutt がPOPサーバにログインする時に使う、コロンで分離された認証方法の
  ** 一覧です。この順で Mutt は接続を試みます。認証方法は、
  ** ``user'', ``apop'' か他の SASL メカニズムで、例えば ``digest-md5'', ``gssapi''や
  ** ``cram-md5'' です。このオプションは大文字小文字を区別しません。
  ** このオプションが \fIunset\fP の場合(既定値)、Mutt はすべての有効な方法を
  ** もっともセキュアなものからそうでないものの順で試みます。
  ** .pp
  ** 例:
  ** .ts
  ** set pop_authenticators="digest-md5:apop:user"
  ** .te
  */
  { "pop_checkinterval", DT_NUM, R_NONE, {.p=&PopCheckTimeout}, {.l=60} },
  /*
  ** .pp
  ** この変数は、現在選択されているメールボックスが POP メールボックスだった場合、
  ** Mutt が新規メールを検索する感覚を(秒単位で)設定します。
  */
  { "pop_delete",	DT_QUAD, R_NONE, {.l=OPT_POPDELETE}, {.l=MUTT_ASKNO} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は \fC$<fetch-mail>\fP 機能を使って POP サーバからダウンロード
  ** 成功したメッセージを削除します。\fIunset\fP の場合は、Mutt はメッセージを
  ** ダウンロードしますが、POP サーバ上に残したままにします。
  */
  { "pop_host",		DT_STR,	 R_NONE, {.p=&PopHost}, {.p=0} },
  /*
  ** .pp
  ** \fC$<fetch-mail>\fP 機能で使う POP サーバの名前です。以下のように
  ** 代替ポート、ユーザ名、パスワードも指定することができます。
  ** .ts
  ** [pop[s]://][username[:password]@]popserver[:port]
  ** .te
  ** .pp
  ** ここで、 ``[...]'' はオプションの部分を意味します。
  */
  { "pop_last",		DT_BOOL, R_NONE, {.l=OPTPOPLAST}, {.l=0} },
  /*
  ** .pp
  ** この変数が\fIset\fP の場合、Mutt は、\fC$<fetch-mail>\fP 機能を使う時に、
  ** POP サーバから未読メッセージのみを検索するために、``\fCLAST\fP'' POP コマンドを
  ** 使おうとします。
  */
  { "pop_oauth_refresh_command", DT_STR, R_NONE, {.p=&PopOauthRefreshCmd}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、POPサーバへの接続時の認証のために OAUTH リフレッシュトークンを
  ** 生成するコマンドです。このコマンドは、OAUTHBEARER 認証メカニズムを使う
  ** 接続時毎に実行されます。詳細は``$oauth'' を参照してください。
  */
  { "pop_pass",		DT_STR,	 R_NONE, {.p=&PopPass}, {.p=0} },
  /*
  ** .pp
  ** POP アカウントのパスワードを指定します。\fIunset\fP の場合、 POP メールボックスを
  ** 開くときにパスワードの入力を求めます。
  ** .pp
  ** \fB警告\fP: このオプションは、自分自身しか読めないファイルであっても、スーパーユーザが
  ** 読むことが出来るため、確実に安全なマシンでのみ使うべきです。
  */
  { "pop_reconnect",	DT_QUAD, R_NONE, {.l=OPT_POPRECONNECT}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** POP サーバとの接続が切れたときに、Mutt が再接続を試みるかどうかを制御します。
  */
  { "pop_user",		DT_STR,	 R_NONE, {.p=&PopUser}, {.p=0} },
  /*
  ** .pp
  ** POP サーバへのログイン名です。
  ** .pp
  ** 既定ではローカルマシンでのユーザ名になります。
  */
#endif /* USE_POP */
  { "post_indent_string",DT_STR, R_NONE, {.p=&PostIndentString}, {.p=0} },
  /*
  ** .pp
  ** $$attribution 変数と似ていて、Mutt はこの文字列を、返信するメッセージの引用の
  ** あとに追加します。
  ** 定義されている \fCprintf(3)\fP 風の書式の完全な一覧は $$index_format
  ** 説を参照してください。
  */
  { "post_indent_str",  DT_SYN,  R_NONE, {.p="post_indent_string"}, {.p=0} },
  /*
  */
  { "postpone",		DT_QUAD, R_NONE, {.l=OPT_POSTPONE}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** メッセージをすぐに送らないことを選択した場合、メッセージを $$postponed
  ** にセーブするかどうかを制御します。
  ** .pp
  **  $$recall 変数も参照してください。
  */
  { "postponed",	DT_PATH, R_INDEX, {.p=&Postponed}, {.p="~/postponed"} },
  /*
  ** .pp
  ** Mutt は編集中のメッセージを無制限に ``メッセージの送信を $postpone'' できます。
  ** メッセージを保留にすることを選んだ場合、Mutt はこの変数で指定されたメールボックスに
  ** 保存します。
  ** .pp
  ** $$postpone 変数も参照してください。
  */
  { "postpone_encrypt",    DT_BOOL, R_NONE, {.l=OPTPOSTPONEENCRYPT}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、暗号化するようにマークされた延期されたメッセージは、
  ** 自己暗号化されます。Mutt は最初に $$pgp_default_key 又は $$smime_default_key
  ** で指定された値を使って暗号化しようとします。それらが設定されていない場合、
  ** 非推奨の $$postpone_encrypt_as を使おうとします。
  ** (Crypto のみです)
  */
  { "postpone_encrypt_as", DT_STR,  R_NONE, {.p=&PostponeEncryptAs}, {.p=0} },
  /*
  ** .pp

  ** これは $$postpone_encrypt に対する非推奨のフォールバック変数です。
  ** $$pgp_default_key 又は $$smime_default_key を使ってください。
  ** (Crypto のみです)
  */
#ifdef USE_SOCKET
  { "preconnect",	DT_STR, R_NONE, {.p=&Preconnect}, {.p=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt がサーバへの接続を確立するのに失敗した場合に
  ** 実行されるシェルコマンドです。これは、たとえば、\fCssh(1)\fP のような
  ** セキュアな接続を設定するのに便利です。コマンドの結果が非ゼロのステータス
  ** を返す場合は、Mutt はサーバをオープンすることを諦めます。例は以下の通りです。
  ** .ts
  ** set preconnect="ssh -f -q -L 1234:mailhost.net:143 mailhost.net \(rs
  ** sleep 20 < /dev/null > /dev/null"
  ** .te
  ** .pp
  ** ``mailhost.net'' 上のメールボックス ``foo'' はこれで ``{localhost:1234}foo''
  ** として扱うことが出来ます。
  ** .pp
  ** 注意: この例を動かすためには、パスワード入力なしで、リモートマシンに
  ** ログインできる必要があります。
  */
#endif /* USE_SOCKET */
  { "print",		DT_QUAD, R_NONE, {.l=OPT_PRINT}, {.l=MUTT_ASKNO} },
  /*
  ** .pp
  ** Mutt が本当にメッセージを印刷するかどうかを制御します。
  ** 何人かが間違ってしばしば ``p'' を叩くために、これは既定値で
  **  ``ask-no'' を設定します。
  */
  { "print_command",	DT_PATH, R_NONE, {.p=&PrintCmd}, {.p="lpr"} },
  /*
  ** .pp
  ** これに指定したコマンドへパイプしてメッセージを印刷します。
  */
  { "print_cmd",	DT_SYN,  R_NONE, {.p="print_command"}, {.p=0} },
  /*
  */
  { "print_decode",	DT_BOOL, R_NONE, {.l=OPTPRINTDECODE}, {.l=1} },
  /*
  ** .pp
  ** \fC<print-message>\fP コマンド関連で使います。このオプションが
  ** \fIset\fP の場合、メッセージは $$print_command で指定された外部コマンドに
  ** 渡される前にデコードされます。このオプションが \fIunset\fP の場合、
  ** 印刷するときに、メッセージには何も処理を適用しません。後者は、
  ** メールメッセイジを印刷時に適切に整形できる、高度なプリンタフィルタを
  ** 使う場合に便利です。
  */
  { "print_split",	DT_BOOL, R_NONE, {.l=OPTPRINTSPLIT},  {.l=0} },
  /*
  ** .pp
  ** \fC<print-message>\fP コマンド関連で使います。このオプションが
  ** \fIset\fP の場合、印刷される各メッセージに対して、$$print_command で指定された
  ** コマンドが1回実行されます。このオプションが\fIunset\fP の場合、
  ** $$print_command で指定されたコマンドが1回だけ実行され、すべてのメッセージが
  ** メッセージの区切りとしてフォームフィードを付けて結合されます。
  ** .pp
  ** \fCenscript\fP(1) プログラムのメール印刷モードを使う場合、このオプションを
  ** \fIset\fP に設定したいと思うようになるでしょう。
  */
  { "prompt_after",	DT_BOOL, R_NONE, {.l=OPTPROMPTAFTER}, {.l=1} },
  /*
  ** .pp
  ** \fIexternal\fP $$pager を使っている場合、この変数を設定すると、
  ** ページャ終了時に、インデックスメニューに戻るのではなく、Mutt がコマンド入力を
  ** 即すようになります。\fIunset\fP の場合、Mutt は外部ページャ終了時に、
  ** インデックスメニューに戻ります。
  */
  { "query_command",	DT_PATH, R_NONE, {.p=&QueryCmd}, {.p=0} },
  /*
  ** .pp
  ** これは、Mutt  が外部のアドレス問合せをするのに使うコマンドを指定します。
  ** 文字列は、ユーザが入力した問合せ文字列に置き換えられる ``%s'' を含むことも
  ** できます。Mutt はシェルの引用ルールに沿って、Mutt は自動的に、``%s'' に
  ** 置き換えられた文字列を引用符で囲みます。文字列中に``%s'' がない場合は、
  ** Mutt は文字列の最後にユーザの問合せを追加します。詳細な情報については
  ** ``$query'' を参照してください。
  */
  { "query_format",	DT_STR, R_NONE, {.p=&QueryFormat}, {.p="%4c %t %-25.25a %-25.25n %?e?(%e)?"} },
  /*
  ** .pp
  ** この変数は``query'' メニューのフォーマットを記述します。以下は
  ** 使用できる\fCprintf(3)\fP 風の書式です:
  ** .dl
  ** .dt %a  .dd 宛先アドレス
  ** .dt %c  .dd 現在のエントリ番号
  ** .dt %e  .dd 拡張情報 *
  ** .dt %n  .dd 宛先名
  ** .dt %t  .dd 現在のエントリがタグづけられている場合は ``*'' で空白はそれ以外
  ** .dt %>X .dd 残りの文字列を右揃えし ``X'' で埋める
  ** .dt %|X .dd 行の最後まで ``X'' で埋める
  ** .dt %*X .dd 埋め草として 文字 ``X'' を使って soft-fill
  ** .de
  ** .pp
  ** ``soft-fill'' についての説明は、$$index_format の説明を参照してください。
  ** .pp
  ** * = は、非ゼロの場合にオプションで表示されます。$$status_format の説明を参照してください。
  */
  { "quit",		DT_QUAD, R_NONE, {.l=OPT_QUIT}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** この変数は ``quit'' と ``exit'' が実際に Mutt を終了させるかどうかを制御します。
  ** 終了させようとしたとき、このオプションが\fIset\fP ならば終了し、\fIunset\fP
  ** ならば無効となり、\fIask-yes\fP か \fIask-no\fP ならば確認のために問合せを
  ** してきます。
  */
  { "quote_regexp",	DT_RX,	 R_PAGER, {.p=&QuoteRegexp}, {.p="^([ \t]*[|>:}#])+"} },
  /*
  ** .pp
  ** 内部ページャで、メッセージ本文の引用部分を見極めるために使う正規表現です。
  ** 引用されたテキストは、\fC<toggle-quoted>\fP コマンドを使って除外するか、
  ** ``color quoted''ファミリのディレクティブに従って色を付けることが出来ます。 
  ** .pp
  ** 高位のレベルの引用は異なって着色されることがあります
  ** (``color quoted1'',``color quoted2'' など)。引用レベルは、一致したテキストから
  ** 最後の文字を取り去り、一致しなくなるまで再帰的に正規表現を適用する
  ** ることで決まります。
  ** .pp
  ** 一致の検出は $$smileys 正規表現によって上書きできます。
  */
  { "read_inc",		DT_NUM,	 R_NONE, {.p=&ReadInc}, {.l=10} },
  /*
  ** .pp
  ** 0 より大きな値に設定されていると、メールボックス読み出し中か、検索と制限の
  ** ような検索動作を実行するときに、 Mutt が現在どのメッセージまで来たのかを
  ** 表示するようになります。メッセージは、このたくさんのメッセージが読み出されたり
  ** 検索された後で表示されます(たとえば 25 に設定すると、Mutt は 25 番目のメッセージを
  ** 読んだときに表示し、さらにまた 50 番目のときにも表示します)。
  ** この変数は、少々時間のかかる、大きなメールボックスを読み出したり検索したりするときに、
  ** 進行条項を表示するということを意味します。0 に設定すると、メールボックスを
  ** 読み出す前に、1度メッセージが表示されるだけになります。
  ** .pp
  ** $write_inc, $$net_inc and $$time_inc 変数と、パフォーマンスの考察についての
  ** ``$tuning'' セクションのマニュアルも参照してください。
  */
  { "read_only",	DT_BOOL, R_NONE, {.l=OPTREADONLY}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、すべてのフォルダはリードオンリモードでオープンされます。
  */
  { "realname",		DT_STR,	 R_BOTH, {.p=&Realname}, {.p=0} },
  /*
  ** .pp
  ** この変数は、メッセージを送信するときに使う ``real'' 又は ``personal'' 名を
  ** 指定します。
  ** .pp
  ** 既定では、これは、 \fC/etc/passwd\fP の GECOS フィールドです。この変数は
  ** $$from variable 中で本名を指定した場合には\fI使われない\fP ことに注意してくださいい。
  */
  { "recall",		DT_QUAD, R_NONE, {.l=OPT_RECALL}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** 新規メニューを編集するときに延期メッセージから続けるかどうかを制御します。
  ** .pp
  ** この変数を \fIyes\fP に設定することは一般的には有用ではないので推奨されて
  ** いません。\fC<recall-message>\fP 機能は、保留されたメッセージを手動で再度
  ** 読み出すときに使われることに注意してください。
  ** .pp
  ** $postponed 変数も参照してください。
  */
  { "record",		DT_PATH, R_NONE, {.p=&Outbox}, {.p="~/sent"} },
  /*
  ** .pp
  ** これは、送信メッセージをどのファイルに追加して保存すべきかを指定します
  ** (これは、メッセージのコピーをセーブする基本的な方法を意味しますが、
  ** ほかにも、自分のメールアドレスの ``Bcc:'' フィールドを作成して、
  ** ``$my_hdr'' コマンドを使ってそこにセーブするという方法もあります)。
  ** .pp
  ** コマンドで上書きできます。$$copy と $$write_bcc も参照してください。
  */
  { "reflow_space_quotes",	DT_BOOL, R_NONE, {.l=OPTREFLOWSPACEQUOTES}, {.l=1} },
  /*
  ** .pp
  ** このオプションはページャ中と返信時に($$text_flowed が\fIunset\fPの時)、
  ** format=flowed メッセージからの引用がどのように表示されるかを制御します。
  ** 設定する場合、このオプションは各レベルの引用マークの後に空白を追加し、
  ** ">>>foo" を "> > > foo" に調整します。
  ** .pp
  ** \fB注意:\fP $$reflow_text が \fIunset\fP の時、このオプションは無効です。
  ** 同様に、このオプションは$$text_flowed が \fIset\fP の時には返信に何ら影響を
  ** 与えません。
  */
  { "reflow_text",	DT_BOOL, R_NONE, {.l=OPTREFLOWTEXT}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt は format=flowed にマークされた text/plain パート中の
  ** 段落を再フォーマットします。\fIunset\fP の場合、Mutt は、メッセージ本体中で
  ** どのように表示されるかを変更していない段落を表示します。\fIformat=flowed\fP
  ** フォーマットの詳細については RFC3676 を参照してください。
  ** .pp
  ** $reflow_wrap と $$wrap も参照してください。
  */
  { "reflow_wrap",	DT_NUM,	R_NONE, {.p=&ReflowWrap}, {.l=78} },
  /*
  ** .pp
  ** この変数は $$reflow_text が \fIset\fP の時に text/plain パートを再フォーマットする
  ** 時の最大段落幅を制御します。値が 0 の場合、段落は端末の右マージンで折り返されます。
  ** 正の値は右マージンからの相対段落幅を設定します。
  ** .pp
  ** Also see $$wrap.
  */
  { "reply_regexp",	DT_RX,	 R_INDEX|R_RESORT, {.p=&ReplyRegexp}, {.p="^(re([\\[0-9\\]+])*|aw):[ \t]*"} },
  /*
  ** .pp
  ** スレッド化および返信する時に返信メッセージを認識するために使われる正規表現
  ** です。既定値は英語の "Re:" とドイツ語の "Aw:" に対応しています。
  */
  { "reply_self",	DT_BOOL, R_NONE, {.l=OPTREPLYSELF}, {.l=0} },
  /*
  ** .pp
  ** \fIunset\fP で、自分自身から送信したメッセージに返信する場合、Mutt は
  ** 自分自身ではなく、そのメッセージの受信者に返信したいと仮定します。
  ** .pp
  ** ``$alternates'' コマンドも参照してください。
  */
  { "reply_to",		DT_QUAD, R_NONE, {.l=OPT_REPLYTO}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** \fIset\fPの場合、メッセージに返信する時、Mutt はメッセージの Reply-to: ヘッダで
  ** 挙げられているアドレスを返信先として使います。\fIunset\fP の場合は、
  ** 代わりに From: フィールドのアドレスを代わりに使います。このオプションは、
  ** Reply-To: ヘッダフィールドをメーリングリストのアドレスに設定していて、
  ** メッセージの作者に個人的なメッセージを送信しようとする場合に便利です。
  */
  { "resolve",		DT_BOOL, R_NONE, {.l=OPTRESOLVE}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、現在のメッセージを変更するコマンドが実行されたときは
  ** 必ず次(おそらく未読)のメッセージに自動的にカーソルを移動します。
  */
  { "resume_draft_files", DT_BOOL, R_NONE, {.l=OPTRESUMEDRAFTFILES}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、ドラフトファイル(コマンド行で\fC-H\fP で指定されたもの)
  ** は、保留メッセージを復活させた時と同じように処理されます。受信者は
  ** 要求されません。send-hooks は評価されません。別名の展開は行われません。
  ** ユーザ定義のヘッダと署名はメッセージに追加されません。
  */
  { "resume_edited_draft_files", DT_BOOL, R_NONE, {.l=OPTRESUMEEDITEDDRAFTFILES}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、以前に編集したドラフトファイル(コマンド行での \fC-E -H\fP
  ** 経由で)は、再度ドラフトファイルとして使われるときに、自動的に
  ** $$resume_draft_files が設定されます。
  ** .pp
  ** 最初にドラフトファイルがセーブされる時、Mutt はヘッダ X-Mutt-Resume-Draft を
  ** セーブされたファイルに追加します。次回ドラフトファイルを読み込んだときに
  ** Mutt がヘッダを見つけると、$$resume_draft_files が設定されます。
  ** .pp
  ** このオプションは、複数の署名、ユーザ定義のヘッダと他の処理効果がドラフトファイルに
  ** 対して複数回実行されることを防ぐために設計されました。
  ** 
  */
  { "reverse_alias",	DT_BOOL, R_BOTH, {.l=OPTREVALIAS}, {.l=0} },
  /*
  ** .pp
  ** この変数は、メッセージの送信者に一致する別名があるときに、別名の ``personal''名を
  ** インデックスメニュー中に表示するかどうかを制御します。例えば、以下のような
  ** 別名があるとします。
  ** .ts
  ** alias juser abd30425@somewhere.net (Joe User)
  ** .te
  ** .pp
  ** この場合、以下のヘッダを含むメールを受け取ったとします。
  ** .ts
  ** From: abd30425@somewhere.net
  ** .te
  ** .pp
  ** この場合、インデックスメニューには ``abd30425@somewhere.net.'' の代わりに
  ** ``Joe User'' が表示されます。これは、個人のメールアドレスが人間向きではない
  ** 場合に便利です。
  */
  { "reverse_name",	DT_BOOL, R_BOTH, {.l=OPTREVNAME}, {.l=0} },
  /*
  ** .pp
  ** 特定のマシンでメールを受信し、そのメッセージを別のマシンに移動し、そこから
  ** いくつかのメッセージを返信するということがたまにあるかもしれません。この変数を
  ** \fIset\fP にした場合、返信メッセージの既定の \fIFrom:\fP 行は、
  ** ``$alternates'' に一致するアドレスの場合、返信しようとする受信したメッセージの
  ** アドレスを使って組み立てます。変数が\fIunset\fP の場合か、``$alternates''に
  ** 一致していなかった場合は、\fIFrom:\fP 行は現在のマシン上でのアドレスが使われます。
  ** .pp
  ** ``$alternates'' コマンドと $$reverse_realname も参照してください。
  */
  { "reverse_realname",	DT_BOOL, R_BOTH, {.l=OPTREVREAL}, {.l=1} },
  /*
  ** .pp
  ** この変数は $$reverse_name 機能の動作を微調整します。
  ** .pp
  ** \fIunset\fP の場合、Mutt は一致したアドレスの実名部分を取り除きます。
  ** これは、送信者が実名フィールドに記述したものを使わず、メールアドレスを
  ** 使います。
  ** .pp
  ** \fIset\fP の場合、Mutt  は一致したアドレスをそのまま使います。
  ** .pp
  ** それ以外の場合、実名がない場合は $$realname の値を後で使って埋めます。
  */
  { "rfc2047_parameters", DT_BOOL, R_NONE, {.l=OPTRFC2047PARAMS}, {.l=0} },
  /*
  ** .pp
  ** この変数が \fIset\fP の場合、Mutt は RFC2047 でエンコードされた MIME パラメータを
  ** デコードします。例えば添付をファイルにセーブする場合に、Mutt が以下のような
  ** 値を提案してくる場合は、設定したいと思うでしょう。
  ** .ts
  ** =?iso-8859-1?Q?file=5F=E4=5F991116=2Ezip?=
  ** .te
  ** .pp
  ** この変数を対話的に fIset\fP にしても、フォルダを変更するまでは変更が有効に
  ** なりません。
  ** .pp
  ** この、RFC2047 エンコードの使用は規格によって明示的に禁止されているのですが、
  ** 実際には使われていることに注意してください。
  ** .pp
  ** また、このパラメートを設定することは、Mutt がこの種のエンコーディングを
  ** \fI生成する\fP 効果が\fない\fことにも注意してください。かわりに、Mutt は
  ** RFC2231 で指定されたエンコーディングを無条件に使います。P
  */
  { "save_address",	DT_BOOL, R_NONE, {.l=OPTSAVEADDRESS}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt はメールをセーブする既定のフォルダを選択するときに、
  ** 送信者の完全なアドレスを使います。$$save_name か $$force_name も \fIset\fP の
  ** 場合、Fcc フォルダの選択も同様に変更になります。
  */
  { "save_empty",	DT_BOOL, R_NONE, {.l=OPTSAVEEMPTY}, {.l=1} },
  /*
  ** .pp
  ** \fIunset\fP の場合、何らメールがセーブされていないメールボックスはクローズする
  ** 時に削除されます(例外は $$spoolfile で決して削除されません)。If \fIset\fPの
  ** 場合、メールボックスは決して削除されません。
  ** .pp
  ** \fB注意:\fP これは mbox と MMDF フォルダにのみ適用され、Mutt は MH と Maildir
  ** ディレクトリは削除しません。
  */
  { "save_history",     DT_NUM,  R_NONE, {.p=&SaveHist}, {.l=0} },
  /*
  ** .pp
  ** この変数は、$$history_file ファイルにセーブされる、(カテゴリ単位の)履歴の
  ** サイズを制御します。
  */
  { "save_name",	DT_BOOL, R_NONE, {.l=OPTSAVENAME}, {.l=0} },
  /*
  ** .pp
  ** この変数は、どのように送信メールのコピーがセーブされるかを制御します。
  ** \fIset\fP の場合、受信者アドレスの存在によって指定されるメールボックスが
  ** あるかどうかをチェックします(これは、受信者アドレスの \fIusername\fP 部分で
  ** $$folder ディレクトリ中にメールボックスがあるかを検索することによって
  ** 行います)。メールボックスが存在する場合、送信メッセージはそのメールボックスに
  ** セーブされます。その他の場合は、メッセージは $$record メールボックスに
  ** セーブされます。
  ** .pp
  ** $$force_name 変数も参照してください。
  */
  { "send_group_reply_to",	DT_BOOL, R_NONE, {.l=OPTSENDGROUPREPLYTO}, {.l=0} },
  /*
  ** .pp
  ** この変数はどのグループ返信が終わったかを制御します。設定された場合、
  ** "To:" 中のすべての受信者リストは "To:" ヘッダに再度設定され、それ以外は、
  ** 既定である "CC" になります。 
  */
  { "score", 		DT_BOOL, R_NONE, {.l=OPTSCORE}, {.l=1} },
  /*
  ** .pp
  ** この変数が \fIunset\fP の場合、スコアリングは停止します。これは、
  ** $$score_threshold_delete 変数と関連したものが使われている場合、特定のフォルダに
  ** 対して選択的に無効にできるので便利です。
  **
  */
  { "score_threshold_delete", DT_NUM, R_NONE, {.p=&ScoreThresholdDelete}, {.l=-1} },
  /*
  ** .pp
  ** この変数の値以下のスコアが割り当てられているメッセージは、Mutt によって
  ** 自動的に削除マークが付けられます。Mutt のスコアは常時 0 以上なので、
  ** この変数の既定の設定では、決して削除マークが付くことはありません。
  */
  { "score_threshold_flag", DT_NUM, R_NONE, {.p=&ScoreThresholdFlag}, {.l=9999} },
  /*
  ** .pp
  ** この変数の値以上のスコアが割り当てられているメッセージは自動的に "flagged" が
  ** マークされます。
  */
  { "score_threshold_read", DT_NUM, R_NONE, {.p=&ScoreThresholdRead}, {.l=-1} },
  /*
  ** .pp
  ** この変数の値以下のスコアが割り当てられたメッセージは自動的にMutt によって
  ** 既読マークが付けられます。Mutt のスコアは常時 0 以上なので、
  ** この変数の既定の設定では、決して既読マークが付くことはありません。
  */
  { "search_context",	DT_NUM,  R_NONE, {.p=&SearchContext}, {.l=0} },
  /*
  ** .pp
  ** ページャにおいて、この変数は検索結果の前に表示する行数を指定します。
  ** 既定では、検索結果は上寄せになります。
  */
  { "send_charset",	DT_STR,  R_NONE, {.p=&SendCharset}, {.p="us-ascii:iso-8859-1:utf-8"} },
  /*
  ** .pp
  ** 送信メッセージに対する、コロンで分離された文字セットのリストです。Mutt は
  ** テキストを正確に変換できた最初の文字セットを使います。$$charset が
  ** ``iso-8859-1'' でなく、受信者が ``UTF-8'' を理解出来ない場合は、``iso-8859-1''の
  ** かわり、あるいはその後に、適切で広く使われている標準的な文字セット
  ** (たとえば``iso-8859-2'', ``koi8-r'' 又は ``iso-2022-jp'')をリスト中に含めることを
  ** 推奨します。
  ** .pp
  ** それらのどれかに正確に変換できない場合、Mutt は $$charset を使うように
  ** フォールバックします。
  ** 
  */
  { "send_multipart_alternative", DT_QUAD, R_NONE, {.l=OPT_SENDMULTIPARTALT}, {.l=MUTT_NO} },
  /*
  ** .pp
  ** \fIset\fP の場合、Mutt はmultipart/alternative コンテナと
  ** $$send_multipart_alternative_filter で指定されたフィルタスクリプトを使う
  ** alternative パートを生成します。
  ** ``MIME Multipart/Alternative'' ($alternative-order) を参照してください。
  ** .pp
  ** multipart/alternative を有効にすることは、インラインの PGP 暗号化とは互換がない
  ** ことに注意してください。Mutt はこの場合、PGP/MIME を使うように問合せしてきます。
  */
  { "send_multipart_alternative_filter", DT_PATH, R_NONE, {.p=&SendMultipartAltFilter}, {.p=0} },
  /*
  ** .pp
  ** これは、メールの(編集された)メインメッセージを lternative フォーマットに
  ** 変換するスクリプトを指定します。メッセージはフィルタの標準入力にパイプされます。
  ** フィルタで展開された出力は生成された mime タイプ、たとえば text/html で、
  ** 空白行とその後に変換された内容が続きます。
  ** ``MIME Multipart/Alternative'' ($alternative-order) を参照してください。
  */
  { "sendmail",		DT_PATH, R_NONE, {.p=&Sendmail}, {.p=SENDMAIL " -oem -oi"} },
  /*
  ** .pp
  ** Mutt によってメール配信を行うのに使うプログラムと引数を指定します。
  ** Mutt は指定されたプログラムが、追加の引数を受信者のアドレスとして解釈することを
  ** 期待しています。Mutt はデリミタ \fC--\fP (前に存在していなければ)の
  ** 後にすべての受信者を追加します。たとえば、$$use_8bitmime, $$use_envelope_from,
  ** $dsn_notify, 又は $$dsn_return のような追加のフラグはデリミタの前に付加されます。
  ** .pp
  ** $$write_bcc も \fB参照してください\fP。
  */
  { "sendmail_wait",	DT_NUM,  R_NONE, {.p=&SendmailWait}, {.l=0} },
  /*
  ** .pp
  ** $$sendmail プロセスが完了するのを諦め、バックグラウンドで配送する
  ** 前までの秒数を指定します。
  ** .pp
  ** Mutt はこの変数の値を以下のように解釈します。
  ** .dl
  ** .dt >0 .dd 継続する前に、sendmail が完了するのを待つ秒数
  ** .dt 0  .dd sendmail の完了を無限に待つ
  ** .dt <0 .dd 待たないで、sendmail をバックグラウンドで常時実行する
  ** .de
  ** .pp
  ** 0 より大きな値を指定した場合、子プロセスの出力はテンポラリファイルに
  ** 出されることに注意してください。もしもなんらかのエラーがあった場合は、
  ** その出力がどこにあるかを通知されることになります。
  */
  { "shell",		DT_PATH, R_NONE, {.p=&Shell}, {.p=0} },
  /*
  ** .pp
  ** サブシェルを起動するときに使うコマンド。既定では、\fC/etc/passwd\fP で
  ** 使われているログインシェルとなります。
  */
#ifdef USE_SIDEBAR
  { "sidebar_delim_chars", DT_STR, R_SIDEBAR, {.p=&SidebarDelimChars}, {.p="/."} },
  /*
  ** .pp
  ** これにはサイドバー中でパスを表示するためのフォルダセパレータとして
  ** 扱う文字の一覧が含まれています。
  ** .pp
  ** ローカルメールはしばしば `dir1/dir2/mailbox' というディレクトリに配置されます。
  ** .ts
  ** set sidebar_delim_chars='/'
  ** .te
  ** .pp
  ** IMAP メールボックスはしばしば `folder1.folder2.mailbox' という名前になります。
  ** .ts
  ** set sidebar_delim_chars='.'
  ** .te
  ** .pp
  ** $$sidebar_short_path, $$sidebar_folder_indent, $$sidebar_indent_string も
  ** \fB参照してください\fP。
  */
  { "sidebar_divider_char", DT_STR, R_SIDEBAR, {.p=&SidebarDividerChar}, {.p="|"} },
  /*
  ** .pp
  ** これは (表示される場合)サイドバーと他の Mutt のパネルとの間に表示される
  ** 文字を指定します。ASCII と Unicode の行描画文字がサポートされています。
  */
  { "sidebar_folder_indent", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARFOLDERINDENT}, {.l=0} },
  /*
  ** .pp
  ** サイドバー中でメールボックスを段付けする場合はこれを設定します。
  ** .pp
  ** $$sidebar_short_path, $$sidebar_folder_indent, $$sidebar_indent_string も
  ** \fB参照してください\fP。
  */
  { "sidebar_format", DT_STR, R_SIDEBAR, {.p=&SidebarFormat}, {.p="%B%*  %n"} },
  /*
  ** .pp
  ** この変数でサイドバーの表示をカスタマイズすることが出来るようになります。
  ** この文字列は $$index_format と似ていますが、\fCprintf(3)\fP 風の固有の書式を
  ** 持っています。
  ** .dl
  ** .dt %B  .dd メールボックスの名前
  ** .dt %S  .dd * メールボックスのサイズ(メッセージ総数)
  ** .dt %N  .dd * メールボックス中の未読メッセージ数
  ** .dt %n  .dd N 新規メールがある場合、その他の場合は空白
  ** .dt %F  .dd * メールボックス中のフラグ付きメッセージ数
  ** .dt %!  .dd ``!'' : 1つフラグがあるメッセージ;
  **             ``!!'' : 2つフラグがあるメッセージ;
  **             ``n!'' : n 2つ以上フラグがあるメッセージ (n > 2)。
  **             その他は表示なし。
  ** .dt %d  .dd * @ 削除メッセージ数
  ** .dt %L  .dd * @ 制限後のメッセージ数
  ** .dt %t  .dd * @ タグ付きメッセージ数
  ** .dt %>X .dd 残りの文字列を右寄せし、``X'' で埋める
  ** .dt %|X .dd 行端まで ``X'' で埋める
  ** .dt %*X .dd 埋め草として 文字 ``X'' を使って soft-fill
  ** .de
  ** .pp
  ** * = 非ゼロの場合オプションで表示
  ** @ = 現在のフォルダのみに適用されます
  ** .pp
  ** %S, %N, %F, と %! を使うために、$$mail_check_stats は
  ** \fIset\fP としなければなりません。これを設定した場合、このオプションに対する
  ** 推奨値は "%B%?F? [%F]?%* %?N?%N/?%S" となります。
  */
  { "sidebar_indent_string", DT_STR, R_SIDEBAR, {.p=&SidebarIndentString}, {.p="  "} },
  /*
  ** .pp
  ** サイドバー中でメールボックスを段付けするために使われる文字列を指定します。
  ** 既定値は2つの空白です。
  ** .pp
  ** $$sidebar_short_path, $$sidebar_folder_indent, $$sidebar_indent_string も
  ** \fB参照してください\fP。
  */
  { "sidebar_new_mail_only", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARNEWMAILONLY}, {.l=0} },
  /*
  ** .pp
  ** 設定した場合、サイドバーはメールボックスに新規メール、フラグ付きのメールがある場合
  ** にのみ表示されます。
  ** .pp
  ** $sidebar_whitelist も \fB参照してください\fP。
  */
  { "sidebar_next_new_wrap", DT_BOOL, R_NONE, {.l=OPTSIDEBARNEXTNEWWRAP}, {.l=0} },
  /*
  ** .pp
  ** 設定した場合、 \fC<sidebar-next-new>\fP コマンドは停止せず、メールボックス一覧の
  ** 最後まで行きますが、最初にまで戻ります。\fC<sidebar-prev-new>\fP コマンドは
  ** 同様に影響を受けますが、リストの最後に戻ります。
  */
  { "sidebar_relative_shortpath_indent", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARRELSPINDENT}, {.l=0} },
  /*
  ** .pp
  ** 設定した場合、このオプションは $$sidebar_short_path と 
  ** $$sidebar_folder_indent がどのように短縮および断付けを実行するかを変更します。
  ** 両者は以前のサイドバーエントリを見て、もっとも最新の親からの相対で、
  ** 短縮/段づけをします。
  ** .pp
  ** 以下は $$sidebar_short_path=yes,$$sidebar_folder_indent=yes と
  ** $$sidebar_indent_string="→" を使って、この順で並んでいるメールボックスの、
  ** オプションの設定/解除の例です。
  ** .dl
  ** .dt \fBmailbox\fP  .dd \fBset\fP   .dd \fBunset\fP
  ** .dt \fC=a.b\fP     .dd \fC=a.b\fP  .dd \fC→b\fP
  ** .dt \fC=a.b.c.d\fP .dd \fC→c.d\fP  .dd \fC→→→d\fP
  ** .dt \fC=a.b.e\fP   .dd \fC→e\fP    .dd \fC→→e\fP
  ** .de
  ** .pp
  ** 2行目はもっともわかりやすい例です。このオプションを設定すると、
  ** \fC=a.b.c.d\fP は \fC=a.b\fP から相対的に短くなり、\fCc.d\fP となります。
  ** また、\fC=a.b\fP から相対的に1段段付けされています。このオプションを設定しないと、
  ** \fC=a.b.c.d\fP は常時メールボックスの最後の部分 \fCd\fP に短縮され、
  ** $$folder ('=' で表される)を鑑みて3段段付けされます。
  ** .pp
  ** 設定すると、3行目も最初の行からの相対で段付けされ短縮されます。
  */
  { "sidebar_short_path", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARSHORTPATH}, {.l=0} },
  /*
  ** .pp
  ** 既定で、サイドバーには、$$folder 変数からの相対で、メールボックスのパスを表示
  ** します。 \fCsidebar_shortpath=yes\fP を設定すると、以前の名前から比べて
  ** 名前が短縮されます。以下が例です。
  ** .dl
  ** .dt \fBshortpath=no\fP .dd \fBshortpath=yes\fP .dd \fBshortpath=yes, folderindent=yes, indentstr=".."\fP
  ** .dt \fCfruit\fP        .dd \fCfruit\fP         .dd \fCfruit\fP
  ** .dt \fCfruit.apple\fP  .dd \fCapple\fP         .dd \fC..apple\fP
  ** .dt \fCfruit.banana\fP .dd \fCbanana\fP        .dd \fC..banana\fP
  ** .dt \fCfruit.cherry\fP .dd \fCcherry\fP        .dd \fC..cherry\fP
  ** .de
  ** .pp
  ** $$sidebar_delim_chars, $$sidebar_folder_indent, $$sidebar_indent_string も
  ** \fB参照してください\fP。
  */
  { "sidebar_sort_method", DT_SORT|DT_SORT_SIDEBAR, R_SIDEBAR, {.p=&SidebarSortMethod}, {.l=SORT_ORDER} },
  /*
  ** .pp
  ** どのようにファイルブラウザ中でエントリを整列するかを指定します。既定では
  ** エントリは英語順に整列されます。正しい値は以下のようになります。
  ** .il
  ** .dd alpha (英語順)
  ** .dd count (全部のメッセージ数順)
  ** .dd flagged (フラグされたメッセージ数順)
  ** .dd name (英語順)
  ** .dd new (未読メッセージ数順)
  ** .dd path (英語順)
  ** .dd unread (未読メッセージ数順)
  ** .dd unsorted
  ** .ie
  ** .pp
  ** オプションで、逆順での整列を指定するために、``reverse-'' 接頭辞を使う事も
  ** できます(例: ``\fCset sort_browser=reverse-date\fP'')。
  */
  { "sidebar_use_mailbox_shortcuts", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARUSEMBSHORTCUTS}, {.l=0} },
  /*
  ** .pp
  ** 設定した場合、サイドバーメールボックスは、メールボックスショートカット接頭辞
  ** "=" 又は "~" をつけて表示されます。
  ** .pp
  ** 設定しない場合、サイドバーは $$folder 接頭辞に一致したものを削除しますが、
  ** それ以外は、メールボックスショートカットを使いません。
  */
  { "sidebar_visible", DT_BOOL, R_REFLOW, {.l=OPTSIDEBAR}, {.l=0} },
  /*
  ** .pp
  ** これは、サイドバーを表示するか否かを指定します。サイドバーはすべてのメールボックスを
  ** 表示します。
  ** .pp
  ** $$sidebar_format, $$sidebar_width も\fB参照してください\fP。
  */
  { "sidebar_width", DT_NUM, R_REFLOW, {.p=&SidebarWidth}, {.l=30} },
  /*
  ** .pp
  ** これはサイドバーの幅を制御します。画面の列で計測されます。たとえば、
  ** sidebar_width=20 は 20 文字のASCII 文字か、10文字の中国文字を表示できます。
  */
#endif
  { "sig_dashes",	DT_BOOL, R_NONE, {.l=OPTSIGDASHES}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、``-- '' を含む行(その後に空白があることに注意)が
  ** $$signature の前に挿入されます。署名が名前のみを含んでいる場合を除き、
  ** この変数を\fIunset\fP しないことを\fB強く\fP 推奨します。その理由は、
  ** 署名を検出するために、多くのソフトウェアパッケージが ``-- \n'' を使うという
  ** ことだからです。たとえば、Mutt は内蔵ページャで異なった色で署名をハイライト
  ** する機能があります。
  */
  { "sig_on_top",	DT_BOOL, R_NONE, {.l=OPTSIGONTOP}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、署名は引用や転送テキストの前に付けられます。これが何をするかを
  ** 完全に分かっている場合と、ネチケットの番人からの怒りを買う準備が出来ている場合を除き、
  ** この変数を設定しないことを\fB強く\fP 推奨します。
  */
  { "signature",	DT_PATH, R_NONE, {.p=&Signature}, {.p="~/.signature"} },
  /*
  ** .pp
  ** すべての送信メッセージに追加される署名のファイル名を指定します。ファイル名の
  ** 末尾がパイプ (``|'') で終わっている場合、ファイル名はシェルコマンドで、
  ** 入力は標準出力から読み出されるものと仮定されます。
  */
  { "simple_search",	DT_STR,	 R_NONE, {.p=&SimpleSearch}, {.p="~f %s | ~s %s"} },
  /*
  ** .pp
  ** どのように Mutt が単純な検索を実際の検索パターンに展開するかを指定します。
  ** 単純な検索とは ``~'' パターン演算子を1つも含まないもののことです。
  ** 検索パターンについての詳細な情報は ``$patterns'' を参照してください。
  ** .pp
  ** たとえば、単に検索または制限プロンプトに対して ``joe'' を入力したとします。
  ** Mutt は提供された文字列で ``%s'' を、この変数で指定された値に自動的に展開します。
  ** 既定値では ``joe'' は ``~f joe | ~s joe'' に展開されます。
  */
  { "size_show_bytes",	DT_BOOL, R_MENU, {.l=OPTSIZESHOWBYTES}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、メッセージサイズは1 キロバイトより小さい場合、バイトで
  ** 表示されます。$formatstrings-size を参照してください。
  */
  { "size_show_fractions", DT_BOOL, R_MENU, {.l=OPTSIZESHOWFRACTIONS}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、メッセージサイズは 0 から 10キロバイトと 1から 10 メガ
  ** バイトの時は1桁の10進数で表示されます。$formatstrings-size を参照してください。
  */
  { "size_show_mb",	DT_BOOL, R_MENU, {.l=OPTSIZESHOWMB}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、メッセージサイズが1メガバイトより大きい場合、メガバイトで
  ** 表示されます。$formatstrings-size を参照してください。
  */
  { "size_units_on_left", DT_BOOL, R_MENU, {.l=OPTSIZEUNITSONLEFT}, {.l=0} },
  /*
  ** .pp
  ** \fIset\fP の場合、メッセージサイズの単位は数字の左側に表示されます。
  ** $formatstrings-size を参照してください。
  */
  { "sleep_time",	DT_NUM, R_NONE, {.p=&SleepTime}, {.l=1} },
  /*
  ** .pp
  ** フォルダからフォルダへ移動し、現在のフォルダからメッセージを消した後、
  ** 特定の通知メッセージを表示する間停止する時間を秒数で指定します。
  ** 既定では、1秒間だけ止まり、このオプションの値を 0 にすると、この待ち時間を
  ** 無くします。
  */
  { "smart_wrap",	DT_BOOL, R_PAGER_FLOW, {.l=OPTWRAP}, {.l=1} },
  /*
  ** .pp
  ** 内部ページャで、画面幅よりも長い行を表示する方法を制御します。\fIset\fP の場合、
  ** 長い行は単語の区切りで折り返されます。 \fIunset\fP の場合、長い行は
  ** 単純に画面の幅で折り返されます。$$markers 変数も参照してください。
  */
  { "smileys",		DT_RX,	 R_PAGER, {.p=&Smileys}, {.p="(>From )|(:[-^]?[][)(><}{|/DP])"} },
  /*
  ** .pp
  ** \fIページャ\fP は、この変数を使って、$$quote_regexp のよくある一般的な
  ** 誤検知、特にスマイリー(訳注:顔文字)、$$smileys にも一致する場合は、引用された
  ** 行を考慮せず拾います。これは主に行の先頭で発生します。
  ** 
  */
  { "pgp_mime_signature_filename", DT_STR, R_NONE, {.p=&PgpMimeSignatureFilename}, {.p="signature.asc"} },
  /*
  ** .pp
  ** このオプションは、PGP/MIME で署名されたメッセージ中の署名部分に使われるファイル名を
  ** 設定します。
  */
  { "pgp_mime_signature_description", DT_STR, R_NONE, {.p=&PgpMimeSignatureDescription}, {.p="Digital signature"} },
  /*
  ** .pp
  ** このオプションは、PGP/MIME で署名されたメッセージ中の署名部分に使われる
  ** Content-Description を指定します。
  */
  { "smime_ask_cert_label",	DT_BOOL, R_NONE, {.l=OPTASKCERTLABEL}, {.l=1} },
  /*
  ** .pp
  ** このフラグは、データベースに追加しようとしている証明書のラベルを入力するか
  ** どうかについて問合せをするか否かを制御します。既定では \fIset\fP です。
  ** (S/MIME のみです)
  */
  { "smime_ca_location",	DT_PATH, R_NONE, {.p=&SmimeCALocation}, {.p=0} },
  /*
  ** .pp
  ** この変数は、OpenSSL で使う信頼された証明書があるディレクトリかファイルの
  ** 名前が含まれています。
  ** (S/MIME のみです)
  */
  { "smime_certificates",	DT_PATH, R_NONE, {.p=&SmimeCertificates}, {.p=0} },
  /*
  ** .pp
  ** S/MIME には PGP でのような pubring/secring がないので、Mutt は自分自身で
  ** 鍵や証明書の格納および取得を行わなければなりません。この仕組みは現在とても
  ** 基本的なもので、鍵と証明書を二つの異なる ディレクトリに格納し、ともに OpenSSL で
  ** 取得したハッシュ値をファイル名にしています。各ディレクトリには index ファイルがあって、
  ** メールアドレスと鍵 ID の組み合わせが入っていて、手動で編集することができます。
  ** このオプションは証明書の位置を示すものです。
  ** (S/MIME のみです)
  */
  { "smime_decrypt_command", 	DT_STR, R_NONE, {.p=&SmimeDecryptCommand}, {.p=0} },
  /*
  ** .pp
  ** このフォーマット文字列は \fCapplication/x-pkcs7-mime\fP 添付を復号化するのに
  ** 使うコマンドを指定します。
  ** .pp
  ** OpenSSL コマンド形式には、PGP に似た、\fCprintf(3)\fP 風の固有な書式があります。
  ** .dl
  ** .dt %f .dd メッセージが入っているファイルの名前に展開します。
  ** .dt %s .dd \fCmultipart/signed\fP 添付を検証するときに
                署名パートがっているファイルの名前に展開します。
  ** .dt %k .dd $$smime_default_key で指定される鍵ペア
  ** .dt %c .dd 1つ以上の証明書 ID。
  ** .dt %a .dd 暗号化に使うアルゴリズム。
  ** .dt %d .dd $$smime_sign_digest_alg で指定されたメッセージダイジェストアルゴリズム。
  ** .dt %C .dd CA の位置:  $smime_ca_location が ディレクトリかファイルを指定するのに
                依存。これは、``-CApath $$smime_ca_location'' か
  **            ``-CAfile $$smime_ca_location'' に展開されます。
  ** .de
  ** .pp
  ** 上記の形式をどのように設定するかの例は、システムの文書のそばにインストールされて
  ** いる \fCsamples/\fP サブディレクトリ中の \fCsmime.rc\fP を参照してください。
  ** (S/MIME のみです)
  */
  { "smime_decrypt_use_default_key",	DT_BOOL, R_NONE, {.l=OPTSDEFAULTDECRYPTKEY}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合(既定値)、これは、Muttが暗号化時に既定のキーを使うようにさせます。
  ** その他の場合は、複数の、証明書-鍵ペア を管理する場合、Mutt は使用する鍵を決めるために、
  ** メールボックス-アドレスを使おうとします。見つからない場合は、鍵を指定するために
  ** 問合せをしてきます。
  ** (S/MIME のみです)
  */
  { "smime_self_encrypt_as",	DT_SYN,  R_NONE, {.p="smime_default_key"}, {.p=0} },
  { "smime_default_key",		DT_STR,	 R_NONE, {.p=&SmimeDefaultKey}, {.p=0} },
  /*
  ** .pp
  ** これは、S/MIME 操作時に使う既定の鍵ペアで、正しく動くためには keyid (OpenSSL が生成する
  ** ハッシュ値)を設定しなければなりません。
  ** .pp
  ** これは暗号化時に使われます($$postpone_encrypt と $$smime_self_encrypt を
  ** 参照してください)。
  ** .pp
  ** これは、$$smime_decrypt_use_default_key が \fIunset\fP でない限り、
  ** 復号化に使われます。
  ** .pp
  ** また、$$smime_sign_as が設定されていない限り、署名にも使われます。
  ** .pp
  ** (現在は非推奨ですが) \fIsmime_self_encrypt_as\fP がこの変数の別名ですが、
  ** もはや使われていません。
  ** (S/MIME のみです)
  */
  { "smime_encrypt_command", 	DT_STR, R_NONE, {.p=&SmimeEncryptCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、暗号化された S/MIME メッセージを作成するのに使われます。
  ** .pp
  ** これはフォーマット文字列で、使用可能な\fCprintf(3)\fP 風の書式については
  ** $$smime_decrypt_command コマンドを参照してください。
  ** (S/MIME のみです)
  */
  { "smime_encrypt_with",	DT_STR,	 R_NONE, {.p=&SmimeCryptAlg}, {.p="aes256"} },
  /*
  ** .pp
  ** これは、暗号化に使うアルゴリズムを設定します。有効な選択肢は
  ** ``aes128'', ``aes192'', ``aes256'', ``des'', ``des3'', ``rc2-40'', ``rc2-64'',``rc2-128'' です。
  ** (S/MIME のみです)
  */
  { "smime_get_cert_command", 	DT_STR, R_NONE, {.p=&SmimeGetCertCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、PKCS7 構造から X509 証明書を抽出するのに使います。
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。
  ** (S/MIME のみです)
  */
  { "smime_get_cert_email_command", 	DT_STR, R_NONE, {.p=&SmimeGetCertEmailCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、X509 証明書を格納するのに使われるメールアドレスを抽出するのと、
  ** 検証のため(証明書が送信者のメールボックス用に発行されいるか否かをチェックするため)
  ** に使います。
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。
  ** (S/MIME のみです)
  */
  { "smime_get_signer_cert_command", 	DT_STR, R_NONE, {.p=&SmimeGetSignerCertCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、S/MIME 署名から 署名者の X509 証明書のみを抽出するのに使うので、
  ** 証明書の所有者は、メールの ``From:'' フィールドと比較することが出来ます。
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。
  ** (S/MIME のみです)
  */
  { "smime_import_cert_command", 	DT_STR, R_NONE, {.p=&SmimeImportCertCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、smime_keys から証明書をインポートするのに使います。
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。
  ** (S/MIME のみです)
  */
  { "smime_is_default", DT_BOOL,  R_NONE, {.l=OPTSMIMEISDEFAULT}, {.l=0} },
  /*
  ** .pp
  ** Mutt の既定の動作は、すべての自動署名/暗号化操作に PGP を使います。これの代わりに
  ** OpenSSL を使って上書きするには、これを \fIset\fP しなければなりません。
  ** しかし、これは返信時には無効なので、Mutt は自動的に、オリジナルのメッセージの
  ** 署名/暗号化に使われる同じアプリケーションを選択します(この変数は
  ** $$crypt_autosmime を設定しないことにより上書きできます)。
  ** (S/MIME のみです)
  */
  { "smime_keys",		DT_PATH, R_NONE, {.p=&SmimeKeys}, {.p=0} },
  /*
  ** .pp
  ** S/MIME には PGP における pubring/secring がないため、Mutt は鍵や証明書の格納
  ** および取得を自分で扱わなければなりません。この仕組みはとても
  ** 基本的なもので、鍵と証明書を二つの異なる ディレクトリに格納し、ともに OpenSSL で
  ** 取得したハッシュ値をファイル名にしています。各ディレクトリには index ファイルがあって、
  ** メールアドレスと鍵 ID の組み合わせが入っていて、手動で編集することができます。
  ** このオプションは秘密鍵の位置を示すものです。
  ** (S/MIME のみです)
  */
  { "smime_pk7out_command", 	DT_STR, R_NONE, {.p=&SmimePk7outCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、 X509 公開鍵を展開するために、S/MIME署名の PKCS7 構造を
  ** 展開するのに使います。
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。
  ** (S/MIME のみです)
  */
  { "smime_self_encrypt",    DT_BOOL, R_NONE, {.l=OPTSMIMESELFENCRYPT}, {.l=1} },
  /*
  ** .pp
  ** \fIset\fP の場合、S/MIME で暗号化されたメッセージは $$smime_default_key
  ** 中の証明書を使って暗号化することも出来ます。
  ** (S/MIME のみです)
  */
  { "smime_sign_as",	DT_STR,	 R_NONE, {.p=&SmimeSignAs}, {.p=0} },
  /*
  ** .pp
  ** 署名のために使う、分離されたキーがある場合、これを署名キーとして設定する
  ** 必要があります。ほとんどの場合は、$$smime_default_key を設定するだけで済みます。
  ** (S/MIME のみです)
  */
  { "smime_sign_command", 	DT_STR, R_NONE, {.p=&SmimeSignCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、すべてのメールクライアントで読むことが出来る、\fCmultipart/signed\fP
  ** タイプの S/MIME 署名を作成するのに使われます。
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。注意: %c と %k は、設定されている場合、
  ** 既定で $$smime_sign_as となり、その他の場合は、$$smime_default_key となる
  ** ことに注意してください。
  ** (S/MIME のみです)
  */
  { "smime_sign_digest_alg",	DT_STR,	 R_NONE, {.p=&SmimeDigestAlg}, {.p="sha256"} },
  /*
  ** .pp
  ** これは、メッセージダイジェストに署名するために使われるアルゴリズムを設定します。
  ** 有効なものは、``md5'', ``sha1'', ``sha224'', ``sha256'', ``sha384'', ``sha512'' です。
  ** (S/MIME のみです)
  */
  { "smime_sign_opaque_command", 	DT_STR, R_NONE, {.p=&SmimeSignOpaqueCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、S/MIME 拡張をサポートするメールクライアントでのみ扱えることができる
  ** \fCapplication/x-pkcs7-signature\fP タイプの S/MIME 署名を作成するのに使います。
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。
  ** (S/MIME のみです)
  */
  { "smime_timeout",		DT_LNUM,	 R_NONE, {.p=&SmimeTimeout}, {.l=300} },
  /*
  ** .pp
  ** キャッシュされたパスフレーズが使用されない場合、満了するまでの秒数を指定します。
  ** (S/MIME のみです)
  */
  { "smime_verify_command", 	DT_STR, R_NONE, {.p=&SmimeVerifyCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは、\fCmultipart/signed\fPタイプの S/MIME 署名を検証するのに使います。
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。
  ** (S/MIME のみです)
  */
  { "smime_verify_opaque_command", 	DT_STR, R_NONE, {.p=&SmimeVerifyOpaqueCommand}, {.p=0} },
  /*
  ** .pp
  ** このコマンドは \fCapplication/x-pkcs7-mime\fP タイプの S/MIME 署名を検証するのに
  ** 使います。  
  ** .pp
  ** これはフォーマット文字列で、\fCprintf(3)\fP 風の書式については、$$smime_decrypt_command
  ** コマンドを参照してください。
  ** (S/MIME のみです)
  */
#ifdef USE_SMTP
  { "smtp_authenticators", DT_STR, R_NONE, {.p=&SmtpAuthenticators}, {.p=0} },
  /*
  ** .pp
  ** これは、Mutt が SMTP サーバに ログインするのに使う、コロンで分離された
  ** 認証方法の一覧で、この順で Mutt が試みます。認証方法は任意の SASL メカニズム、
  ** たとえば、``digest-md5'', ``gssapi'' 又は ``cram-md5'' です。このオプションは
  ** 大文字小文字を区別しないので、``unset'' の場合は(既定値)、Mutt は
  ** すべての有効な方式をもっともセキュアなものからそうでないものの順で試します。
  ** .pp
  ** 例:
  ** .ts
  ** set smtp_authenticators="digest-md5:cram-md5"
  ** .te
  */
  { "smtp_oauth_refresh_command", DT_STR, R_NONE, {.p=&SmtpOauthRefreshCmd}, {.p=0} },
  /*
  ** .pp
  ** SMTP サーバへの接続を認証するための、 OAUTH リフレッシュトークンを生成する
  ** コマンドです。このコマンドは、各接続時に、OAUTHBEARER 認証メカニズムを使って
  ** 実行されます。詳細は ``$oauth'' を参照してください。
  */
  { "smtp_pass", 	DT_STR,  R_NONE, {.p=&SmtpPass}, {.p=0} },
  /*
  ** .pp
  ** SMTP アカウントのパスワードを指定します。\fIunset\fP の場合、Mutt は
  ** SMTP 経由で Mutt が差書にメールを送信する時にパスワードを聞いてきます。
  ** Mutt が SMTP 経由でメールを送るための設定方法は $$smtp_url も参照してください。
  ** .pp
  ** \fB注意\fP: かなり安全なマシン上で使う時のみこのオプションを使ってください。
  ** なぜなら、スーパーユーザは、このファイルを自分自身のみが読めるようにしていても、
  ** muttrc を読み取ることが出来るからです。
  */
  { "smtp_url",		DT_STR, R_NONE, {.p=&SmtpUrl}, {.p=0} },
  /*
  ** .pp
  ** 配送のために送信されたメッセージが中継される SMTP スマートホストを定義
  ** します。これは SMTP URL 形式をとります。例えば以下のようになります。
  ** .ts
  ** smtp[s]://[user[:pass]@]host[:port]
  ** .te
  ** .pp
  ** ここで、``[...]'' はオプションパートに依存します。
  ** この変数を設定すると、$$sendmail 変数の値を上書きします。
  ** .pp
  ** $$write_bcc も参照してください。
  */
#endif /* USE_SMTP */
  { "sort",		DT_SORT, R_INDEX|R_RESORT, {.p=&Sort}, {.l=SORT_DATE} },
  /*
  ** .pp
  ** どのように、``index'' メニュー中でメッセージを整列するかを指定します。
  ** 有効な値は以下の通りです。
  ** .il
  ** .dd date 又は date-sent
  ** .dd date-received
  ** .dd from
  ** .dd mailbox-order (未整列)
  ** .dd score
  ** .dd size
  ** .dd spam
  ** .dd subject
  ** .dd threads
  ** .dd to
  ** .ie
  ** .pp
  ** 逆順整列のために、接頭辞 ``reverse-'' を使う事も出来ます
  ** (例: ``\fCset sort=reverse-date-sent\fP'')。
  */
  { "sort_alias",	DT_SORT|DT_SORT_ALIAS,	R_NONE,	{.p=&SortAlias}, {.l=SORT_ALIAS} },
  /*
  ** .pp
  ** どのように ``alias'' メニューのエントリを整列するかを指定します。
  ** 使用できる値は以下の通りです。
  ** .il
  ** .dd address (メールアドレスの英語順で整列)
  ** .dd alias (別名の英語順で整列)
  ** .dd unsorted (.muttrc で指定された順そのまま)
  ** .ie
  */
  { "sort_aux",		DT_SORT|DT_SORT_AUX, R_INDEX|R_RESORT_BOTH, {.p=&SortAux}, {.l=SORT_DATE} },
  /*
  ** .pp
  ** これは、$$sort の値が2つのメッセージに対して同じだった場合に、``index''
  ** メニュー中で使われる、2番目のメッセージ整列方法を提供します。
  ** .pp
  ** スレッドで整列しているとき、この変数は、他のスレッドとの関係でスレッドを整列する
  ** 方法と、どのようにスレッドツリーのブランチを整列するかを制御します。
  ** これは、``threads'' (この場合、Mutt は ``date-sent'' のみを使用します) 以外の、
  ** $$sort で使える任意の値を設定できます。また、``reverse-'' に追加して ``last-''
  ** 接頭辞を指定することも出来ますが、``last-'' 接頭辞は ``reverse-'' の後に置かなければ
  ** なりません。``last-'' 接頭辞は、最後の子孫を持つ兄弟に対して、残りの $$sort_aux を
  ** 順番として使用し、メッセージを整列します。 
  ** たとえば、
  ** .ts
  ** set sort_aux=last-date-received
  ** .te
  ** .pp
  ** は、新規メッセージがスレッド中にある場合、そのスレッドは最後に表示される
  ** ものになります(又は、``\fCset sort=reverse-threads\fP'' を設定している場合は
  ** 最初のスレッド)。
  ** .pp
  ** 注意: 逆順スレッドの $$sort 順では、$$sort_aux は再度逆順になります
  ** (これは正しくはないのですが、既存の設定を壊さないようにしています)。
  */
  { "sort_browser",	DT_SORT|DT_SORT_BROWSER, R_NONE, {.p=&BrowserSort}, {.l=SORT_ALPHA} },
  /*
  ** .pp
  ** Specifies how to sort entries in the file browser.  By default, the
  ** entries are sorted alphabetically.  Valid values:
  ** ファイルブラウザ中でどのようにエントリを整列させるかを指定します。既定では
  ** エントリは英語順に整列されます。有効な値は以下の通りです。
  ** .il
  ** .dd alpha (英語順)
  ** .dd count
  ** .dd date
  ** .dd size
  ** .dd unread
  ** .dd unsorted
  ** .ie
  ** .pp
  ** 逆順で整列したい場合には、``reverse-'' 接頭辞を使う事が出来ます
  ** (例 ``\fCset sort_browser=reverse-date\fP'')。
  */
  { "sort_re",		DT_BOOL, R_INDEX|R_RESORT|R_RESORT_INIT, {.l=OPTSORTRE}, {.l=1} },
  /*
  ** .pp
  ** This variable is only useful when sorting by threads with
  ** $$strict_threads \fIunset\fP.  In that case, it changes the heuristic
  ** mutt uses to thread messages by subject.  With $$sort_re \fIset\fP, mutt will
  ** only attach a message as the child of another message by subject if
  ** the subject of the child message starts with a substring matching the
  ** setting of $$reply_regexp.  With $$sort_re \fIunset\fP, mutt will attach
  ** the message whether or not this is the case, as long as the
  ** non-$$reply_regexp parts of both messages are identical.
  */
  { "spam_separator",   DT_STR, R_NONE, {.p=&SpamSep}, {.p=","} },
  /*
  ** .pp
  ** This variable controls what happens when multiple spam headers
  ** are matched: if \fIunset\fP, each successive header will overwrite any
  ** previous matches value for the spam label. If \fIset\fP, each successive
  ** match will append to the previous, using this variable's value as a
  ** separator.
  */
  { "spoolfile",	DT_PATH, R_NONE, {.p=&Spoolfile}, {.p=0} },
  /*
  ** .pp
  ** If your spool mailbox is in a non-default place where Mutt cannot find
  ** it, you can specify its location with this variable.  Mutt will
  ** initially set this variable to the value of the environment
  ** variable \fC$$$MAIL\fP or \fC$$$MAILDIR\fP if either is defined.
  */
#if defined(USE_SSL)
#ifdef USE_SSL_GNUTLS
  { "ssl_ca_certificates_file", DT_PATH, R_NONE, {.p=&SslCACertFile}, {.p=0} },
  /*
  ** .pp
  ** This variable specifies a file containing trusted CA certificates.
  ** Any server certificate that is signed with one of these CA
  ** certificates is also automatically accepted. (GnuTLS only)
  ** .pp
  ** Example:
  ** .ts
  ** set ssl_ca_certificates_file=/etc/ssl/certs/ca-certificates.crt
  ** .te
  */
#endif /* USE_SSL_GNUTLS */
  { "ssl_client_cert", DT_PATH, R_NONE, {.p=&SslClientCert}, {.p=0} },
  /*
  ** .pp
  ** The file containing a client certificate and its associated private
  ** key.
  */
  { "ssl_force_tls",		DT_BOOL, R_NONE, {.l=OPTSSLFORCETLS}, {.l=0} },
  /*
  ** .pp
  ** If this variable is \fIset\fP, Mutt will require that all connections
  ** to remote servers be encrypted. Furthermore it will attempt to
  ** negotiate TLS even if the server does not advertise the capability,
  ** since it would otherwise have to abort the connection anyway. This
  ** option supersedes $$ssl_starttls.
  */
# ifdef USE_SSL_GNUTLS
  { "ssl_min_dh_prime_bits", DT_NUM, R_NONE, {.p=&SslDHPrimeBits}, {.l=0} },
  /*
  ** .pp
  ** This variable specifies the minimum acceptable prime size (in bits)
  ** for use in any Diffie-Hellman key exchange. A value of 0 will use
  ** the default from the GNUTLS library. (GnuTLS only)
  */
# endif /* USE_SSL_GNUTLS */
  { "ssl_starttls", DT_QUAD, R_NONE, {.l=OPT_SSLSTARTTLS}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** If \fIset\fP (the default), mutt will attempt to use \fCSTARTTLS\fP on servers
  ** advertising the capability. When \fIunset\fP, mutt will not attempt to
  ** use \fCSTARTTLS\fP regardless of the server's capabilities.
  */
# ifdef USE_SSL_OPENSSL
  { "ssl_use_sslv2", DT_BOOL, R_NONE, {.l=OPTSSLV2}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP , Mutt will use SSLv2 when communicating with servers that
  ** request it. \fBN.B. As of 2011, SSLv2 is considered insecure, and using
  ** is inadvisable. See https://tools.ietf.org/html/rfc6176 .\fP
  ** (OpenSSL only)
  */
# endif /* defined USE_SSL_OPENSSL */
  { "ssl_use_sslv3", DT_BOOL, R_NONE, {.l=OPTSSLV3}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP , Mutt will use SSLv3 when communicating with servers that
  ** request it. \fBN.B. As of 2015, SSLv3 is considered insecure, and using
  ** it is inadvisable. See https://tools.ietf.org/html/rfc7525 .\fP
  */
  { "ssl_use_tlsv1", DT_BOOL, R_NONE, {.l=OPTTLSV1}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP , Mutt will use TLSv1.0 when communicating with servers that
  ** request it. \fBN.B. As of 2015, TLSv1.0 is considered insecure, and using
  ** it is inadvisable. See https://tools.ietf.org/html/rfc7525 .\fP
  */
  { "ssl_use_tlsv1_1", DT_BOOL, R_NONE, {.l=OPTTLSV1_1}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP , Mutt will use TLSv1.1 when communicating with servers that
  ** request it. \fBN.B. As of 2015, TLSv1.1 is considered insecure, and using
  ** it is inadvisable. See https://tools.ietf.org/html/rfc7525 .\fP
  */
  { "ssl_use_tlsv1_2", DT_BOOL, R_NONE, {.l=OPTTLSV1_2}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP , Mutt will use TLSv1.2 when communicating with servers that
  ** request it.
  */
  { "ssl_use_tlsv1_3", DT_BOOL, R_NONE, {.l=OPTTLSV1_3}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP , Mutt will use TLSv1.3 when communicating with servers that
  ** request it.
  */
#ifdef USE_SSL_OPENSSL
  { "ssl_usesystemcerts", DT_BOOL, R_NONE, {.l=OPTSSLSYSTEMCERTS}, {.l=1} },
  /*
  ** .pp
  ** If set to \fIyes\fP, mutt will use CA certificates in the
  ** system-wide certificate store when checking if a server certificate
  ** is signed by a trusted CA. (OpenSSL only)
  */
#endif
  { "ssl_verify_dates", DT_BOOL, R_NONE, {.l=OPTSSLVERIFYDATES}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP (the default), mutt will not automatically accept a server
  ** certificate that is either not yet valid or already expired. You should
  ** only unset this for particular known hosts, using the
  ** \fC$<account-hook>\fP function.
  */
  { "ssl_verify_host", DT_BOOL, R_NONE, {.l=OPTSSLVERIFYHOST}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP (the default), mutt will not automatically accept a server
  ** certificate whose host name does not match the host used in your folder
  ** URL. You should only unset this for particular known hosts, using
  ** the \fC$<account-hook>\fP function.
  */
# ifdef USE_SSL_OPENSSL
#  ifdef HAVE_SSL_PARTIAL_CHAIN
  { "ssl_verify_partial_chains", DT_BOOL, R_NONE, {.l=OPTSSLVERIFYPARTIAL}, {.l=0} },
  /*
  ** .pp
  ** This option should not be changed from the default unless you understand
  ** what you are doing.
  ** .pp
  ** Setting this variable to \fIyes\fP will permit verifying partial
  ** certification chains, i. e. a certificate chain where not the root,
  ** but an intermediate certificate CA, or the host certificate, are
  ** marked trusted (in $$certificate_file), without marking the root
  ** signing CA as trusted.
  ** .pp
  ** (OpenSSL 1.0.2b and newer only).
  */
#  endif /* defined HAVE_SSL_PARTIAL_CHAIN */
# endif /* defined USE_SSL_OPENSSL */
  { "ssl_ciphers", DT_STR, R_NONE, {.p=&SslCiphers}, {.p=0} },
  /*
  ** .pp
  ** Contains a colon-seperated list of ciphers to use when using SSL.
  ** For OpenSSL, see ciphers(1) for the syntax of the string.
  ** .pp
  ** For GnuTLS, this option will be used in place of "NORMAL" at the
  ** start of the priority string.  See gnutls_priority_init(3) for the
  ** syntax and more details. (Note: GnuTLS version 2.1.7 or higher is
  ** required.)
  */
#endif /* defined(USE_SSL) */
  { "status_chars",	DT_MBCHARTBL, R_BOTH, {.p=&StChars}, {.p="-*%A"} },
  /*
  ** .pp
  ** Controls the characters used by the ``%r'' indicator in
  ** $$status_format. The first character is used when the mailbox is
  ** unchanged. The second is used when the mailbox has been changed, and
  ** it needs to be resynchronized. The third is used if the mailbox is in
  ** read-only mode, or if the mailbox will not be written when exiting
  ** that mailbox (You can toggle whether to write changes to a mailbox
  ** with the \fC<toggle-write>\fP operation, bound by default to ``%''). The fourth
  ** is used to indicate that the current folder has been opened in attach-
  ** message mode (Certain operations like composing a new mail, replying,
  ** forwarding, etc. are not permitted in this mode).
  */
  { "status_format",	DT_STR,	 R_BOTH, {.p=&Status}, {.p="-%r-Mutt: %f [Msgs:%?M?%M/?%m%?n? New:%n?%?o? Old:%o?%?d? Del:%d?%?F? Flag:%F?%?t? Tag:%t?%?p? Post:%p?%?b? Inc:%b?%?l? %l?]---(%s/%S)-%>-(%P)---"} },
  /*
  ** .pp
  ** Controls the format of the status line displayed in the ``index''
  ** menu.  This string is similar to $$index_format, but has its own
  ** set of \fCprintf(3)\fP-like sequences:
  ** .dl
  ** .dt %b  .dd number of mailboxes with new mail *
  ** .dt %d  .dd number of deleted messages *
  ** .dt %f  .dd the full pathname of the current mailbox
  ** .dt %F  .dd number of flagged messages *
  ** .dt %h  .dd local hostname
  ** .dt %l  .dd size (in bytes) of the current mailbox (see $formatstrings-size) *
  ** .dt %L  .dd size (in bytes) of the messages shown
  **             (i.e., which match the current limit) (see $formatstrings-size) *
  ** .dt %m  .dd the number of messages in the mailbox *
  ** .dt %M  .dd the number of messages shown (i.e., which match the current limit) *
  ** .dt %n  .dd number of new messages in the mailbox *
  ** .dt %o  .dd number of old unread messages *
  ** .dt %p  .dd number of postponed messages *
  ** .dt %P  .dd percentage of the way through the index
  ** .dt %r  .dd modified/read-only/won't-write/attach-message indicator,
  **             according to $$status_chars
  ** .dt %R  .dd number of read messages *
  ** .dt %s  .dd current sorting mode ($$sort)
  ** .dt %S  .dd current aux sorting method ($$sort_aux)
  ** .dt %t  .dd number of tagged messages *
  ** .dt %u  .dd number of unread messages *
  ** .dt %v  .dd Mutt version string
  ** .dt %V  .dd currently active limit pattern, if any *
  ** .dt %>X .dd right justify the rest of the string and pad with ``X''
  ** .dt %|X .dd pad to the end of the line with ``X''
  ** .dt %*X .dd soft-fill with character ``X'' as pad
  ** .de
  ** .pp
  ** For an explanation of ``soft-fill'', see the $$index_format documentation.
  ** .pp
  ** * = can be optionally printed if nonzero
  ** .pp
  ** Some of the above sequences can be used to optionally print a string
  ** if their value is nonzero.  For example, you may only want to see the
  ** number of flagged messages if such messages exist, since zero is not
  ** particularly meaningful.  To optionally print a string based upon one
  ** of the above sequences, the following construct is used:
  ** .pp
  **  \fC%?<sequence_char>?<optional_string>?\fP
  ** .pp
  ** where \fIsequence_char\fP is a character from the table above, and
  ** \fIoptional_string\fP is the string you would like printed if
  ** \fIsequence_char\fP is nonzero.  \fIoptional_string\fP \fBmay\fP contain
  ** other sequences as well as normal text, but you may \fBnot\fP nest
  ** optional strings.
  ** .pp
  ** Here is an example illustrating how to optionally print the number of
  ** new messages in a mailbox:
  ** .pp
  ** \fC%?n?%n new messages.?\fP
  ** .pp
  ** You can also switch between two strings using the following construct:
  ** .pp
  ** \fC%?<sequence_char>?<if_string>&<else_string>?\fP
  ** .pp
  ** If the value of \fIsequence_char\fP is non-zero, \fIif_string\fP will
  ** be expanded, otherwise \fIelse_string\fP will be expanded.
  ** .pp
  ** You can force the result of any \fCprintf(3)\fP-like sequence to be lowercase
  ** by prefixing the sequence character with an underscore (``_'') sign.
  ** For example, if you want to display the local hostname in lowercase,
  ** you would use: ``\fC%_h\fP''.
  ** .pp
  ** If you prefix the sequence character with a colon (``:'') character, mutt
  ** will replace any dots in the expansion by underscores. This might be helpful
  ** with IMAP folders that don't like dots in folder names.
  */
  { "status_on_top",	DT_BOOL, R_REFLOW, {.l=OPTSTATUSONTOP}, {.l=0} },
  /*
  ** .pp
  ** Setting this variable causes the ``status bar'' to be displayed on
  ** the first line of the screen rather than near the bottom. If $$help
  ** is \fIset\fP, too it'll be placed at the bottom.
  */
  { "strict_threads",	DT_BOOL, R_RESORT|R_RESORT_INIT|R_INDEX, {.l=OPTSTRICTTHREADS}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP, threading will only make use of the ``In-Reply-To'' and
  ** ``References:'' fields when you $$sort by message threads.  By
  ** default, messages with the same subject are grouped together in
  ** ``pseudo threads.''. This may not always be desirable, such as in a
  ** personal mailbox where you might have several unrelated messages with
  ** the subjects like ``hi'' which will get grouped together. See also
  ** $$sort_re for a less drastic way of controlling this
  ** behavior.
  */
  { "suspend",		DT_BOOL, R_NONE, {.l=OPTSUSPEND}, {.l=1} },
  /*
  ** .pp
  ** When \fIunset\fP, mutt won't stop when the user presses the terminal's
  ** \fIsusp\fP key, usually ``^Z''. This is useful if you run mutt
  ** inside an xterm using a command like ``\fCxterm -e mutt\fP''.
  */
  { "text_flowed", 	DT_BOOL, R_NONE, {.l=OPTTEXTFLOWED},  {.l=0} },
  /*
  ** .pp
  ** When \fIset\fP, mutt will generate ``format=flowed'' bodies with a content type
  ** of ``\fCtext/plain; format=flowed\fP''.
  ** This format is easier to handle for some mailing software, and generally
  ** just looks like ordinary text.  To actually make use of this format's
  ** features, you'll need support in your editor.
  ** .pp
  ** The option only controls newly composed messages.  Postponed messages,
  ** resent messages, and draft messages (via -H on the command line) will
  ** use the content-type of the source message.
  ** .pp
  ** Note that $$indent_string is ignored when this option is \fIset\fP.
  */
  { "thorough_search",	DT_BOOL, R_NONE, {.l=OPTTHOROUGHSRC}, {.l=1} },
  /*
  ** .pp
  ** Affects the \fC~b\fP and \fC~h\fP search operations described in
  ** section ``$patterns''.  If \fIset\fP, the headers and body/attachments of
  ** messages to be searched are decoded before searching. If \fIunset\fP,
  ** messages are searched as they appear in the folder.
  ** .pp
  ** Users searching attachments or for non-ASCII characters should \fIset\fP
  ** this value because decoding also includes MIME parsing/decoding and possible
  ** character set conversions. Otherwise mutt will attempt to match against the
  ** raw message received (for example quoted-printable encoded or with encoded
  ** headers) which may lead to incorrect search results.
  */
  { "thread_received",	DT_BOOL, R_RESORT|R_RESORT_INIT|R_INDEX, {.l=OPTTHREADRECEIVED}, {.l=0} },
  /*
  ** .pp
  ** When \fIset\fP, mutt uses the date received rather than the date sent
  ** to thread messages by subject.
  */
  { "tilde",		DT_BOOL, R_PAGER, {.l=OPTTILDE}, {.l=0} },
  /*
  ** .pp
  ** When \fIset\fP, the internal-pager will pad blank lines to the bottom of the
  ** screen with a tilde (``~'').
  */
  { "time_inc",		DT_NUM,	 R_NONE, {.p=&TimeInc}, {.l=0} },
  /*
  ** .pp
  ** Along with $$read_inc, $$write_inc, and $$net_inc, this
  ** variable controls the frequency with which progress updates are
  ** displayed. It suppresses updates less than $$time_inc milliseconds
  ** apart. This can improve throughput on systems with slow terminals,
  ** or when running mutt on a remote system.
  ** .pp
  ** Also see the ``$tuning'' section of the manual for performance considerations.
  */
  { "timeout",		DT_NUM,	 R_NONE, {.p=&Timeout}, {.l=600} },
  /*
  ** .pp
  ** When Mutt is waiting for user input either idling in menus or
  ** in an interactive prompt, Mutt would block until input is
  ** present. Depending on the context, this would prevent certain
  ** operations from working, like checking for new mail or keeping
  ** an IMAP connection alive.
  ** .pp
  ** This variable controls how many seconds Mutt will at most wait
  ** until it aborts waiting for input, performs these operations and
  ** continues to wait for input.
  ** .pp
  ** A value of zero or less will cause Mutt to never time out.
  */
  { "tmpdir",		DT_PATH, R_NONE, {.p=&Tempdir}, {.p=0} },
  /*
  ** .pp
  ** This variable allows you to specify where Mutt will place its
  ** temporary files needed for displaying and composing messages.  If
  ** this variable is not set, the environment variable \fC$$$TMPDIR\fP is
  ** used.  If \fC$$$TMPDIR\fP is not set then ``\fC/tmp\fP'' is used.
  */
  { "to_chars",		DT_MBCHARTBL, R_BOTH, {.p=&Tochars}, {.p=" +TCFL"} },
  /*
  ** .pp
  ** Controls the character used to indicate mail addressed to you.  The
  ** first character is the one used when the mail is \fInot\fP addressed to your
  ** address.  The second is used when you are the only
  ** recipient of the message.  The third is when your address
  ** appears in the ``To:'' header field, but you are not the only recipient of
  ** the message.  The fourth character is used when your
  ** address is specified in the ``Cc:'' header field, but you are not the only
  ** recipient.  The fifth character is used to indicate mail that was sent
  ** by \fIyou\fP.  The sixth character is used to indicate when a mail
  ** was sent to a mailing-list you subscribe to.
  */
  { "trash",		DT_PATH, R_NONE, {.p=&TrashPath}, {.p=0} },
  /*
  ** .pp
  ** If set, this variable specifies the path of the trash folder where the
  ** mails marked for deletion will be moved, instead of being irremediably
  ** purged.
  ** .pp
  ** NOTE: When you delete a message in the trash folder, it is really
  ** deleted, so that you have a way to clean the trash.
  */
  {"ts_icon_format",	DT_STR,  R_BOTH, {.p=&TSIconFormat}, {.p="M%?n?AIL&ail?"} },
  /*
  ** .pp
  ** Controls the format of the icon title, as long as ``$$ts_enabled'' is set.
  ** This string is identical in formatting to the one used by
  ** ``$$status_format''.
  */
  {"ts_enabled",	DT_BOOL,  R_BOTH, {.l=OPTTSENABLED}, {.l=0} },
  /* The default must be off to force in the validity checking. */
  /*
  ** .pp
  ** Controls whether mutt tries to set the terminal status line and icon name.
  ** Most terminal emulators emulate the status line in the window title.
  */
  {"ts_status_format",	DT_STR,   R_BOTH, {.p=&TSStatusFormat}, {.p="Mutt with %?m?%m messages&no messages?%?n? [%n NEW]?"} },
  /*
  ** .pp
  ** Controls the format of the terminal status line (or window title),
  ** provided that ``$$ts_enabled'' has been set. This string is identical in
  ** formatting to the one used by ``$$status_format''.
  */
#ifdef USE_SOCKET
  { "tunnel",            DT_STR, R_NONE, {.p=&Tunnel}, {.p=0} },
  /*
  ** .pp
  ** Setting this variable will cause mutt to open a pipe to a command
  ** instead of a raw socket. You may be able to use this to set up
  ** preauthenticated connections to your IMAP/POP3/SMTP server. Example:
  ** .ts
  ** set tunnel="ssh -q mailhost.net /usr/local/libexec/imapd"
  ** .te
  ** .pp
  ** Note: For this example to work you must be able to log in to the remote
  ** machine without having to enter a password.
  ** .pp
  ** When set, Mutt uses the tunnel for all remote connections.
  ** Please see ``$account-hook'' in the manual for how to use different
  ** tunnel commands per connection.
  */
#endif
  { "uncollapse_jump", 	DT_BOOL, R_NONE, {.l=OPTUNCOLLAPSEJUMP}, {.l=0} },
  /*
  ** .pp
  ** When \fIset\fP, Mutt will jump to the next unread message, if any,
  ** when the current thread is \fIun\fPcollapsed.
  */
  { "uncollapse_new", 	DT_BOOL, R_NONE, {.l=OPTUNCOLLAPSENEW}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, Mutt will automatically uncollapse any collapsed thread
  ** that receives a new message. When \fIunset\fP, collapsed threads will
  ** remain collapsed. the presence of the new message will still affect
  ** index sorting, though.
  */
  { "use_8bitmime",	DT_BOOL, R_NONE, {.l=OPTUSE8BITMIME}, {.l=0} },
  /*
  ** .pp
  ** \fBWarning:\fP do not set this variable unless you are using a version
  ** of sendmail which supports the \fC-B8BITMIME\fP flag (such as sendmail
  ** 8.8.x) or you may not be able to send mail.
  ** .pp
  ** When \fIset\fP, Mutt will invoke $$sendmail with the \fC-B8BITMIME\fP
  ** flag when sending 8-bit messages to enable ESMTP negotiation.
  */
  { "use_domain",	DT_BOOL, R_NONE, {.l=OPTUSEDOMAIN}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, Mutt will qualify all local addresses (ones without the
  ** ``@host'' portion) with the value of $$hostname.  If \fIunset\fP, no
  ** addresses will be qualified.
  */
  { "use_envelope_from", 	DT_BOOL, R_NONE, {.l=OPTENVFROM}, {.l=0} },
  /*
  ** .pp
  ** When \fIset\fP, mutt will set the \fIenvelope\fP sender of the message.
  ** If $$envelope_from_address is \fIset\fP, it will be used as the sender
  ** address. If \fIunset\fP, mutt will attempt to derive the sender from the
  ** ``From:'' header.
  ** .pp
  ** Note that this information is passed to sendmail command using the
  ** \fC-f\fP command line switch. Therefore setting this option is not useful
  ** if the $$sendmail variable already contains \fC-f\fP or if the
  ** executable pointed to by $$sendmail doesn't support the \fC-f\fP switch.
  */
  { "envelope_from",	DT_SYN,  R_NONE, {.p="use_envelope_from"}, {.p=0} },
  /*
  */
  { "use_from",		DT_BOOL, R_NONE, {.l=OPTUSEFROM}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, Mutt will generate the ``From:'' header field when
  ** sending messages.  If \fIunset\fP, no ``From:'' header field will be
  ** generated unless the user explicitly sets one using the ``$my_hdr''
  ** command.
  */
#ifdef HAVE_GETADDRINFO
  { "use_ipv6",		DT_BOOL, R_NONE, {.l=OPTUSEIPV6}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, Mutt will look for IPv6 addresses of hosts it tries to
  ** contact.  If this option is \fIunset\fP, Mutt will restrict itself to IPv4 addresses.
  ** Normally, the default should work.
  */
#endif /* HAVE_GETADDRINFO */
  { "user_agent",	DT_BOOL, R_NONE, {.l=OPTXMAILER}, {.l=0} },
  /*
  ** .pp
  ** When \fIset\fP, mutt will add a ``User-Agent:'' header to outgoing
  ** messages, indicating which version of mutt was used for composing
  ** them.
  */
  { "visual",		DT_PATH, R_NONE, {.p=&Visual}, {.p=0} },
  /*
  ** .pp
  ** Specifies the visual editor to invoke when the ``\fC~v\fP'' command is
  ** given in the built-in editor.
  */
  { "wait_key",		DT_BOOL, R_NONE, {.l=OPTWAITKEY}, {.l=1} },
  /*
  ** .pp
  ** Controls whether Mutt will ask you to press a key after an external command
  ** has been invoked by these functions: \fC<shell-escape>\fP,
  ** \fC<pipe-message>\fP, \fC<pipe-entry>\fP, \fC<print-message>\fP,
  ** and \fC<print-entry>\fP commands.
  ** .pp
  ** It is also used when viewing attachments with ``$auto_view'', provided
  ** that the corresponding mailcap entry has a \fIneedsterminal\fP flag,
  ** and the external program is interactive.
  ** .pp
  ** When \fIset\fP, Mutt will always ask for a key. When \fIunset\fP, Mutt will wait
  ** for a key only if the external command returned a non-zero status.
  */
  { "weed",		DT_BOOL, R_NONE, {.l=OPTWEED}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, mutt will weed headers when displaying, forwarding,
  ** printing, or replying to messages.
  */
  { "wrap",             DT_NUM,  R_PAGER, {.p=&Wrap}, {.l=0} },
  { "wrapcolumn",       DT_SYN,  R_NONE, {.p="wrap"}, {.p=0} },
  /*
  ** .pp
  ** When set to a positive value, mutt will wrap text at $$wrap characters.
  ** When set to a negative value, mutt will wrap text so that there are $$wrap
  ** characters of empty space on the right side of the terminal. Setting it
  ** to zero makes mutt wrap at the terminal width.
  ** .pp
  ** Also see $$reflow_wrap.
  */
  { "wrap_headers",     DT_NUM,  R_PAGER, {.p=&WrapHeaders}, {.l=78} },
  /*
  ** .pp
  ** This option specifies the number of characters to use for wrapping
  ** an outgoing message's headers. Allowed values are between 78 and 998
  ** inclusive.
  ** .pp
  ** \fBNote:\fP This option usually shouldn't be changed. RFC5233
  ** recommends a line length of 78 (the default), so \fBplease only change
  ** this setting when you know what you're doing\fP.
  */
  { "wrap_search",	DT_BOOL, R_NONE, {.l=OPTWRAPSEARCH}, {.l=1} },
  /*
  ** .pp
  ** Controls whether searches wrap around the end.
  ** .pp
  ** When \fIset\fP, searches will wrap around the first (or last) item. When
  ** \fIunset\fP, incremental searches will not wrap.
  */
  { "wrapmargin",	DT_NUM,	 R_PAGER, {.p=&Wrap}, {.l=0} },
  /*
  ** .pp
  ** (DEPRECATED) Equivalent to setting $$wrap with a negative value.
  */
  { "write_bcc",	DT_BOOL, R_NONE, {.l=OPTWRITEBCC}, {.l=0} },
  /*
  ** .pp
  ** Controls whether mutt writes out the ``Bcc:'' header when
  ** preparing messages to be sent.  Some MTAs, such as Exim and
  ** Courier, do not strip the ``Bcc:'' header; so it is advisable to
  ** leave this unset unless you have a particular need for the header
  ** to be in the sent message.
  ** .pp
  ** If mutt is set to deliver directly via SMTP (see $$smtp_url),
  ** this option does nothing: mutt will never write out the ``Bcc:''
  ** header in this case.
  ** .pp
  ** Note this option only affects the sending of messages.  Fcc'ed
  ** copies of a message will always contain the ``Bcc:'' header if
  ** one exists.
  */
  { "write_inc",	DT_NUM,	 R_NONE, {.p=&WriteInc}, {.l=10} },
  /*
  ** .pp
  ** When writing a mailbox, a message will be printed every
  ** $$write_inc messages to indicate progress.  If set to 0, only a
  ** single message will be displayed before writing a mailbox.
  ** .pp
  ** Also see the $$read_inc, $$net_inc and $$time_inc variables and the
  ** ``$tuning'' section of the manual for performance considerations.
  */
  {"xterm_icon",	DT_SYN,  R_NONE, {.p="ts_icon_format"}, {.p=0} },
  /*
  */
  {"xterm_title",	DT_SYN,  R_NONE, {.p="ts_status_format"}, {.p=0} },
  /*
  */
  {"xterm_set_titles",	DT_SYN,  R_NONE, {.p="ts_enabled"}, {.p=0} },
  /*
  */
  /*--*/
  { NULL, 0, 0, {.l=0}, {.l=0} }
};

const struct mapping_t SortMethods[] = {
  { "date",		SORT_DATE },
  { "date-sent",	SORT_DATE },
  { "date-received",	SORT_RECEIVED },
  { "mailbox-order",	SORT_ORDER },
  { "subject",		SORT_SUBJECT },
  { "from",		SORT_FROM },
  { "size",		SORT_SIZE },
  { "threads",		SORT_THREADS },
  { "to",		SORT_TO },
  { "score",		SORT_SCORE },
  { "spam",		SORT_SPAM },
  { "label",		SORT_LABEL },
  { NULL,               0 }
};

/* same as SortMethods, but with "threads" replaced by "date" */

const struct mapping_t SortAuxMethods[] = {
  { "date",		SORT_DATE },
  { "date-sent",	SORT_DATE },
  { "date-received",	SORT_RECEIVED },
  { "mailbox-order",	SORT_ORDER },
  { "subject",		SORT_SUBJECT },
  { "from",		SORT_FROM },
  { "size",		SORT_SIZE },
  { "threads",		SORT_DATE },	/* note: sort_aux == threads
					 * isn't possible.
					 */
  { "to",		SORT_TO },
  { "score",		SORT_SCORE },
  { "spam",		SORT_SPAM },
  { "label",		SORT_LABEL },
  { NULL,               0 }
};


const struct mapping_t SortBrowserMethods[] = {
  { "alpha",	SORT_SUBJECT },
  { "count",	SORT_COUNT },
  { "date",	SORT_DATE },
  { "size",	SORT_SIZE },
  { "unread",	SORT_UNREAD },
  { "unsorted",	SORT_ORDER },
  { NULL,       0 }
};

const struct mapping_t SortAliasMethods[] = {
  { "alias",	SORT_ALIAS },
  { "address",	SORT_ADDRESS },
  { "unsorted", SORT_ORDER },
  { NULL,       0 }
};

const struct mapping_t SortKeyMethods[] = {
  { "address",	SORT_ADDRESS },
  { "date",	SORT_DATE },
  { "keyid",	SORT_KEYID },
  { "trust",	SORT_TRUST },
  { NULL,       0 }
};

const struct mapping_t SortSidebarMethods[] = {
  { "alpha",		SORT_PATH },
  { "count",		SORT_COUNT },
  { "flagged",		SORT_FLAGGED },
  { "mailbox-order",	SORT_ORDER },
  { "name",		SORT_PATH },
  { "new",		SORT_UNREAD },  /* kept for compatibility */
  { "path",		SORT_PATH },
  { "unread",		SORT_UNREAD },
  { "unsorted",		SORT_ORDER },
  { NULL,		0 }
};


/* functions used to parse commands in a rc file */

static int parse_list (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_spam_list (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unlist (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
#ifdef USE_SIDEBAR
static int parse_path_list (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_path_unlist (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
#endif /* USE_SIDEBAR */

static int parse_group (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);

static int parse_lists (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unlists (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_alias (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unalias (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_echo (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_ignore (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unignore (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_source (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_set (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_setenv (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_my_hdr (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unmy_hdr (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_subscribe (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unsubscribe (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_attachments (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unattachments (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);


static int parse_replace_list (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unreplace_list (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_subjectrx_list (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unsubjectrx_list (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_alternates (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
static int parse_unalternates (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);

/* Parse -group arguments */
static int parse_group_context (group_context_t **ctx, BUFFER *buf, BUFFER *s, BUFFER *err);


struct command_t
{
  char *name;
  int (*func) (BUFFER *, BUFFER *, union pointer_long_t, BUFFER *);
  union pointer_long_t data;
};

const struct command_t Commands[] = {
  { "alternates",	parse_alternates,	{.l=0} },
  { "unalternates",	parse_unalternates,	{.l=0} },
#ifdef USE_SOCKET
  { "account-hook",     mutt_parse_hook,        {.l=MUTT_ACCOUNTHOOK} },
#endif
  { "alias",		parse_alias,		{.l=0} },
  { "attachments",	parse_attachments,	{.l=0} },
  { "unattachments",	parse_unattachments,	{.l=0} },
  { "auto_view",	parse_list,		{.p=&AutoViewList} },
  { "alternative_order",	parse_list,	{.p=&AlternativeOrderList} },
  { "bind",		mutt_parse_bind,	{.l=0} },
  { "charset-hook",	mutt_parse_hook,	{.l=MUTT_CHARSETHOOK} },
#ifdef HAVE_COLOR
  { "color",		mutt_parse_color,	{.l=0} },
  { "uncolor",		mutt_parse_uncolor,	{.l=0} },
#endif
  { "echo",		parse_echo,		{.l=0} },
  { "exec",		mutt_parse_exec,	{.l=0} },
  { "fcc-hook",		mutt_parse_hook,	{.l=MUTT_FCCHOOK} },
  { "fcc-save-hook",	mutt_parse_hook,	{.l=MUTT_FCCHOOK | MUTT_SAVEHOOK} },
  { "folder-hook",	mutt_parse_hook,	{.l=MUTT_FOLDERHOOK} },
#ifdef USE_COMPRESSED
  { "open-hook",	mutt_parse_hook,	{.l=MUTT_OPENHOOK} },
  { "close-hook",	mutt_parse_hook,	{.l=MUTT_CLOSEHOOK} },
  { "append-hook",	mutt_parse_hook,	{.l=MUTT_APPENDHOOK} },
#endif
  { "group",		parse_group,		{.l=MUTT_GROUP} },
  { "ungroup",		parse_group,		{.l=MUTT_UNGROUP} },
  { "hdr_order",	parse_list,		{.p=&HeaderOrderList} },
#ifdef HAVE_ICONV
  { "iconv-hook",	mutt_parse_hook,	{.l=MUTT_ICONVHOOK} },
#endif
  { "ignore",		parse_ignore,		{.l=0} },
  { "index-format-hook",mutt_parse_idxfmt_hook, {.l=MUTT_IDXFMTHOOK} },
  { "lists",		parse_lists,		{.l=0} },
  { "macro",		mutt_parse_macro,	{.l=0} },
  { "mailboxes",	mutt_parse_mailboxes,	{.l=MUTT_MAILBOXES} },
  { "unmailboxes",	mutt_parse_mailboxes,	{.l=MUTT_UNMAILBOXES} },
  { "mailto_allow",	parse_list,		{.p=&MailtoAllow} },
  { "unmailto_allow",	parse_unlist,		{.p=&MailtoAllow} },
  { "message-hook",	mutt_parse_hook,	{.l=MUTT_MESSAGEHOOK} },
  { "mbox-hook",	mutt_parse_hook,	{.l=MUTT_MBOXHOOK} },
  { "mime_lookup",	parse_list,		{.p=&MimeLookupList} },
  { "unmime_lookup",	parse_unlist,		{.p=&MimeLookupList} },
  { "mono",		mutt_parse_mono,	{.l=0} },
  { "my_hdr",		parse_my_hdr,		{.l=0} },
  { "pgp-hook",		mutt_parse_hook,	{.l=MUTT_CRYPTHOOK} },
  { "crypt-hook",	mutt_parse_hook,	{.l=MUTT_CRYPTHOOK} },
  { "push",		mutt_parse_push,	{.l=0} },
  { "reply-hook",	mutt_parse_hook,	{.l=MUTT_REPLYHOOK} },
  { "reset",		parse_set,		{.l=MUTT_SET_RESET} },
  { "save-hook",	mutt_parse_hook,	{.l=MUTT_SAVEHOOK} },
  { "score",		mutt_parse_score,	{.l=0} },
  { "send-hook",	mutt_parse_hook,	{.l=MUTT_SENDHOOK} },
  { "send2-hook",	mutt_parse_hook,	{.l=MUTT_SEND2HOOK} },
  { "set",		parse_set,		{.l=0} },
  { "setenv",		parse_setenv,		{.l=0} },
#ifdef USE_SIDEBAR
  { "sidebar_whitelist",parse_path_list,	{.p=&SidebarWhitelist} },
  { "unsidebar_whitelist",parse_path_unlist,	{.p=&SidebarWhitelist} },
#endif
  { "source",		parse_source,		{.l=0} },
  { "spam",		parse_spam_list,	{.l=MUTT_SPAM} },
  { "nospam",		parse_spam_list,	{.l=MUTT_NOSPAM} },
  { "subscribe",	parse_subscribe,	{.l=0} },
  { "subjectrx",    parse_subjectrx_list, 	{.p=&SubjectRxList} },
  { "unsubjectrx",  parse_unsubjectrx_list,	{.p=&SubjectRxList} },
  { "toggle",		parse_set,		{.l=MUTT_SET_INV} },
  { "unalias",		parse_unalias,		{.l=0} },
  { "unalternative_order",parse_unlist,		{.p=&AlternativeOrderList} },
  { "unauto_view",	parse_unlist,		{.p=&AutoViewList} },
  { "unhdr_order",	parse_unlist,		{.p=&HeaderOrderList} },
  { "unhook",		mutt_parse_unhook,	{.l=0} },
  { "unignore",		parse_unignore,		{.l=0} },
  { "unlists",		parse_unlists,		{.l=0} },
  { "unmono",		mutt_parse_unmono,	{.l=0} },
  { "unmy_hdr",		parse_unmy_hdr,		{.l=0} },
  { "unscore",		mutt_parse_unscore,	{.l=0} },
  { "unset",		parse_set,		{.l=MUTT_SET_UNSET} },
  { "unsetenv",		parse_setenv,		{.l=MUTT_SET_UNSET} },
  { "unsubscribe",	parse_unsubscribe,	{.l=0} },
  { NULL,		NULL,			{.l=0} }
};
