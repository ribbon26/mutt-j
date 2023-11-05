<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: menu.inc.php,v 1.8 2004/11/27 12:23:02 henoheno Exp $
//

// ���֥�˥塼����Ѥ���
define('MENU_ENABLE_SUBMENU', FALSE);

// ���֥�˥塼��̾��
define('MENU_SUBMENUBAR', 'MenuBar');

function plugin_menu_convert()
{
	global $vars, $menubar;
	static $menu = NULL;

	$num = func_num_args();
	if ($num > 0) {
		// Try to change default 'MenuBar' page name (only)
		if ($num > 1)       return '#menu(): Zero or One argument needed';
		if ($menu !== NULL) return '#menu(): Already set: ' . htmlspecialchars($menu);
		$args = func_get_args();
		if (! is_page($args[0])) {
			return '#menu(): No such page: ' . htmlspecialchars($args[0]);
		} else {
			$menu = $args[0]; // Set
			return '';
		}

	} else {
		// Output menubar page data
		$page = ($menu === NULL) ? $menubar : $menu;

		if (MENU_ENABLE_SUBMENU) {
			$path = explode('/', strip_bracket($vars['page']));
			while(! empty($path)) {
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
			return '<!-- #menu(): You already view ' . htmlspecialchars($page) . ' -->';
		} else {
			// Cut fixed anchors
			$menutext = preg_replace('/^(\*{1,3}.*)\[#[A-Za-z][\w-]+\](.*)$/m', '$1$2', get_source($page));

			return preg_replace('/<ul[^>]*>/', '<ul>', convert_html($menutext));  
		}
	}
}
?>
