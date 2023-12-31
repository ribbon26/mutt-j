<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: pukiwiki.ini.php,v 1.55 2004/04/03 15:56:08 arino Exp $
//
// PukiWiki setting file

/////////////////////////////////////////////////
// ディレクトリ指定 最後に / が必要 属性は 777
/////////////////////////////////////////////////
// データの格納ディレクトリ
define('DATA_DIR','./wiki/');
/////////////////////////////////////////////////
// 差分ファイルの格納ディレクトリ
define('DIFF_DIR','./diff/');
/////////////////////////////////////////////////
// バックアップファイル格納先ディレクトリ
define('BACKUP_DIR','./backup/');
/////////////////////////////////////////////////
// プラグインファイル格納先ディレクトリ
define('PLUGIN_DIR','./plugin/');
/////////////////////////////////////////////////
// キャッシュファイル格納ディレクトリ
define('CACHE_DIR','./cache/');
/////////////////////////////////////////////////
// 添付ファイル格納ディレクトリ
define('UPLOAD_DIR','./attach/');
/////////////////////////////////////////////////
// カウンタファイル格納ディレクトリ
define('COUNTER_DIR', './counter/');
/////////////////////////////////////////////////
// TrackBackファイル格納ディレクトリ
define('TRACKBACK_DIR','./trackback/');
/////////////////////////////////////////////////
// スキン/スタイルシートファイル格納ディレクトリ
define('SKIN_DIR','./skin/');

/////////////////////////////////////////////////
// ローカル時間
define('ZONE','JST');
define('ZONETIME',9 * 3600); // JST = GMT+9
/////////////////////////////////////////////////
// index.php などに変更した場合のスクリプト名の設定
// とくに設定しなくても問題なし
$script = '/pukiwiki/';

/////////////////////////////////////////////////
// トップページの名前
$defaultpage = 'FrontPage';
/////////////////////////////////////////////////
// 更新履歴ページの名前
$whatsnew = 'RecentChanges';
/////////////////////////////////////////////////
// 削除履歴ページの名前
$whatsdeleted = 'RecentDeleted';
/////////////////////////////////////////////////
// InterWikiNameページの名前
$interwiki = 'InterWikiName';
/////////////////////////////////////////////////
// MenuBarページの名前
$menubar = 'MenuBar';
/////////////////////////////////////////////////
// 編集者の名前(自由に変えてください)
$modifier = 'mutt-j';
/////////////////////////////////////////////////
// 編集者のホームページ(自由に変えてください)
$modifierlink = 'http://mutt-j.sourceforge.jp/';

/////////////////////////////////////////////////
// ホームページのタイトル(自由に変えてください)
// RSS に出力するチャンネル名
$page_title = 'mutt-j wiki';

/////////////////////////////////////////////////
// TrackBack機能を使用する
$trackback = 0;

/////////////////////////////////////////////////
// Referer機能を使用する
$referer = 1;

/////////////////////////////////////////////////
// WikiNameを*無効に*する
$nowikiname = 0;

/////////////////////////////////////////////////
// AutoLinkを有効にする場合は、AutoLink対象となる
// ページ名の最短バイト数を指定
// AutoLinkを無効にする場合は0
$autolink = 0;

/////////////////////////////////////////////////
// 凍結機能を有効にする
$function_freeze = 1;
/////////////////////////////////////////////////
// 凍結解除用の管理者パスワード(MD5)
// pukiwiki.php?md5=pass のようにURLに入力し
// MD5にしてからどうぞ。面倒なら以下のように。
// $adminpass = md5("pass");
// 以下は pass のMD5パスワードになってます。
$adminpass = '892c5945c37de51a31a64cc828aa41d2';

