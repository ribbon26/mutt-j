<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: interwiki.inc.php,v 1.6 2003/07/14 04:41:10 arino Exp $
//
// InterWikiName��Ƚ�̤ȥڡ�����ɽ��

function plugin_interwiki_action()
{
	global $vars,$InterWikiName;
	global $_title_invalidiwn,$_msg_invalidiwn;
	
	$retvars = array();
	
	if (!preg_match("/^$InterWikiName$/",$vars['page'],$match))
	{
		$retvars['msg'] = $_title_invalidiwn;
		$retvars['body'] = str_replace(
			array('$1','$2'),
			array(htmlspecialchars($name),make_pagelink('InterWikiName')),
			$_msg_invalidiwn
		);
		return $retvars;
	}
	$name = $match[2];
	$param = $match[3];
	
	$url = get_interwiki_url($name,$param);
	if ($url === FALSE)
	{
		$retvars['msg'] = $_title_invalidiwn;
		$retvars['body'] = str_replace(
			array('$1','$2'),
			array(htmlspecialchars($name),make_pagelink('InterWikiName')),
			$_msg_invalidiwn
		);
		return $retvars;
	}
	
	header("Location: $url");
	die();
}
?>
