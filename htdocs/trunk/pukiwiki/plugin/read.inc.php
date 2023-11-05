<?php
// PukiWiki - Yet another WikiWikiWeb clone.
// $Id: read.inc.php,v 1.8 2005/01/15 13:57:07 henoheno Exp $
//
// Read plugin: Show a page and InterWiki

function plugin_read_action()
{
	global $vars, $_title_invalidwn, $_msg_invalidiwn;

	$page = isset($vars['page']) ? $vars['page'] : '';

	if (is_page($page)) {
		// �ڡ�����ɽ��
		check_readable($page, true, true);
		header_lastmod($page);
		return array('msg'=>'', 'body'=>'');

	} else if (! PKWK_SAFE_MODE && is_interwiki($page)) {
		return do_plugin_action('interwiki'); // InterWikiName�����

	} else if (is_pagename($page)) {
		$vars['cmd'] = 'edit';
		return do_plugin_action('edit'); // ¸�ߤ��ʤ��Τǡ��Խ��ե������ɽ��

	} else {
		// ̵���ʥڡ���̾
		return array(
			'msg'=>$_title_invalidwn,
			'body'=>str_replace('$1', htmlspecialchars($page),
				str_replace('$2', 'WikiName', $_msg_invalidiwn))
		);
	}
}
?>
