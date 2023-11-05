<?php
/*
 * PukiWiki ls�ץ饰����
 *
 * CopyRight 2002 Y.MASUI GPL2
 * http://masui.net/pukiwiki/ masui@masui.net
 *
 * $Id: ls.inc.php,v 1.8 2003/06/10 14:02:16 arino Exp $
 */

function plugin_ls_convert()
{
	global $vars;
	
	$with_title = FALSE;
	
	if (func_num_args())
	{
		$args = func_get_args();
		$with_title = in_array('title',$args);
	}
	
	$prefix = $vars['page'].'/';
	
	$pages = array();
	foreach (get_existpages() as $page)
	{
		if (strpos($page,$prefix) === 0)
		{
			$pages[] = $page;
		}
	}
	natcasesort($pages);
	
	$ls = array();
	foreach ($pages as $page)
	{
		$comment = '';
		if ($with_title)
		{
			list($comment) = get_source($page);
			// ���Ф��θ�ͭID������
			$comment = preg_replace('/^(\*{1,3}.*)\[#[A-Za-z][\w-]+\](.*)$/','$1$2',$comment);
			
			$comment = '- ' . ereg_replace('^[-*]+','',$comment);
		}
		$ls[] = "-[[$page]] $comment";
	}
	
	return convert_html($ls);
}
?>
