<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: read.inc.php,v 1.4 2003/07/05 01:31:54 arino Exp $
//
// �ڡ�����ɽ����InterWikiName�β��
function plugin_read_action()
{
	global $get,$post,$vars;
	global $_title_edit,$_title_invalidwn,$_msg_invalidiwn;
	
	// WikiName��BracketName�������ڡ�����ɽ��
	if (is_page($get['page'])) {
		check_readable($get['page'],true,true);
		header_lastmod($get['page']);
		return array('msg'=>'','body'=>'');
	}

	// InterWikiName�����
	if (is_interwiki($get['page']))
		return do_plugin_action('interwiki');

	// �ڡ���̾�Ȥ���ͭ�������ڡ�����¸�ߤ��ʤ��Τǡ��Խ��ե������ɽ��
	if (is_pagename($get['page'])) {
		$get['cmd'] = $post['cmd'] = $vars['cmd'] = 'edit';
		return do_plugin_action('edit');
	}
	// ̵���ʥڡ���̾
	return array(
		'msg'=>$_title_invalidwn,
		'body'=>str_replace('$1',htmlspecialchars($get['page']),
			str_replace('$2','WikiName',$_msg_invalidiwn))
	);
}
?>
