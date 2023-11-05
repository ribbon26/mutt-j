<?php
// $Id: back.inc.php,v 1.5 2004/07/31 03:09:20 henoheno Exp $
/*
 * PukiWiki back �ץ饰����
 * (C) 2002, Katsumi Saito <katsumi@jo1upk.ymt.prug.or.jp>
 *
 * [������]
 * #back(,,0)
 * #back(,left)
 * #back(,right,0)
 * #back(���,center,0,http://localhost)
 *
 * [����]
 * 1 - ʸ��                  ��ά��:���
 * 2 - left, center, right   ��ά��:center
 * 3 - <hr> ������̵ͭ       ��ά��:����
 * 4 - �̾�ϡ����ʤ櫓�ʤ�Ǥ������ɤ����Ƥ�ξ��������������ǽ
 */

function plugin_back_convert()
{
	global $_msg_back_word;

	$argv = func_get_args();

	// ���������
	$word  = $_msg_back_word;
	$align = 'center';
	$hr    = 1;
	$href  = 'javascript:history.go(-1)';
	$ret   = '';

	// �ѥ�᡼����Ƚ��
	if (func_num_args() > 0) {
		$word = htmlspecialchars(trim(strip_tags($argv[0])));
		if ($word == '') $word = $_msg_back_word;
	}
	if (func_num_args() > 1) {
		$align = htmlspecialchars(trim(strip_tags($argv[1])));
		if ($align == '') $align = 'center';
	}
	if (func_num_args() > 2) {
		$hr = trim(strip_tags($argv[2]));
	}
	if (func_num_args() > 3) {
		$href = rawurlencode(trim(strip_tags($argv[3])));
		if ($href == '') $href = 'javascript:history.go(-1)';
	}

	// <hr> ��������Ϥ��뤫�ɤ���
	if ($hr) {
		$ret = "<hr class=\"full_hr\" />\n";
	}

	$ret.= "<div style=\"text-align:$align\">[ <a href=\"$href\">$word</a> ]</div>\n";

	return $ret;
}
?>
