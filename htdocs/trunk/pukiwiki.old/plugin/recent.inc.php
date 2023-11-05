<?php
/*
 * PukiWiki �ǿ���?���ɽ������ץ饰����
 *
 * CopyRight 2002 Y.MASUI GPL2
 * http://masui.net/pukiwiki/ masui@masui.net
 * 
 * �ѹ�����:
 *  2002.04.08: pat���󡢤ߤΤ뤵��λ�Ŧ�ˤ�ꡢ����褬���ܸ�ξ���
 *              ������Τ���
 * 
 *  2002.06.17: plugin_recent_init()������
 *  2002.07.02: <ul>�ˤ����Ϥ��ѹ�����¤��
 *
 * $Id: recent.inc.php,v 1.10 2003/02/22 13:16:21 panda Exp $
 */

function plugin_recent_convert()
{
	global $script,$BracketName,$date_format;
	global $_recent_plugin_frame;
	
	$recent_lines = 10;
	if (func_num_args()) {
		$args = func_get_args();
		if (is_numeric($args[0])) {
			$recent_lines = $args[0];
		}
	}
	
	$date = $items = '';
	if (!file_exists(CACHE_DIR.'recent.dat')) {
		return '';
	}
	$recent = file(CACHE_DIR.'recent.dat');
	$lines = array_splice($recent,0,$recent_lines);
	foreach ($lines as $line) {
		list($time,$page) = explode("\t",rtrim($line));
		$_date = get_date($date_format,$time);
		if ($date != $_date) {
			if ($date != '') {
				$items .= '</ul>';
			}
			$date = $_date;
			$items .= "<strong>$date</strong>\n<ul class=\"recent_list\">";
		}
		$s_page = htmlspecialchars($page);
		$r_page = rawurlencode($page);
		$pg_passage = get_pg_passage($page,FALSE);
		$items .=" <li><a href=\"$script?$r_page\" title=\"$s_page $pg_passage\">$s_page</a></li>\n";
	}
	if (count($lines)) {
		$items .='</ul>';
	}
	return sprintf($_recent_plugin_frame,count($lines),$items);
}
?>
