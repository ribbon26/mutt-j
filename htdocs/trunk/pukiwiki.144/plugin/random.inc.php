<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: random.inc.php,v 1.7 2004/08/06 16:13:18 henoheno Exp $
//

/*
 *�ץ饰���� random
  �۲��Υڡ�����������ɽ������

 *Usage
  #random(��å�����)

 *�ѥ�᡼��
 -��å�����~
 ��󥯤�ɽ������ʸ����

 */
function plugin_random_convert()
{
	global $script, $vars;

	$title = 'press here.';

	if (func_num_args())
	{
		$args = func_get_args();
		$title = htmlspecialchars($args[0]);
	}
	return "<p><a href=\"$script?plugin=random&amp;refer={$vars['page']}\">$title</a></p>";
}

function plugin_random_action()
{
	global $script, $vars;

	$pattern = strip_bracket($vars['refer']) . '/';

	$pages = array();
	foreach (get_existpages() as $_page)
	{
		if (strpos($_page,$pattern) === 0)
		{
			$pages[$_page] = strip_bracket($_page);
		}
	}
//	natcasesort($pages);
	srand((double)microtime()*1000000);
	$page = array_rand($pages);

	if ($page != '')
	{
		$vars['refer'] = $page;
	}
	return array('body'=>'','msg'=>'');
}
?>
