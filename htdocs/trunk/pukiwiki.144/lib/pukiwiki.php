<?php
// PukiWiki - Yet another WikiWikiWeb clone.
//
// PukiWiki 1.4.*
//  Copyright (C) 2002 by PukiWiki Developers Team
//  http://pukiwiki.org/
//
// PukiWiki 1.3.*
//  Copyright (C) 2002 by PukiWiki Developers Team
//  http://pukiwiki.org/
//
// PukiWiki 1.3 (Base)
//  Copyright (C) 2001,2002 by sng.
//  <sng@factage.com>
//  http://factage.com/sng/pukiwiki/
//
// Special thanks
//  YukiWiki by Hiroshi Yuki
//  <hyuki@hyuki.com>
//  http://www.hyuki.com/yukiwiki/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Id: pukiwiki.php,v 1.3 2004/08/05 14:46:28 henoheno Exp $
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// データを格納するディレクトリや設定ファイルを置くディレクトリ

if (! defined('DATA_HOME')) define('DATA_HOME', '');

/////////////////////////////////////////////////
// サブルーチンの格納先ディレクトリ (他の *.phpファイル)

if (! defined('LIB_DIR')) define('LIB_DIR', '');

/////////////////////////////////////////////////
// サブルーチンの読み込み

require(LIB_DIR . 'func.php');
require(LIB_DIR . 'file.php');
require(LIB_DIR . 'plugin.php');
require(LIB_DIR . 'html.php');
require(LIB_DIR . 'backup.php');

require(LIB_DIR . 'convert_html.php');
require(LIB_DIR . 'make_link.php');
require(LIB_DIR . 'diff.php');
require(LIB_DIR . 'config.php');
require(LIB_DIR . 'link.php');
require(LIB_DIR . 'trackback.php');
require(LIB_DIR . 'auth.php');
require(LIB_DIR . 'proxy.php');
require(LIB_DIR . 'mail.php');
if (!extension_loaded('mbstring')) {
	require(LIB_DIR . 'mbstring.php');
}

// 初期化: 設定ファイルの読み込み
require(LIB_DIR . 'init.php');

/////////////////////////////////////////////////
// メイン処理

$base = $defaultpage;
$retvars = array();

// Plug-in action
if (!empty($vars['plugin'])) {
	if (!exist_plugin_action($vars['plugin'])) {
		$s_plugin = htmlspecialchars($vars['plugin']);
		$msg = "plugin=$s_plugin is not implemented.";
		$retvars = array('msg'=>$msg,'body'=>$msg);
	}
	else {
		$retvars = do_plugin_action($vars['plugin']);
		if ($retvars !== FALSE) {
			$base = array_key_exists('refer',$vars) ? $vars['refer'] : '';
		}
	}
}
// Command action
else if (!empty($vars['cmd'])) {
	if (!exist_plugin_action($vars['cmd'])) {
		$s_cmd = htmlspecialchars($vars['cmd']);
		$msg = "cmd=$s_cmd is not implemented.";
		$retvars = array('msg'=>$msg,'body'=>$msg);
	}
	else {
		$retvars = do_plugin_action($vars['cmd']);
		$base = $vars['page'];
	}
}

if ($retvars !== FALSE) {
	$title = htmlspecialchars(strip_bracket($base));
	$page = make_search($base);

	if (array_key_exists('msg',$retvars) and $retvars['msg'] != '') {
		$title = str_replace('$1',$title,$retvars['msg']);
		$page = str_replace('$1',$page,$retvars['msg']);
	}

	if (array_key_exists('body',$retvars) and $retvars['body'] != '') {
		$body = $retvars['body'];
	}
	else {
		if ($base == '' or !is_page($base)) {
			$base = $defaultpage;
			$title = htmlspecialchars(strip_bracket($base));
			$page = make_search($base);
		}

		$vars['cmd'] = 'read';
		$vars['page'] = $base;
		$body = convert_html(get_source($base));
		$body .= tb_get_rdf($vars['page']);
		ref_save($vars['page']);
	}

	// ** 出力処理 **
	catbody($title,$page,$body);
}
// ** 終了 **
?>
