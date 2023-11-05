<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: menu.inc.php,v 1.6 2004/09/04 05:42:37 henoheno Exp $
//

// ���֥�˥塼����Ѥ���
define('MENU_ENABLE_SUBMENU', FALSE);

// ���֥�˥塼��̾��
define('MENU_SUBMENUBAR', 'MenuBar');

function plugin_menu_convert()
{
	global $vars, $menubar;
	static $menu = NULL;

	if (func_num_args()) {
		$args = func_get_args();
		if (is_page($args[0])) $menu = $args[0];
		return '';
	}

	$page = ($menu === NULL) ? $menubar : $menu;

	if (MENU_ENABLE_SUBMENU) {
		$path = explode('/', strip_bracket($vars['page']));
		while(count($path)) {
			$_page = join('/', $path) . '/' . MENU_SUBMENUBAR;
			if (is_page($_page)) {
				$page = $_page;
				break;
			}
			array_pop($path);
		}
	}

	if (! is_page($page)) {
		return '';
	} else if ($vars['page'] == $page) {
		return '<!-- #menu(): You already view ' .
			htmlspecialchars($page) . ' -->';
	} else {
		return preg_replace('/<ul[^>]*>/', '<ul>', convert_html(get_source($page)));
	}
}
?>
