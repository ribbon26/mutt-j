<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: list.inc.php,v 1.4 2004/07/31 03:09:20 henoheno Exp $
//
// ������ɽ��
function plugin_list_action()
{
	global $vars,$_title_list,$_title_filelist,$whatsnew;

	$filelist = (array_key_exists('cmd',$vars) and $vars['cmd']=='filelist'); //��©����

	return array(
		'msg'=>$filelist ? $_title_filelist : $_title_list,
		'body'=>get_list($filelist)
	);
}

// �����μ���
function get_list($withfilename)
{
	global $non_list,$whatsnew;

	$pages = array_diff(get_existpages(),array($whatsnew));
	if (!$withfilename)
	{
		$pages = array_diff($pages,preg_grep("/$non_list/",$pages));
	}
	if (count($pages) == 0)
	{
	        return '';
	}

	return page_list($pages,'read',$withfilename);
}
?>
