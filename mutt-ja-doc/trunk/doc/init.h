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
  ** (PGP only)
  */
  { "pgp_list_secring_command",	DT_STR, R_NONE, {.p=&PgpListSecringCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to list the secret key ring's contents.  The
  ** output format must be analogous to the one used by:
  ** .ts
  ** gpg --list-keys --with-colons --with-fingerprint
  ** .te
  ** .pp
  ** This format is also generated by the \fCmutt_pgpring\fP utility which comes
  ** with mutt.
  ** .pp
  ** Note: gpg's \fCfixed-list-mode\fP option should not be used.  It
  ** produces a different date format which may result in mutt showing
  ** incorrect key generation dates.
  ** .pp
  ** This is a format string, see the $$pgp_decode_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** Note that in this case, %r expands to the search string, which is a list of
  ** one or more quoted values such as email address, name, or keyid.
  ** (PGP only)
  */
  { "pgp_long_ids",	DT_BOOL, R_NONE, {.l=OPTPGPLONGIDS}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, use 64 bit PGP key IDs, if \fIunset\fP use the normal 32 bit key IDs.
  ** NOTE: Internally, Mutt has transitioned to using fingerprints (or long key IDs
  ** as a fallback).  This option now only controls the display of key IDs
  ** in the key selection menu and a few other places.
  ** (PGP only)
  */
  { "pgp_mime_auto", DT_QUAD, R_NONE, {.l=OPT_PGPMIMEAUTO}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** This option controls whether Mutt will prompt you for
  ** automatically sending a (signed/encrypted) message using
  ** PGP/MIME when inline (traditional) fails (for any reason).
  ** .pp
  ** Also note that using the old-style PGP message format is \fBstrongly\fP
  ** \fBdeprecated\fP.
  ** (PGP only)
  */
  { "pgp_auto_traditional",	DT_SYN, R_NONE, {.p="pgp_replyinline"}, {.p=0} },
  { "pgp_replyinline",		DT_BOOL, R_NONE, {.l=OPTPGPREPLYINLINE}, {.l=0} },
  /*
  ** .pp
  ** Setting this variable will cause Mutt to always attempt to
  ** create an inline (traditional) message when replying to a
  ** message which is PGP encrypted/signed inline.  This can be
  ** overridden by use of the pgp menu, when inline is not
  ** required.  This option does not automatically detect if the
  ** (replied-to) message is inline; instead it relies on Mutt
  ** internals for previously checked/flagged messages.
  ** .pp
  ** Note that Mutt might automatically use PGP/MIME for messages
  ** which consist of more than a single MIME part.  Mutt can be
  ** configured to ask before sending PGP/MIME messages when inline
  ** (traditional) would not work.
  ** .pp
  ** Also see the $$pgp_mime_auto variable.
  ** .pp
  ** Also note that using the old-style PGP message format is \fBstrongly\fP
  ** \fBdeprecated\fP.
  ** (PGP only)
  **
  */
  { "pgp_retainable_sigs", DT_BOOL, R_NONE, {.l=OPTPGPRETAINABLESIG}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP, signed and encrypted messages will consist of nested
  ** \fCmultipart/signed\fP and \fCmultipart/encrypted\fP body parts.
  ** .pp
  ** This is useful for applications like encrypted and signed mailing
  ** lists, where the outer layer (\fCmultipart/encrypted\fP) can be easily
  ** removed, while the inner \fCmultipart/signed\fP part is retained.
  ** (PGP only)
  */
  { "pgp_self_encrypt",    DT_BOOL, R_NONE, {.l=OPTPGPSELFENCRYPT}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, PGP encrypted messages will also be encrypted
  ** using the key in $$pgp_default_key.
  ** (PGP only)
  */
  { "pgp_show_unusable", DT_BOOL, R_NONE, {.l=OPTPGPSHOWUNUSABLE}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, mutt will display non-usable keys on the PGP key selection
  ** menu.  This includes keys which have been revoked, have expired, or
  ** have been marked as ``disabled'' by the user.
  ** (PGP only)
  */
  { "pgp_sign_as",	DT_STR,	 R_NONE, {.p=&PgpSignAs}, {.p=0} },
  /*
  ** .pp
  ** If you have a different key pair to use for signing, you should
  ** set this to the signing key.  Most people will only need to set
  ** $$pgp_default_key.  It is recommended that you use the keyid form
  ** to specify your key (e.g. \fC0x00112233\fP).
  ** (PGP only)
  */
  { "pgp_sign_command",		DT_STR, R_NONE, {.p=&PgpSignCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to create the detached PGP signature for a
  ** \fCmultipart/signed\fP PGP/MIME body part.
  ** .pp
  ** This is a format string, see the $$pgp_decode_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (PGP only)
  */
  { "pgp_sort_keys",	DT_SORT|DT_SORT_KEYS, R_NONE, {.p=&PgpSortKeys}, {.l=SORT_ADDRESS} },
  /*
  ** .pp
  ** Specifies how the entries in the pgp menu are sorted. The
  ** following are legal values:
  ** .dl
  ** .dt address .dd sort alphabetically by user id
  ** .dt keyid   .dd sort alphabetically by key id
  ** .dt date    .dd sort by key creation date
  ** .dt trust   .dd sort by the trust of the key
  ** .de
  ** .pp
  ** If you prefer reverse order of the above values, prefix it with
  ** ``reverse-''.
  ** (PGP only)
  */
  { "pgp_strict_enc",	DT_BOOL, R_NONE, {.l=OPTPGPSTRICTENC}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, Mutt will automatically encode PGP/MIME signed messages as
  ** quoted-printable.  Please note that unsetting this variable may
  ** lead to problems with non-verifyable PGP signatures, so only change
  ** this if you know what you are doing.
  ** (PGP only)
  */
  { "pgp_timeout",	DT_LNUM,	 R_NONE, {.p=&PgpTimeout}, {.l=300} },
  /*
  ** .pp
  ** The number of seconds after which a cached passphrase will expire if
  ** not used.
  ** (PGP only)
  */
  { "pgp_use_gpg_agent", DT_BOOL, R_NONE, {.l=OPTUSEGPGAGENT}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, mutt expects a \fCgpg-agent(1)\fP process will handle
  ** private key passphrase prompts.  If \fIunset\fP, mutt will prompt
  ** for the passphrase and pass it via stdin to the pgp command.
  ** .pp
  ** Note that as of version 2.1, GnuPG automatically spawns an agent
  ** and requires the agent be used for passphrase management.  Since
  ** that version is increasingly prevalent, this variable now
  ** defaults \fIset\fP.
  ** .pp
  ** Mutt works with a GUI or curses pinentry program.  A TTY pinentry
  ** should not be used.
  ** .pp
  ** If you are using an older version of GnuPG without an agent running,
  ** or another encryption program without an agent, you will need to
  ** \fIunset\fP this variable.
  ** (PGP only)
  */
  { "pgp_verify_command", 	DT_STR, R_NONE, {.p=&PgpVerifyCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to verify PGP signatures.
  ** .pp
  ** This is a format string, see the $$pgp_decode_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (PGP only)
  */
  { "pgp_verify_key_command",	DT_STR, R_NONE, {.p=&PgpVerifyKeyCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to verify key information from the key selection
  ** menu.
  ** .pp
  ** This is a format string, see the $$pgp_decode_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (PGP only)
  */
  { "pipe_decode",	DT_BOOL, R_NONE, {.l=OPTPIPEDECODE}, {.l=0} },
  /*
  ** .pp
  ** Used in connection with the \fC<pipe-message>\fP command.  When \fIunset\fP,
  ** Mutt will pipe the messages without any preprocessing. When \fIset\fP, Mutt
  ** will weed headers and will attempt to decode the messages
  ** first.
  */
  { "pipe_sep",		DT_STR,	 R_NONE, {.p=&PipeSep}, {.p="\n"} },
  /*
  ** .pp
  ** The separator to add between messages when piping a list of tagged
  ** messages to an external Unix command.
  */
  { "pipe_split",	DT_BOOL, R_NONE, {.l=OPTPIPESPLIT}, {.l=0} },
  /*
  ** .pp
  ** Used in connection with the \fC<pipe-message>\fP function following
  ** \fC<tag-prefix>\fP.  If this variable is \fIunset\fP, when piping a list of
  ** tagged messages Mutt will concatenate the messages and will pipe them
  ** all concatenated.  When \fIset\fP, Mutt will pipe the messages one by one.
  ** In both cases the messages are piped in the current sorted order,
  ** and the $$pipe_sep separator is added after each message.
  */
#ifdef USE_POP
  { "pop_auth_try_all",	DT_BOOL, R_NONE, {.l=OPTPOPAUTHTRYALL}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, Mutt will try all available authentication methods.
  ** When \fIunset\fP, Mutt will only fall back to other authentication
  ** methods if the previous methods are unavailable. If a method is
  ** available but authentication fails, Mutt will not connect to the POP server.
  */
  { "pop_authenticators", DT_STR, R_NONE, {.p=&PopAuthenticators}, {.p=0} },
  /*
  ** .pp
  ** This is a colon-delimited list of authentication methods mutt may
  ** attempt to use to log in to an POP server, in the order mutt should
  ** try them.  Authentication methods are either ``user'', ``apop'' or any
  ** SASL mechanism, e.g. ``digest-md5'', ``gssapi'' or ``cram-md5''.
  ** This option is case-insensitive. If this option is \fIunset\fP
  ** (the default) mutt will try all available methods, in order from
  ** most-secure to least-secure.
  ** .pp
  ** Example:
  ** .ts
  ** set pop_authenticators="digest-md5:apop:user"
  ** .te
  */
  { "pop_checkinterval", DT_NUM, R_NONE, {.p=&PopCheckTimeout}, {.l=60} },
  /*
  ** .pp
  ** This variable configures how often (in seconds) mutt should look for
  ** new mail in the currently selected mailbox if it is a POP mailbox.
  */
  { "pop_delete",	DT_QUAD, R_NONE, {.l=OPT_POPDELETE}, {.l=MUTT_ASKNO} },
  /*
  ** .pp
  ** If \fIset\fP, Mutt will delete successfully downloaded messages from the POP
  ** server when using the \fC$<fetch-mail>\fP function.  When \fIunset\fP, Mutt will
  ** download messages but also leave them on the POP server.
  */
  { "pop_host",		DT_STR,	 R_NONE, {.p=&PopHost}, {.p=0} },
  /*
  ** .pp
  ** The name of your POP server for the \fC$<fetch-mail>\fP function.  You
  ** can also specify an alternative port, username and password, i.e.:
  ** .ts
  ** [pop[s]://][username[:password]@]popserver[:port]
  ** .te
  ** .pp
  ** where ``[...]'' denotes an optional part.
  */
  { "pop_last",		DT_BOOL, R_NONE, {.l=OPTPOPLAST}, {.l=0} },
  /*
  ** .pp
  ** If this variable is \fIset\fP, mutt will try to use the ``\fCLAST\fP'' POP command
  ** for retrieving only unread messages from the POP server when using
  ** the \fC$<fetch-mail>\fP function.
  */
  { "pop_oauth_refresh_command", DT_STR, R_NONE, {.p=&PopOauthRefreshCmd}, {.p=0} },
  /*
  ** .pp
  ** The command to run to generate an OAUTH refresh token for
  ** authorizing your connection to your POP server.  This command will be
  ** run on every connection attempt that uses the OAUTHBEARER authentication
  ** mechanism.  See ``$oauth'' for details.
  */
  { "pop_pass",		DT_STR,	 R_NONE, {.p=&PopPass}, {.p=0} },
  /*
  ** .pp
  ** Specifies the password for your POP account.  If \fIunset\fP, Mutt will
  ** prompt you for your password when you open a POP mailbox.
  ** .pp
  ** \fBWarning\fP: you should only use this option when you are on a
  ** fairly secure machine, because the superuser can read your muttrc
  ** even if you are the only one who can read the file.
  */
  { "pop_reconnect",	DT_QUAD, R_NONE, {.l=OPT_POPRECONNECT}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** Controls whether or not Mutt will try to reconnect to the POP server if
  ** the connection is lost.
  */
  { "pop_user",		DT_STR,	 R_NONE, {.p=&PopUser}, {.p=0} },
  /*
  ** .pp
  ** Your login name on the POP server.
  ** .pp
  ** This variable defaults to your user name on the local machine.
  */
#endif /* USE_POP */
  { "post_indent_string",DT_STR, R_NONE, {.p=&PostIndentString}, {.p=0} },
  /*
  ** .pp
  ** Similar to the $$attribution variable, Mutt will append this
  ** string after the inclusion of a message which is being replied to.
  ** For a full listing of defined \fCprintf(3)\fP-like sequences see
  ** the section on $$index_format.
  */
  { "post_indent_str",  DT_SYN,  R_NONE, {.p="post_indent_string"}, {.p=0} },
  /*
  */
  { "postpone",		DT_QUAD, R_NONE, {.l=OPT_POSTPONE}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** Controls whether or not messages are saved in the $$postponed
  ** mailbox when you elect not to send immediately.
  ** .pp
  ** Also see the $$recall variable.
  */
  { "postponed",	DT_PATH, R_INDEX, {.p=&Postponed}, {.p="~/postponed"} },
  /*
  ** .pp
  ** Mutt allows you to indefinitely ``$postpone sending a message'' which
  ** you are editing.  When you choose to postpone a message, Mutt saves it
  ** in the mailbox specified by this variable.
  ** .pp
  ** Also see the $$postpone variable.
  */
  { "postpone_encrypt",    DT_BOOL, R_NONE, {.l=OPTPOSTPONEENCRYPT}, {.l=0} },
  /*
  ** .pp
  ** When \fIset\fP, postponed messages that are marked for encryption will be
  ** self-encrypted.  Mutt will first try to encrypt using the value specified
  ** in $$pgp_default_key or $$smime_default_key.  If those are not
  ** set, it will try the deprecated $$postpone_encrypt_as.
  ** (Crypto only)
  */
  { "postpone_encrypt_as", DT_STR,  R_NONE, {.p=&PostponeEncryptAs}, {.p=0} },
  /*
  ** .pp
  ** This is a deprecated fall-back variable for $$postpone_encrypt.
  ** Please use $$pgp_default_key or $$smime_default_key.
  ** (Crypto only)
  */
#ifdef USE_SOCKET
  { "preconnect",	DT_STR, R_NONE, {.p=&Preconnect}, {.p=0} },
  /*
  ** .pp
  ** If \fIset\fP, a shell command to be executed if mutt fails to establish
  ** a connection to the server. This is useful for setting up secure
  ** connections, e.g. with \fCssh(1)\fP. If the command returns a  nonzero
  ** status, mutt gives up opening the server. Example:
  ** .ts
  ** set preconnect="ssh -f -q -L 1234:mailhost.net:143 mailhost.net \(rs
  ** sleep 20 < /dev/null > /dev/null"
  ** .te
  ** .pp
  ** Mailbox ``foo'' on ``mailhost.net'' can now be reached
  ** as ``{localhost:1234}foo''.
  ** .pp
  ** Note: For this example to work, you must be able to log in to the
  ** remote machine without having to enter a password.
  */
#endif /* USE_SOCKET */
  { "print",		DT_QUAD, R_NONE, {.l=OPT_PRINT}, {.l=MUTT_ASKNO} },
  /*
  ** .pp
  ** Controls whether or not Mutt really prints messages.
  ** This is set to ``ask-no'' by default, because some people
  ** accidentally hit ``p'' often.
  */
  { "print_command",	DT_PATH, R_NONE, {.p=&PrintCmd}, {.p="lpr"} },
  /*
  ** .pp
  ** This specifies the command pipe that should be used to print messages.
  */
  { "print_cmd",	DT_SYN,  R_NONE, {.p="print_command"}, {.p=0} },
  /*
  */
  { "print_decode",	DT_BOOL, R_NONE, {.l=OPTPRINTDECODE}, {.l=1} },
  /*
  ** .pp
  ** Used in connection with the \fC<print-message>\fP command.  If this
  ** option is \fIset\fP, the message is decoded before it is passed to the
  ** external command specified by $$print_command.  If this option
  ** is \fIunset\fP, no processing will be applied to the message when
  ** printing it.  The latter setting may be useful if you are using
  ** some advanced printer filter which is able to properly format
  ** e-mail messages for printing.
  */
  { "print_split",	DT_BOOL, R_NONE, {.l=OPTPRINTSPLIT},  {.l=0} },
  /*
  ** .pp
  ** Used in connection with the \fC<print-message>\fP command.  If this option
  ** is \fIset\fP, the command specified by $$print_command is executed once for
  ** each message which is to be printed.  If this option is \fIunset\fP,
  ** the command specified by $$print_command is executed only once, and
  ** all the messages are concatenated, with a form feed as the message
  ** separator.
  ** .pp
  ** Those who use the \fCenscript\fP(1) program's mail-printing mode will
  ** most likely want to \fIset\fP this option.
  */
  { "prompt_after",	DT_BOOL, R_NONE, {.l=OPTPROMPTAFTER}, {.l=1} },
  /*
  ** .pp
  ** If you use an \fIexternal\fP $$pager, setting this variable will
  ** cause Mutt to prompt you for a command when the pager exits rather
  ** than returning to the index menu.  If \fIunset\fP, Mutt will return to the
  ** index menu when the external pager exits.
  */
  { "query_command",	DT_PATH, R_NONE, {.p=&QueryCmd}, {.p=0} },
  /*
  ** .pp
  ** This specifies the command Mutt will use to make external address
  ** queries.  The string may contain a ``%s'', which will be substituted
  ** with the query string the user types.  Mutt will add quotes around the
  ** string substituted for ``%s'' automatically according to shell quoting
  ** rules, so you should avoid adding your own.  If no ``%s'' is found in
  ** the string, Mutt will append the user's query to the end of the string.
  ** See ``$query'' for more information.
  */
  { "query_format",	DT_STR, R_NONE, {.p=&QueryFormat}, {.p="%4c %t %-25.25a %-25.25n %?e?(%e)?"} },
  /*
  ** .pp
  ** This variable describes the format of the ``query'' menu. The
  ** following \fCprintf(3)\fP-style sequences are understood:
  ** .dl
  ** .dt %a  .dd destination address
  ** .dt %c  .dd current entry number
  ** .dt %e  .dd extra information *
  ** .dt %n  .dd destination name
  ** .dt %t  .dd ``*'' if current entry is tagged, a space otherwise
  ** .dt %>X .dd right justify the rest of the string and pad with ``X''
  ** .dt %|X .dd pad to the end of the line with ``X''
  ** .dt %*X .dd soft-fill with character ``X'' as pad
  ** .de
  ** .pp
  ** For an explanation of ``soft-fill'', see the $$index_format documentation.
  ** .pp
  ** * = can be optionally printed if nonzero, see the $$status_format documentation.
  */
  { "quit",		DT_QUAD, R_NONE, {.l=OPT_QUIT}, {.l=MUTT_YES} },
  /*
  ** .pp
  ** This variable controls whether ``quit'' and ``exit'' actually quit
  ** from mutt.  If this option is \fIset\fP, they do quit, if it is \fIunset\fP, they
  ** have no effect, and if it is set to \fIask-yes\fP or \fIask-no\fP, you are
  ** prompted for confirmation when you try to quit.
  */
  { "quote_regexp",	DT_RX,	 R_PAGER, {.p=&QuoteRegexp}, {.p="^([ \t]*[|>:}#])+"} },
  /*
  ** .pp
  ** A regular expression used in the internal pager to determine quoted
  ** sections of text in the body of a message. Quoted text may be filtered
  ** out using the \fC<toggle-quoted>\fP command, or colored according to the
  ** ``color quoted'' family of directives.
  ** .pp
  ** Higher levels of quoting may be colored differently (``color quoted1'',
  ** ``color quoted2'', etc.). The quoting level is determined by removing
  ** the last character from the matched text and recursively reapplying
  ** the regular expression until it fails to produce a match.
  ** .pp
  ** Match detection may be overridden by the $$smileys regular expression.
  */
  { "read_inc",		DT_NUM,	 R_NONE, {.p=&ReadInc}, {.l=10} },
  /*
  ** .pp
  ** If set to a value greater than 0, Mutt will display which message it
  ** is currently on when reading a mailbox or when performing search actions
  ** such as search and limit. The message is printed after
  ** this many messages have been read or searched (e.g., if set to 25, Mutt will
  ** print a message when it is at message 25, and then again when it gets
  ** to message 50).  This variable is meant to indicate progress when
  ** reading or searching large mailboxes which may take some time.
  ** When set to 0, only a single message will appear before the reading
  ** the mailbox.
  ** .pp
  ** Also see the $$write_inc, $$net_inc and $$time_inc variables and the
  ** ``$tuning'' section of the manual for performance considerations.
  */
  { "read_only",	DT_BOOL, R_NONE, {.l=OPTREADONLY}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP, all folders are opened in read-only mode.
  */
  { "realname",		DT_STR,	 R_BOTH, {.p=&Realname}, {.p=0} },
  /*
  ** .pp
  ** This variable specifies what ``real'' or ``personal'' name should be used
  ** when sending messages.
  ** .pp
  ** By default, this is the GECOS field from \fC/etc/passwd\fP.  Note that this
  ** variable will \fInot\fP be used when the user has set a real name
  ** in the $$from variable.
  */
  { "recall",		DT_QUAD, R_NONE, {.l=OPT_RECALL}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** Controls whether or not Mutt recalls postponed messages
  ** when composing a new message.
  ** .pp
  ** Setting this variable to \fIyes\fP is not generally useful, and thus not
  ** recommended.  Note that the \fC<recall-message>\fP function can be used
  ** to manually recall postponed messages.
  ** .pp
  ** Also see $$postponed variable.
  */
  { "record",		DT_PATH, R_NONE, {.p=&Outbox}, {.p="~/sent"} },
  /*
  ** .pp
  ** This specifies the file into which your outgoing messages should be
  ** appended.  (This is meant as the primary method for saving a copy of
  ** your messages, but another way to do this is using the ``$my_hdr''
  ** command to create a ``Bcc:'' field with your email address in it.)
  ** .pp
  ** The value of \fI$$record\fP is overridden by the $$force_name and
  ** $$save_name variables, and the ``$fcc-hook'' command.  Also see $$copy
  ** and $$write_bcc.
  */
  { "reflow_space_quotes",	DT_BOOL, R_NONE, {.l=OPTREFLOWSPACEQUOTES}, {.l=1} },
  /*
  ** .pp
  ** This option controls how quotes from format=flowed messages are displayed
  ** in the pager and when replying (with $$text_flowed \fIunset\fP).
  ** When set, this option adds spaces after each level of quote marks, turning
  ** ">>>foo" into "> > > foo".
  ** .pp
  ** \fBNote:\fP If $$reflow_text is \fIunset\fP, this option has no effect.
  ** Also, this option does not affect replies when $$text_flowed is \fIset\fP.
  */
  { "reflow_text",	DT_BOOL, R_NONE, {.l=OPTREFLOWTEXT}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, Mutt will reformat paragraphs in text/plain
  ** parts marked format=flowed.  If \fIunset\fP, Mutt will display paragraphs
  ** unaltered from how they appear in the message body.  See RFC3676 for
  ** details on the \fIformat=flowed\fP format.
  ** .pp
  ** Also see $$reflow_wrap, and $$wrap.
  */
  { "reflow_wrap",	DT_NUM,	R_NONE, {.p=&ReflowWrap}, {.l=78} },
  /*
  ** .pp
  ** This variable controls the maximum paragraph width when reformatting text/plain
  ** parts when $$reflow_text is \fIset\fP.  When the value is 0, paragraphs will
  ** be wrapped at the terminal's right margin.  A positive value sets the
  ** paragraph width relative to the left margin.  A negative value set the
  ** paragraph width relative to the right margin.
  ** .pp
  ** Also see $$wrap.
  */
  { "reply_regexp",	DT_RX,	 R_INDEX|R_RESORT, {.p=&ReplyRegexp}, {.p="^(re([\\[0-9\\]+])*|aw):[ \t]*"} },
  /*
  ** .pp
  ** A regular expression used to recognize reply messages when threading
  ** and replying. The default value corresponds to the English "Re:" and
  ** the German "Aw:".
  */
  { "reply_self",	DT_BOOL, R_NONE, {.l=OPTREPLYSELF}, {.l=0} },
  /*
  ** .pp
  ** If \fIunset\fP and you are replying to a message sent by you, Mutt will
  ** assume that you want to reply to the recipients of that message rather
  ** than to yourself.
  ** .pp
  ** Also see the ``$alternates'' command.
  */
  { "reply_to",		DT_QUAD, R_NONE, {.l=OPT_REPLYTO}, {.l=MUTT_ASKYES} },
  /*
  ** .pp
  ** If \fIset\fP, when replying to a message, Mutt will use the address listed
  ** in the Reply-to: header as the recipient of the reply.  If \fIunset\fP,
  ** it will use the address in the From: header field instead.  This
  ** option is useful for reading a mailing list that sets the Reply-To:
  ** header field to the list address and you want to send a private
  ** message to the author of a message.
  */
  { "resolve",		DT_BOOL, R_NONE, {.l=OPTRESOLVE}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, the cursor will be automatically advanced to the next
  ** (possibly undeleted) message whenever a command that modifies the
  ** current message is executed.
  */
  { "resume_draft_files", DT_BOOL, R_NONE, {.l=OPTRESUMEDRAFTFILES}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP, draft files (specified by \fC-H\fP on the command
  ** line) are processed similarly to when resuming a postponed
  ** message.  Recipients are not prompted for; send-hooks are not
  ** evaluated; no alias expansion takes place; user-defined headers
  ** and signatures are not added to the message.
  */
  { "resume_edited_draft_files", DT_BOOL, R_NONE, {.l=OPTRESUMEEDITEDDRAFTFILES}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, draft files previously edited (via \fC-E -H\fP on
  ** the command line) will have $$resume_draft_files automatically
  ** set when they are used as a draft file again.
  ** .pp
  ** The first time a draft file is saved, mutt will add a header,
  ** X-Mutt-Resume-Draft to the saved file.  The next time the draft
  ** file is read in, if mutt sees the header, it will set
  ** $$resume_draft_files.
  ** .pp
  ** This option is designed to prevent multiple signatures,
  ** user-defined headers, and other processing effects from being
  ** made multiple times to the draft file.
  */
  { "reverse_alias",	DT_BOOL, R_BOTH, {.l=OPTREVALIAS}, {.l=0} },
  /*
  ** .pp
  ** This variable controls whether or not Mutt will display the ``personal''
  ** name from your aliases in the index menu if it finds an alias that
  ** matches the message's sender.  For example, if you have the following
  ** alias:
  ** .ts
  ** alias juser abd30425@somewhere.net (Joe User)
  ** .te
  ** .pp
  ** and then you receive mail which contains the following header:
  ** .ts
  ** From: abd30425@somewhere.net
  ** .te
  ** .pp
  ** It would be displayed in the index menu as ``Joe User'' instead of
  ** ``abd30425@somewhere.net.''  This is useful when the person's e-mail
  ** address is not human friendly.
  */
  { "reverse_name",	DT_BOOL, R_BOTH, {.l=OPTREVNAME}, {.l=0} },
  /*
  ** .pp
  ** It may sometimes arrive that you receive mail to a certain machine,
  ** move the messages to another machine, and reply to some the messages
  ** from there.  If this variable is \fIset\fP, the default \fIFrom:\fP line of
  ** the reply messages is built using the address where you received the
  ** messages you are replying to \fBif\fP that address matches your
  ** ``$alternates''.  If the variable is \fIunset\fP, or the address that would be
  ** used doesn't match your ``$alternates'', the \fIFrom:\fP line will use
  ** your address on the current machine.
  ** .pp
  ** Also see the ``$alternates'' command and $$reverse_realname.
  */
  { "reverse_realname",	DT_BOOL, R_BOTH, {.l=OPTREVREAL}, {.l=1} },
  /*
  ** .pp
  ** This variable fine-tunes the behavior of the $$reverse_name feature.
  ** .pp
  ** When it is \fIunset\fP, Mutt will remove the real name part of a
  ** matching address.  This allows the use of the email address
  ** without having to also use what the sender put in the real name
  ** field.
  ** .pp
  ** When it is \fIset\fP, Mutt will use the matching address as-is.
  ** .pp
  ** In either case, a missing real name will be filled in afterwards
  ** using the value of $$realname.
  */
  { "rfc2047_parameters", DT_BOOL, R_NONE, {.l=OPTRFC2047PARAMS}, {.l=0} },
  /*
  ** .pp
  ** When this variable is \fIset\fP, Mutt will decode RFC2047-encoded MIME
  ** parameters. You want to set this variable when mutt suggests you
  ** to save attachments to files named like:
  ** .ts
  ** =?iso-8859-1?Q?file=5F=E4=5F991116=2Ezip?=
  ** .te
  ** .pp
  ** When this variable is \fIset\fP interactively, the change won't be
  ** active until you change folders.
  ** .pp
  ** Note that this use of RFC2047's encoding is explicitly
  ** prohibited by the standard, but nevertheless encountered in the
  ** wild.
  ** .pp
  ** Also note that setting this parameter will \fInot\fP have the effect
  ** that mutt \fIgenerates\fP this kind of encoding.  Instead, mutt will
  ** unconditionally use the encoding specified in RFC2231.
  */
  { "save_address",	DT_BOOL, R_NONE, {.l=OPTSAVEADDRESS}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP, mutt will take the sender's full address when choosing a
  ** default folder for saving a mail. If $$save_name or $$force_name
  ** is \fIset\fP too, the selection of the Fcc folder will be changed as well.
  */
  { "save_empty",	DT_BOOL, R_NONE, {.l=OPTSAVEEMPTY}, {.l=1} },
  /*
  ** .pp
  ** When \fIunset\fP, mailboxes which contain no saved messages will be removed
  ** when closed (the exception is $$spoolfile which is never removed).
  ** If \fIset\fP, mailboxes are never removed.
  ** .pp
  ** \fBNote:\fP This only applies to mbox and MMDF folders, Mutt does not
  ** delete MH and Maildir directories.
  */
  { "save_history",     DT_NUM,  R_NONE, {.p=&SaveHist}, {.l=0} },
  /*
  ** .pp
  ** This variable controls the size of the history (per category) saved in the
  ** $$history_file file.
  */
  { "save_name",	DT_BOOL, R_NONE, {.l=OPTSAVENAME}, {.l=0} },
  /*
  ** .pp
  ** This variable controls how copies of outgoing messages are saved.
  ** When \fIset\fP, a check is made to see if a mailbox specified by the
  ** recipient address exists (this is done by searching for a mailbox in
  ** the $$folder directory with the \fIusername\fP part of the
  ** recipient address).  If the mailbox exists, the outgoing message will
  ** be saved to that mailbox, otherwise the message is saved to the
  ** $$record mailbox.
  ** .pp
  ** Also see the $$force_name variable.
  */
  { "send_group_reply_to",	DT_BOOL, R_NONE, {.l=OPTSENDGROUPREPLYTO}, {.l=0} },
  /*
  ** .pp
  ** This variable controls how group replies are done.
  ** When set, all recepients listet in "To:" are set in the
  ** "To:" header again, else in the "CC", which is the default.
  */
  { "score", 		DT_BOOL, R_NONE, {.l=OPTSCORE}, {.l=1} },
  /*
  ** .pp
  ** When this variable is \fIunset\fP, scoring is turned off.  This can
  ** be useful to selectively disable scoring for certain folders when the
  ** $$score_threshold_delete variable and related are used.
  **
  */
  { "score_threshold_delete", DT_NUM, R_NONE, {.p=&ScoreThresholdDelete}, {.l=-1} },
  /*
  ** .pp
  ** Messages which have been assigned a score equal to or lower than the value
  ** of this variable are automatically marked for deletion by mutt.  Since
  ** mutt scores are always greater than or equal to zero, the default setting
  ** of this variable will never mark a message for deletion.
  */
  { "score_threshold_flag", DT_NUM, R_NONE, {.p=&ScoreThresholdFlag}, {.l=9999} },
  /*
  ** .pp
  ** Messages which have been assigned a score greater than or equal to this
  ** variable's value are automatically marked "flagged".
  */
  { "score_threshold_read", DT_NUM, R_NONE, {.p=&ScoreThresholdRead}, {.l=-1} },
  /*
  ** .pp
  ** Messages which have been assigned a score equal to or lower than the value
  ** of this variable are automatically marked as read by mutt.  Since
  ** mutt scores are always greater than or equal to zero, the default setting
  ** of this variable will never mark a message read.
  */
  { "search_context",	DT_NUM,  R_NONE, {.p=&SearchContext}, {.l=0} },
  /*
  ** .pp
  ** For the pager, this variable specifies the number of lines shown
  ** before search results. By default, search results will be top-aligned.
  */
  { "send_charset",	DT_STR,  R_NONE, {.p=&SendCharset}, {.p="us-ascii:iso-8859-1:utf-8"} },
  /*
  ** .pp
  ** A colon-delimited list of character sets for outgoing messages. Mutt will use the
  ** first character set into which the text can be converted exactly.
  ** If your $$charset is not ``iso-8859-1'' and recipients may not
  ** understand ``UTF-8'', it is advisable to include in the list an
  ** appropriate widely used standard character set (such as
  ** ``iso-8859-2'', ``koi8-r'' or ``iso-2022-jp'') either instead of or after
  ** ``iso-8859-1''.
  ** .pp
  ** In case the text cannot be converted into one of these exactly,
  ** mutt uses $$charset as a fallback.
  */
  { "send_multipart_alternative", DT_QUAD, R_NONE, {.l=OPT_SENDMULTIPARTALT}, {.l=MUTT_NO} },
  /*
  ** .pp
  ** If \fIset\fP, Mutt will generate a multipart/alternative
  ** container and an alternative part using the filter script specified in
  ** $$send_multipart_alternative_filter.
  ** See the section ``MIME Multipart/Alternative'' ($alternative-order).
  ** .pp
  ** Note that enabling multipart/alternative is not compatible with inline
  ** PGP encryption.  Mutt will prompt to use PGP/MIME in that case.
  */
  { "send_multipart_alternative_filter", DT_PATH, R_NONE, {.p=&SendMultipartAltFilter}, {.p=0} },
  /*
  ** .pp
  ** This specifies a filter script, which will convert the main
  ** (composed) message of the email to an alternative format.  The
  ** message will be piped to the filter's stdin.  The expected output
  ** of the filter is the generated mime type, e.g. text/html,
  ** followed by a blank line, and then the converted content.
  ** See the section ``MIME Multipart/Alternative'' ($alternative-order).
  */
  { "sendmail",		DT_PATH, R_NONE, {.p=&Sendmail}, {.p=SENDMAIL " -oem -oi"} },
  /*
  ** .pp
  ** Specifies the program and arguments used to deliver mail sent by Mutt.
  ** Mutt expects that the specified program interprets additional
  ** arguments as recipient addresses.  Mutt appends all recipients after
  ** adding a \fC--\fP delimiter (if not already present).  Additional
  ** flags, such as for $$use_8bitmime, $$use_envelope_from,
  ** $$dsn_notify, or $$dsn_return will be added before the delimiter.
  ** .pp
  ** \fBSee also:\fP $$write_bcc.
  */
  { "sendmail_wait",	DT_NUM,  R_NONE, {.p=&SendmailWait}, {.l=0} },
  /*
  ** .pp
  ** Specifies the number of seconds to wait for the $$sendmail process
  ** to finish before giving up and putting delivery in the background.
  ** .pp
  ** Mutt interprets the value of this variable as follows:
  ** .dl
  ** .dt >0 .dd number of seconds to wait for sendmail to finish before continuing
  ** .dt 0  .dd wait forever for sendmail to finish
  ** .dt <0 .dd always put sendmail in the background without waiting
  ** .de
  ** .pp
  ** Note that if you specify a value other than 0, the output of the child
  ** process will be put in a temporary file.  If there is some error, you
  ** will be informed as to where to find the output.
  */
  { "shell",		DT_PATH, R_NONE, {.p=&Shell}, {.p=0} },
  /*
  ** .pp
  ** Command to use when spawning a subshell.  By default, the user's login
  ** shell from \fC/etc/passwd\fP is used.
  */
#ifdef USE_SIDEBAR
  { "sidebar_delim_chars", DT_STR, R_SIDEBAR, {.p=&SidebarDelimChars}, {.p="/."} },
  /*
  ** .pp
  ** This contains the list of characters which you would like to treat
  ** as folder separators for displaying paths in the sidebar.
  ** .pp
  ** Local mail is often arranged in directories: `dir1/dir2/mailbox'.
  ** .ts
  ** set sidebar_delim_chars='/'
  ** .te
  ** .pp
  ** IMAP mailboxes are often named: `folder1.folder2.mailbox'.
  ** .ts
  ** set sidebar_delim_chars='.'
  ** .te
  ** .pp
  ** \fBSee also:\fP $$sidebar_short_path, $$sidebar_folder_indent, $$sidebar_indent_string.
  */
  { "sidebar_divider_char", DT_STR, R_SIDEBAR, {.p=&SidebarDividerChar}, {.p="|"} },
  /*
  ** .pp
  ** This specifies the characters to be drawn between the sidebar (when
  ** visible) and the other Mutt panels. ASCII and Unicode line-drawing
  ** characters are supported.
  */
  { "sidebar_folder_indent", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARFOLDERINDENT}, {.l=0} },
  /*
  ** .pp
  ** Set this to indent mailboxes in the sidebar.
  ** .pp
  ** \fBSee also:\fP $$sidebar_short_path, $$sidebar_indent_string, $$sidebar_delim_chars.
  */
  { "sidebar_format", DT_STR, R_SIDEBAR, {.p=&SidebarFormat}, {.p="%B%*  %n"} },
  /*
  ** .pp
  ** This variable allows you to customize the sidebar display. This string is
  ** similar to $$index_format, but has its own set of \fCprintf(3)\fP-like
  ** sequences:
  ** .dl
  ** .dt %B  .dd Name of the mailbox
  ** .dt %S  .dd * Size of mailbox (total number of messages)
  ** .dt %N  .dd * Number of unread messages in the mailbox
  ** .dt %n  .dd N if mailbox has new mail, blank otherwise
  ** .dt %F  .dd * Number of Flagged messages in the mailbox
  ** .dt %!  .dd ``!'' : one flagged message;
  **             ``!!'' : two flagged messages;
  **             ``n!'' : n flagged messages (for n > 2).
  **             Otherwise prints nothing.
  ** .dt %d  .dd * @ Number of deleted messages
  ** .dt %L  .dd * @ Number of messages after limiting
  ** .dt %t  .dd * @ Number of tagged messages
  ** .dt %>X .dd right justify the rest of the string and pad with ``X''
  ** .dt %|X .dd pad to the end of the line with ``X''
  ** .dt %*X .dd soft-fill with character ``X'' as pad
  ** .de
  ** .pp
  ** * = Can be optionally printed if nonzero
  ** @ = Only applicable to the current folder
  ** .pp
  ** In order to use %S, %N, %F, and %!, $$mail_check_stats must
  ** be \fIset\fP.  When thus set, a suggested value for this option is
  ** "%B%?F? [%F]?%* %?N?%N/?%S".
  */
  { "sidebar_indent_string", DT_STR, R_SIDEBAR, {.p=&SidebarIndentString}, {.p="  "} },
  /*
  ** .pp
  ** This specifies the string that is used to indent mailboxes in the sidebar.
  ** It defaults to two spaces.
  ** .pp
  ** \fBSee also:\fP $$sidebar_short_path, $$sidebar_folder_indent, $$sidebar_delim_chars.
  */
  { "sidebar_new_mail_only", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARNEWMAILONLY}, {.l=0} },
  /*
  ** .pp
  ** When set, the sidebar will only display mailboxes containing new, or
  ** flagged, mail.
  ** .pp
  ** \fBSee also:\fP $sidebar_whitelist.
  */
  { "sidebar_next_new_wrap", DT_BOOL, R_NONE, {.l=OPTSIDEBARNEXTNEWWRAP}, {.l=0} },
  /*
  ** .pp
  ** When set, the \fC<sidebar-next-new>\fP command will not stop and the end of
  ** the list of mailboxes, but wrap around to the beginning. The
  ** \fC<sidebar-prev-new>\fP command is similarly affected, wrapping around to
  ** the end of the list.
  */
  { "sidebar_relative_shortpath_indent", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARRELSPINDENT}, {.l=0} },
  /*
  ** .pp
  ** When set, this option changes how $$sidebar_short_path and
  ** $$sidebar_folder_indent perform shortening and indentation: both
  ** will look at the previous sidebar entries and shorten/indent
  ** relative to the most recent parent.
  ** .pp
  ** An example of this option set/unset for mailboxes listed in this
  ** order, with $$sidebar_short_path=yes,
  ** $$sidebar_folder_indent=yes, and $$sidebar_indent_string="→":
  ** .dl
  ** .dt \fBmailbox\fP  .dd \fBset\fP   .dd \fBunset\fP
  ** .dt \fC=a.b\fP     .dd \fC=a.b\fP  .dd \fC→b\fP
  ** .dt \fC=a.b.c.d\fP .dd \fC→c.d\fP  .dd \fC→→→d\fP
  ** .dt \fC=a.b.e\fP   .dd \fC→e\fP    .dd \fC→→e\fP
  ** .de
  ** .pp
  ** The second line illustrates most clearly.  With this option set,
  ** \fC=a.b.c.d\fP is shortened relative to \fC=a.b\fP, becoming
  ** \fCc.d\fP; it is also indented one place relative to \fC=a.b\fP.
  ** With this option unset \fC=a.b.c.d\fP is always shortened to the
  ** last part of the mailbox, \fCd\fP and is indented three places,
  ** with respect to $$folder (represented by '=').
  ** .pp
  ** When set, the third line will also be indented and shortened
  ** relative to the first line.
  */
  { "sidebar_short_path", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARSHORTPATH}, {.l=0} },
  /*
  ** .pp
  ** By default the sidebar will show the mailbox's path, relative to the
  ** $$folder variable. Setting \fCsidebar_shortpath=yes\fP will shorten the
  ** names relative to the previous name. Here's an example:
  ** .dl
  ** .dt \fBshortpath=no\fP .dd \fBshortpath=yes\fP .dd \fBshortpath=yes, folderindent=yes, indentstr=".."\fP
  ** .dt \fCfruit\fP        .dd \fCfruit\fP         .dd \fCfruit\fP
  ** .dt \fCfruit.apple\fP  .dd \fCapple\fP         .dd \fC..apple\fP
  ** .dt \fCfruit.banana\fP .dd \fCbanana\fP        .dd \fC..banana\fP
  ** .dt \fCfruit.cherry\fP .dd \fCcherry\fP        .dd \fC..cherry\fP
  ** .de
  ** .pp
  ** \fBSee also:\fP $$sidebar_delim_chars, $$sidebar_folder_indent, $$sidebar_indent_string.
  */
  { "sidebar_sort_method", DT_SORT|DT_SORT_SIDEBAR, R_SIDEBAR, {.p=&SidebarSortMethod}, {.l=SORT_ORDER} },
  /*
  ** .pp
  ** Specifies how to sort entries in the file browser.  By default, the
  ** entries are sorted alphabetically.  Valid values:
  ** .il
  ** .dd alpha (alphabetically)
  ** .dd count (all message count)
  ** .dd flagged (flagged message count)
  ** .dd name (alphabetically)
  ** .dd new (unread message count)
  ** .dd path (alphabetically)
  ** .dd unread (unread message count)
  ** .dd unsorted
  ** .ie
  ** .pp
  ** You may optionally use the ``reverse-'' prefix to specify reverse sorting
  ** order (example: ``\fCset sort_browser=reverse-date\fP'').
  */
  { "sidebar_use_mailbox_shortcuts", DT_BOOL, R_SIDEBAR, {.l=OPTSIDEBARUSEMBSHORTCUTS}, {.l=0} },
  /*
  ** .pp
  ** When set, sidebar mailboxes will be displayed with mailbox shortcut prefixes
  ** "=" or "~".
  ** .pp
  ** When unset, the sidebar will trim off a matching $$folder prefix
  ** but otherwise not use mailbox shortcuts.
  */
  { "sidebar_visible", DT_BOOL, R_REFLOW, {.l=OPTSIDEBAR}, {.l=0} },
  /*
  ** .pp
  ** This specifies whether or not to show sidebar. The sidebar shows a list of
  ** all your mailboxes.
  ** .pp
  ** \fBSee also:\fP $$sidebar_format, $$sidebar_width
  */
  { "sidebar_width", DT_NUM, R_REFLOW, {.p=&SidebarWidth}, {.l=30} },
  /*
  ** .pp
  ** This controls the width of the sidebar.  It is measured in screen columns.
  ** For example: sidebar_width=20 could display 20 ASCII characters, or 10
  ** Chinese characters.
  */
#endif
  { "sig_dashes",	DT_BOOL, R_NONE, {.l=OPTSIGDASHES}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, a line containing ``-- '' (note the trailing space) will be inserted before your
  ** $$signature.  It is \fBstrongly\fP recommended that you not \fIunset\fP
  ** this variable unless your signature contains just your name.  The
  ** reason for this is because many software packages use ``-- \n'' to
  ** detect your signature.  For example, Mutt has the ability to highlight
  ** the signature in a different color in the built-in pager.
  */
  { "sig_on_top",	DT_BOOL, R_NONE, {.l=OPTSIGONTOP}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP, the signature will be included before any quoted or forwarded
  ** text.  It is \fBstrongly\fP recommended that you do not set this variable
  ** unless you really know what you are doing, and are prepared to take
  ** some heat from netiquette guardians.
  */
  { "signature",	DT_PATH, R_NONE, {.p=&Signature}, {.p="~/.signature"} },
  /*
  ** .pp
  ** Specifies the filename of your signature, which is appended to all
  ** outgoing messages.   If the filename ends with a pipe (``|''), it is
  ** assumed that filename is a shell command and input should be read from
  ** its standard output.
  */
  { "simple_search",	DT_STR,	 R_NONE, {.p=&SimpleSearch}, {.p="~f %s | ~s %s"} },
  /*
  ** .pp
  ** Specifies how Mutt should expand a simple search into a real search
  ** pattern.  A simple search is one that does not contain any of the ``~'' pattern
  ** operators.  See ``$patterns'' for more information on search patterns.
  ** .pp
  ** For example, if you simply type ``joe'' at a search or limit prompt, Mutt
  ** will automatically expand it to the value specified by this variable by
  ** replacing ``%s'' with the supplied string.
  ** For the default value, ``joe'' would be expanded to: ``~f joe | ~s joe''.
  */
  { "size_show_bytes",	DT_BOOL, R_MENU, {.l=OPTSIZESHOWBYTES}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP, message sizes will display bytes for values less than
  ** 1 kilobyte.  See $formatstrings-size.
  */
  { "size_show_fractions", DT_BOOL, R_MENU, {.l=OPTSIZESHOWFRACTIONS}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, message sizes will be displayed with a single decimal value
  ** for sizes from 0 to 10 kilobytes and 1 to 10 megabytes.
  ** See $formatstrings-size.
  */
  { "size_show_mb",	DT_BOOL, R_MENU, {.l=OPTSIZESHOWMB}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP, message sizes will display megabytes for values greater than
  ** or equal to 1 megabyte.  See $formatstrings-size.
  */
  { "size_units_on_left", DT_BOOL, R_MENU, {.l=OPTSIZEUNITSONLEFT}, {.l=0} },
  /*
  ** .pp
  ** If \fIset\fP, message sizes units will be displayed to the left of the number.
  ** See $formatstrings-size.
  */
  { "sleep_time",	DT_NUM, R_NONE, {.p=&SleepTime}, {.l=1} },
  /*
  ** .pp
  ** Specifies time, in seconds, to pause while displaying certain informational
  ** messages, while moving from folder to folder and after expunging
  ** messages from the current folder.  The default is to pause one second, so
  ** a value of zero for this option suppresses the pause.
  */
  { "smart_wrap",	DT_BOOL, R_PAGER_FLOW, {.l=OPTWRAP}, {.l=1} },
  /*
  ** .pp
  ** Controls the display of lines longer than the screen width in the
  ** internal pager. If \fIset\fP, long lines are wrapped at a word boundary.  If
  ** \fIunset\fP, lines are simply wrapped at the screen edge. Also see the
  ** $$markers variable.
  */
  { "smileys",		DT_RX,	 R_PAGER, {.p=&Smileys}, {.p="(>From )|(:[-^]?[][)(><}{|/DP])"} },
  /*
  ** .pp
  ** The \fIpager\fP uses this variable to catch some common false
  ** positives of $$quote_regexp, most notably smileys and not consider
  ** a line quoted text if it also matches $$smileys. This mostly
  ** happens at the beginning of a line.
  */
  { "pgp_mime_signature_filename", DT_STR, R_NONE, {.p=&PgpMimeSignatureFilename}, {.p="signature.asc"} },
  /*
  ** .pp
  ** This option sets the filename used for signature parts in PGP/MIME
  ** signed messages.
  */
  { "pgp_mime_signature_description", DT_STR, R_NONE, {.p=&PgpMimeSignatureDescription}, {.p="Digital signature"} },
  /*
  ** .pp
  ** This option sets the Content-Description used for signature parts in
  ** PGP/MIME signed messages.
  */
  { "smime_ask_cert_label",	DT_BOOL, R_NONE, {.l=OPTASKCERTLABEL}, {.l=1} },
  /*
  ** .pp
  ** This flag controls whether you want to be asked to enter a label
  ** for a certificate about to be added to the database or not. It is
  ** \fIset\fP by default.
  ** (S/MIME only)
  */
  { "smime_ca_location",	DT_PATH, R_NONE, {.p=&SmimeCALocation}, {.p=0} },
  /*
  ** .pp
  ** This variable contains the name of either a directory, or a file which
  ** contains trusted certificates for use with OpenSSL.
  ** (S/MIME only)
  */
  { "smime_certificates",	DT_PATH, R_NONE, {.p=&SmimeCertificates}, {.p=0} },
  /*
  ** .pp
  ** Since for S/MIME there is no pubring/secring as with PGP, mutt has to handle
  ** storage and retrieval of keys by itself. This is very basic right
  ** now, and keys and certificates are stored in two different
  ** directories, both named as the hash-value retrieved from
  ** OpenSSL. There is an index file which contains mailbox-address
  ** keyid pairs, and which can be manually edited. This option points to
  ** the location of the certificates.
  ** (S/MIME only)
  */
  { "smime_decrypt_command", 	DT_STR, R_NONE, {.p=&SmimeDecryptCommand}, {.p=0} },
  /*
  ** .pp
  ** This format string specifies a command which is used to decrypt
  ** \fCapplication/x-pkcs7-mime\fP attachments.
  ** .pp
  ** The OpenSSL command formats have their own set of \fCprintf(3)\fP-like sequences
  ** similar to PGP's:
  ** .dl
  ** .dt %f .dd Expands to the name of a file containing a message.
  ** .dt %s .dd Expands to the name of a file containing the signature part
  ** .          of a \fCmultipart/signed\fP attachment when verifying it.
  ** .dt %k .dd The key-pair specified with $$smime_default_key
  ** .dt %c .dd One or more certificate IDs.
  ** .dt %a .dd The algorithm used for encryption.
  ** .dt %d .dd The message digest algorithm specified with $$smime_sign_digest_alg.
  ** .dt %C .dd CA location:  Depending on whether $$smime_ca_location
  ** .          points to a directory or file, this expands to
  ** .          ``-CApath $$smime_ca_location'' or ``-CAfile $$smime_ca_location''.
  ** .de
  ** .pp
  ** For examples on how to configure these formats, see the \fCsmime.rc\fP in
  ** the \fCsamples/\fP subdirectory which has been installed on your system
  ** alongside the documentation.
  ** (S/MIME only)
  */
  { "smime_decrypt_use_default_key",	DT_BOOL, R_NONE, {.l=OPTSDEFAULTDECRYPTKEY}, {.l=1} },
  /*
  ** .pp
  ** If \fIset\fP (default) this tells mutt to use the default key for decryption. Otherwise,
  ** if managing multiple certificate-key-pairs, mutt will try to use the mailbox-address
  ** to determine the key to use. It will ask you to supply a key, if it can't find one.
  ** (S/MIME only)
  */
  { "smime_self_encrypt_as",	DT_SYN,  R_NONE, {.p="smime_default_key"}, {.p=0} },
  { "smime_default_key",		DT_STR,	 R_NONE, {.p=&SmimeDefaultKey}, {.p=0} },
  /*
  ** .pp
  ** This is the default key-pair to use for S/MIME operations, and must be
  ** set to the keyid (the hash-value that OpenSSL generates) to work properly.
  ** .pp
  ** It will be used for encryption (see $$postpone_encrypt and
  ** $$smime_self_encrypt).
  ** .pp
  ** It will be used for decryption unless $$smime_decrypt_use_default_key
  ** is \fIunset\fP.
  ** .pp
  ** It will also be used for signing unless $$smime_sign_as is set.
  ** .pp
  ** The (now deprecated) \fIsmime_self_encrypt_as\fP is an alias for this
  ** variable, and should no longer be used.
  ** (S/MIME only)
  */
  { "smime_encrypt_command", 	DT_STR, R_NONE, {.p=&SmimeEncryptCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to create encrypted S/MIME messages.
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
  { "smime_encrypt_with",	DT_STR,	 R_NONE, {.p=&SmimeCryptAlg}, {.p="aes256"} },
  /*
  ** .pp
  ** This sets the algorithm that should be used for encryption.
  ** Valid choices are ``aes128'', ``aes192'', ``aes256'', ``des'', ``des3'', ``rc2-40'', ``rc2-64'', ``rc2-128''.
  ** (S/MIME only)
  */
  { "smime_get_cert_command", 	DT_STR, R_NONE, {.p=&SmimeGetCertCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to extract X509 certificates from a PKCS7 structure.
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
  { "smime_get_cert_email_command", 	DT_STR, R_NONE, {.p=&SmimeGetCertEmailCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to extract the mail address(es) used for storing
  ** X509 certificates, and for verification purposes (to check whether the
  ** certificate was issued for the sender's mailbox).
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
  { "smime_get_signer_cert_command", 	DT_STR, R_NONE, {.p=&SmimeGetSignerCertCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to extract only the signers X509 certificate from a S/MIME
  ** signature, so that the certificate's owner may get compared to the
  ** email's ``From:'' field.
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
  { "smime_import_cert_command", 	DT_STR, R_NONE, {.p=&SmimeImportCertCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to import a certificate via smime_keys.
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
  { "smime_is_default", DT_BOOL,  R_NONE, {.l=OPTSMIMEISDEFAULT}, {.l=0} },
  /*
  ** .pp
  ** The default behavior of mutt is to use PGP on all auto-sign/encryption
  ** operations. To override and to use OpenSSL instead this must be \fIset\fP.
  ** However, this has no effect while replying, since mutt will automatically
  ** select the same application that was used to sign/encrypt the original
  ** message.  (Note that this variable can be overridden by unsetting $$crypt_autosmime.)
  ** (S/MIME only)
  */
  { "smime_keys",		DT_PATH, R_NONE, {.p=&SmimeKeys}, {.p=0} },
  /*
  ** .pp
  ** Since for S/MIME there is no pubring/secring as with PGP, mutt has to handle
  ** storage and retrieval of keys/certs by itself. This is very basic right now,
  ** and stores keys and certificates in two different directories, both
  ** named as the hash-value retrieved from OpenSSL. There is an index file
  ** which contains mailbox-address keyid pair, and which can be manually
  ** edited. This option points to the location of the private keys.
  ** (S/MIME only)
  */
  { "smime_pk7out_command", 	DT_STR, R_NONE, {.p=&SmimePk7outCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to extract PKCS7 structures of S/MIME signatures,
  ** in order to extract the public X509 certificate(s).
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
  { "smime_self_encrypt",    DT_BOOL, R_NONE, {.l=OPTSMIMESELFENCRYPT}, {.l=1} },
  /*
  ** .pp
  ** When \fIset\fP, S/MIME encrypted messages will also be encrypted
  ** using the certificate in $$smime_default_key.
  ** (S/MIME only)
  */
  { "smime_sign_as",	DT_STR,	 R_NONE, {.p=&SmimeSignAs}, {.p=0} },
  /*
  ** .pp
  ** If you have a separate key to use for signing, you should set this
  ** to the signing key. Most people will only need to set $$smime_default_key.
  ** (S/MIME only)
  */
  { "smime_sign_command", 	DT_STR, R_NONE, {.p=&SmimeSignCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to created S/MIME signatures of type
  ** \fCmultipart/signed\fP, which can be read by all mail clients.
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.  NOTE: %c and %k will default
  ** to $$smime_sign_as if set, otherwise $$smime_default_key.
  ** (S/MIME only)
  */
  { "smime_sign_digest_alg",	DT_STR,	 R_NONE, {.p=&SmimeDigestAlg}, {.p="sha256"} },
  /*
  ** .pp
  ** This sets the algorithm that should be used for the signature message digest.
  ** Valid choices are ``md5'', ``sha1'', ``sha224'', ``sha256'', ``sha384'', ``sha512''.
  ** (S/MIME only)
  */
  { "smime_sign_opaque_command", 	DT_STR, R_NONE, {.p=&SmimeSignOpaqueCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to created S/MIME signatures of type
  ** \fCapplication/x-pkcs7-signature\fP, which can only be handled by mail
  ** clients supporting the S/MIME extension.
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
  { "smime_timeout",		DT_LNUM,	 R_NONE, {.p=&SmimeTimeout}, {.l=300} },
  /*
  ** .pp
  ** The number of seconds after which a cached passphrase will expire if
  ** not used.
  ** (S/MIME only)
  */
  { "smime_verify_command", 	DT_STR, R_NONE, {.p=&SmimeVerifyCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to verify S/MIME signatures of type \fCmultipart/signed\fP.
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
  { "smime_verify_opaque_command", 	DT_STR, R_NONE, {.p=&SmimeVerifyOpaqueCommand}, {.p=0} },
  /*
  ** .pp
  ** This command is used to verify S/MIME signatures of type
  ** \fCapplication/x-pkcs7-mime\fP.
  ** .pp
  ** This is a format string, see the $$smime_decrypt_command command for
  ** possible \fCprintf(3)\fP-like sequences.
  ** (S/MIME only)
  */
#ifdef USE_SMTP
  { "smtp_authenticators", DT_STR, R_NONE, {.p=&SmtpAuthenticators}, {.p=0} },
  /*
  ** .pp
  ** This is a colon-delimited list of authentication methods mutt may
  ** attempt to use to log in to an SMTP server, in the order mutt should
  ** try them.  Authentication methods are any SASL mechanism, e.g.
  ** ``digest-md5'', ``gssapi'' or ``cram-md5''.
  ** This option is case-insensitive. If it is ``unset''
  ** (the default) mutt will try all available methods, in order from
  ** most-secure to least-secure.
  ** .pp
  ** Example:
  ** .ts
  ** set smtp_authenticators="digest-md5:cram-md5"
  ** .te
  */
  { "smtp_oauth_refresh_command", DT_STR, R_NONE, {.p=&SmtpOauthRefreshCmd}, {.p=0} },
  /*
  ** .pp
  ** The command to run to generate an OAUTH refresh token for
  ** authorizing your connection to your SMTP server.  This command will be
  ** run on every connection attempt that uses the OAUTHBEARER authentication
  ** mechanism.  See ``$oauth'' for details.
  */
  { "smtp_pass", 	DT_STR,  R_NONE, {.p=&SmtpPass}, {.p=0} },
  /*
  ** .pp
  ** Specifies the password for your SMTP account.  If \fIunset\fP, Mutt will
  ** prompt you for your password when you first send mail via SMTP.
  ** See $$smtp_url to configure mutt to send mail via SMTP.
  ** .pp
  ** \fBWarning\fP: you should only use this option when you are on a
  ** fairly secure machine, because the superuser can read your muttrc even
  ** if you are the only one who can read the file.
  */
  { "smtp_url",		DT_STR, R_NONE, {.p=&SmtpUrl}, {.p=0} },
  /*
  ** .pp
  ** Defines the SMTP smarthost where sent messages should relayed for
  ** delivery. This should take the form of an SMTP URL, e.g.:
  ** .ts
  ** smtp[s]://[user[:pass]@]host[:port]
  ** .te
  ** .pp
  ** where ``[...]'' denotes an optional part.
  ** Setting this variable overrides the value of the $$sendmail
  ** variable.
  ** .pp
  ** Also see $$write_bcc.
  */
#endif /* USE_SMTP */
  { "sort",		DT_SORT, R_INDEX|R_RESORT, {.p=&Sort}, {.l=SORT_DATE} },
  /*
  ** .pp
  ** Specifies how to sort messages in the ``index'' menu.  Valid values
  ** are:
  ** .il
  ** .dd date or date-sent
  ** .dd date-received
  ** .dd from
  ** .dd mailbox-order (unsorted)
  ** .dd score
  ** .dd size
  ** .dd spam
  ** .dd subject
  ** .dd threads
  ** .dd to
  ** .ie
  ** .pp
  ** You may optionally use the ``reverse-'' prefix to specify reverse sorting
  ** order (example: ``\fCset sort=reverse-date-sent\fP'').
  */
  { "sort_alias",	DT_SORT|DT_SORT_ALIAS,	R_NONE,	{.p=&SortAlias}, {.l=SORT_ALIAS} },
  /*
  ** .pp
  ** Specifies how the entries in the ``alias'' menu are sorted.  The
  ** following are legal values:
  ** .il
  ** .dd address (sort alphabetically by email address)
  ** .dd alias (sort alphabetically by alias name)
  ** .dd unsorted (leave in order specified in .muttrc)
  ** .ie
  */
  { "sort_aux",		DT_SORT|DT_SORT_AUX, R_INDEX|R_RESORT_BOTH, {.p=&SortAux}, {.l=SORT_DATE} },
  /*
  ** .pp
  ** This provides a secondary sort for messages in the ``index'' menu, used
  ** when the $$sort value is equal for two messages.
  ** .pp
  ** When sorting by threads, this variable controls how threads are sorted
  ** in relation to other threads, and how the branches of the thread trees
  ** are sorted.  This can be set to any value that $$sort can, except
  ** ``threads'' (in that case, mutt will just use ``date-sent'').  You can also
  ** specify the ``last-'' prefix in addition to the ``reverse-'' prefix, but ``last-''
  ** must come after ``reverse-''.  The ``last-'' prefix causes messages to be
  ** sorted against its siblings by which has the last descendant, using
  ** the rest of $$sort_aux as an ordering.  For instance,
  ** .ts
  ** set sort_aux=last-date-received
  ** .te
  ** .pp
  ** would mean that if a new message is received in a
  ** thread, that thread becomes the last one displayed (or the first, if
  ** you have ``\fCset sort=reverse-threads\fP''.)
  ** .pp
  ** Note: For reversed-threads $$sort
  ** order, $$sort_aux is reversed again (which is not the right thing to do,
  ** but kept to not break any existing configuration setting).
  */
  { "sort_browser",	DT_SORT|DT_SORT_BROWSER, R_NONE, {.p=&BrowserSort}, {.l=SORT_ALPHA} },
  /*
  ** .pp
  ** Specifies how to sort entries in the file browser.  By default, the
  ** entries are sorted alphabetically.  Valid values:
  ** .il
  ** .dd alpha (alphabetically)
  ** .dd count
  ** .dd date
  ** .dd size
  ** .dd unread
  ** .dd unsorted
  ** .ie
  ** .pp
  ** You may optionally use the ``reverse-'' prefix to specify reverse sorting
  ** order (example: ``\fCset sort_browser=reverse-date\fP'').
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