/////////////////////////////////////////////////
// ChaSen, KAKASI による、ページ名の読みの取得 (0:無効,1:有効)
$pagereading_enable = 0;
// ChaSen or KAKASI or none
//$pagereading_kanji2kana_converter = 'chasen';
//$pagereading_kanji2kana_converter = 'kakasi';
$pagereading_kanji2kana_converter = 'none';
// ChaSen/KAKASI との受け渡しに使う漢字コード (UNIX系は EUC、Win系は SJIS が基本)
$pagereading_kanji2kana_encoding = 'EUC';
//$pagereading_kanji2kana_encoding = 'SJIS';
// ChaSen/KAKASI の実行ファイル (各自の環境に合わせて設定)
$pagereading_chasen_path = '/usr/local/bin/chasen';
//$pagereading_chasen_path = 'c:\progra~1\chasen21\chasen.exe';
$pagereading_kakasi_path = '/usr/local/bin/kakasi';
//$pagereading_kakasi_path = 'c:\kakasi\bin\kakasi.exe';
// ページ名読みを格納したページの名前
$pagereading_config_page = ':config/PageReading';
// converter = none の場合の読み仮名辞書
$pagereading_config_dict = ':config/PageReading/dict';

/////////////////////////////////////////////////
// ユーザ定義
$auth_users = array(
'tamomo' => 'foo_passwd',
't-tkzw' => 'bar_passwd',
'ribbon' => 'hoge_passwd',
);
/////////////////////////////////////////////////
// 認証方式種別
// pagename : ページ名
// contents : ページ内容
$auth_method_type = "contents";
/////////////////////////////////////////////////
// 閲覧認証フラグ
// 0:不要
// 1:必要
$read_auth = 0;
/////////////////////////////////////////////////
// 閲覧認証対象パターン定義
$read_auth_pages = array(
'/ひきこもるほげ/' => 'hoge',
'/(ネタバレ|ねたばれ)/' => 'foo,bar,hoge',
);
/////////////////////////////////////////////////
// 編集認証フラグ
// 0:不要
// 1:必要
$edit_auth = 0;
/////////////////////////////////////////////////
// 編集認証対象パターン定義
$edit_auth_pages = array(
'/Barの公開日記/' => 'bar',
'/ひきこもるほげ/' => 'hoge',
'/(ネタバレ|ねたばれ)/' => 'foo',
);
/////////////////////////////////////////////////
// 検索認証フラグ
// 0: 閲覧が許可されていないページ内容も検索対象とする
// 1: 検索時のログインユーザに許可されたページのみ検索対象とする
$search_auth = 0;

/////////////////////////////////////////////////
// 更新履歴を表示するときの最大件数
$maxshow = 80;
/////////////////////////////////////////////////
// 削除履歴の最大件数(0で記録しない)
$maxshow_deleted = 0;
/////////////////////////////////////////////////
// 編集することのできないページの名前 , で区切る
$cantedit = array( $whatsnew,$whatsdeleted );

/////////////////////////////////////////////////
// Last-Modified ヘッダを出力する
$lastmod = 0;

/////////////////////////////////////////////////
// 日付フォーマット
$date_format = 'Y-m-d';
/////////////////////////////////////////////////
// 時刻フォーマット
$time_format = 'H:i:s';
/////////////////////////////////////////////////
// 曜日配列
$weeklabels = $_msg_week;

/////////////////////////////////////////////////
// RSS に出力するページ数
$rss_max = 15;

/////////////////////////////////////////////////
// バックアップを行う
$do_backup = 1;
/////////////////////////////////////////////////
// ページを削除した際にバックアップもすべて削除する
$del_backup = 0;
/////////////////////////////////////////////////
// 定期バックアップの間隔を時間(hour)で指定します(0で更新毎)
$cycle = 6;
/////////////////////////////////////////////////
// バックアップの最大世代数
$maxage = 20;
/////////////////////////////////////////////////
// バックアップの世代を区切る文字列
// (通常はこのままで良いが、文章中で使われる可能性
// があれば、使われそうにない文字を設定する)
$splitter = ">>>>>>>>>>";
/////////////////////////////////////////////////
// ページの更新時にバックグランドで実行されるコマンド(mknmzなど)
$update_exec = '';
//$update_exec = '/usr/bin/mknmz --media-type=text/pukiwiki -O /var/lib/namazu/index/ -L ja -c -K /var/www/wiki/';

