<?php
/*
 * PukiWiki BugTrackプラグイン
 *
 * CopyRight 2002 Y.MASUI GPL2
 * http://masui.net/pukiwiki/ masui@masui.net
 *
 * 変更履歴:
 *  2002.06.17: 作り始め
 *
 * $Id: bugtrack_list.inc.php,v 1.4 2004/07/31 03:09:20 henoheno Exp $
 */

require_once(PLUGIN_DIR.'bugtrack.inc.php');

function plugin_bugtrack_list_init()
{
	plugin_bugtrack_init();
}
?>
