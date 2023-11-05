<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: menu.inc.php,v 1.2 2003/07/22 06:00:23 arino Exp $
//

// ���֥�˥塼����Ѥ���
define('MENU_ENABLE_SUBMENU',FALSE);

function plugin_menu_convert()
{
	global $script,$vars,$menubar;
	static $menu = NULL;
	
	if (func_num_args())
	{
		$args = func_get_args();
		if (is_page($args[0]))
		{
			$menu = $args[0];
		}
		return '';
	}
	
	$page = ($menu === NULL) ? $menubar : $menu;
	if (MENU_ENABLE_SUBMENU)
	{
		$path = explode('/',strip_bracket($vars['page']));
		while(count($path))
		{
			$_page = join('/',$path).'/MenuBar';
			if (is_page($_page))
			{
				$page = $_page;
				break;
			}
			array_pop($path);
		}
	}
	if (!is_page($page))
	{
		return '';
	}
	return preg_replace('/<ul[^>]*>/','<ul>',convert_html(get_source($page)));
}
?>