/////////////////////////////////////////////////
// HTTPリクエストにプロキシサーバを使用する
$use_proxy = 0;
// proxy ホスト
$proxy_host = 'proxy.xxx.yyy.zzz';
// proxy ポート番号
$proxy_port = 8080;
// proxyのBasic認証が必要な場合に1
$need_proxy_auth = 0;
// proxyのBasic認証用ID,PW
$proxy_auth_user = 'foo';
$proxy_auth_pass = 'foo_password';
// プロキシサーバを使用しないホストのリスト
$no_proxy = array(
'localhost',        // localhost 
'127.0.0.0/8',      // loopback
// '10.0.0.0/8'     // private class A 
// '172.16.0.0/12'  // private class B 
// '192.168.0.0/16' // private class C
//'no-proxy.com',
);

////////////////////////////////////////////////
// ページの更新時にメールを送信する
$notify = 1;
// 差分だけを送信する
$notify_diff_only = 1;
// To:（宛先）
$notify_to = 'tamo-wiki@mutt.no-ip.org';
// From:（送り主）
$notify_from = 'tamomo@users.sourceforge.jp';
// Subject:（件名） $pageにページ名が入る
$notify_subject = '$page';
// 追加ヘッダ
$notify_header = "From: $notify_from\r\nX-Mailer: PukiWiki/".S_VERSION." PHP/".phpversion();
// POP Before SMTP を実施
$smtp_auth = 0;
// SMTPサーバ名を指定する (Windows のみ, 通常は php.ini で指定)
$smtp_server = 'localhost';
// POPサーバ名を指定する
$pop_server = 'localhost';
// POP のポート番号 (通常 110)
$pop_port = 110;
// 認証に APOP を利用するかどうか (APOP 利用時は 1、以外は POP3)
$pop_auth_use_apop = 0;
// POP ユーザ名
$pop_userid = '';
// POP パスワード
$pop_passwd = '';

/////////////////////////////////////////////////
// 一覧・更新一覧に含めないページ名(正規表現で)
$non_list = '^\:';

/////////////////////////////////////////////////
// $non_listを文字列検索の対象ページとするか
// 0にすると、上記ページ名が単語検索からも除外されます。
$search_non_list = 1;

/////////////////////////////////////////////////
// ページ名に従って自動で、雛形とするページの読み込み
$auto_template_func = 1;
$auto_template_rules = array(
'((.+)\/([^\/]+))' => '\2/template'
);

/////////////////////////////////////////////////
// 見出し行に固有のアンカーを自動挿入する
$fixed_heading_anchor = 0;
/////////////////////////////////////////////////
// <pre>の行頭スペースをひとつ取り除く
$preformat_ltrim = 0;

/////////////////////////////////////////////////
// 改行を反映する(改行を<br />に置換する)
$line_break = 0;

/////////////////////////////////////////////////
// ユーザーエージェント対応設定
// デフォルト
$user_agent = array('name'=>'default');
// 携帯端末
$agents = array(
	array('name'=>'jphone','pattern'=>'#^J-PHONE.+(Profile/)#'),
	array('name'=>'jphone','pattern'=>'#^J-PHONE#'),
	array('name'=>'i_mode','pattern'=>'#DoCoMo/(1\.0)/(?:[^/]+/c([0-9]+))?#'),
	array('name'=>'i_mode','pattern'=>'#DoCoMo/(2\.0) [^(]+\(c([0-9]+)#'),
	array('name'=>'i_mode','pattern'=>'#DDIPOCKET;JRC/[^/]+/(1\.0)/0100/c([0-9]+)#'),
);
?>
