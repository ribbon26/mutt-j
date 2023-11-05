<?php
// $Id: referer.inc.php,v 1.7 2004/07/31 03:09:20 henoheno Exp $
/*
 * PukiWiki Referer �ץ饰����(��󥯸�ɽ���ץ饰����)
 * (C) 2003, Katsumi Saito <katsumi@jo1upk.ymt.prug.or.jp>
 * License: GPL
*/

// ��������ե�����
define('CONFIG_REFERER','plugin/referer/config');

function plugin_referer_action()
{
	global $vars,$referer;
	global $_referer_msg;

	// Referer��ǽ����Ѥ��Ƥ��ʤ�
	if (!$referer)
	{
		return array('msg'=>'','body'=>'');
	}

	if (array_key_exists('page',$vars) and is_page($vars['page']))
	{
		// �����
		$sort = (empty($vars['sort'])) ? '0d' : $vars['sort'];

		return array(
			'msg'  => $_referer_msg['msg_H0_Refer'],
			'body' => referer_body($vars['page'],$sort)
		);
	}
	$pages = get_existpages(TRACKBACK_DIR,'.ref');

	if (count($pages) == 0)
	{
		return array('msg'=>'','body'=>'');
	}

	return array(
		'msg' => 'referer list',
		'body' => page_list($pages,'referer',FALSE)
	);
}

// Referer ���ٹ��Խ�
function referer_body($page,$sort)
{
	global $script,$_referer_msg;

	$data = tb_get(tb_get_filename($page,'.ref'));
	if (count($data) == 0)
	{
		return '<p>no data.</p>';
	}
	$bg = referer_set_color();

	$arrow_last = $arrow_1st = $arrow_ctr = '';
	$color_last = $color_1st = $color_ctr = $color_ref = $bg['etc'];
	$sort_last = '0d';
	$sort_1st = '1d';
	$sort_ctr = '2d';

	switch ($sort)
	{
		case '0d': // 0d �ǽ���������(�����)
			usort($data,create_function('$a,$b','return $b[0] - $a[0];'));
			$color_last = $bg['cur'];
			$arrow_last = $_referer_msg['msg_Chr_darr'];
			$sort_last = '0a';
			break;
		case '0a': // 0a �ǽ���������(���ս�)
			usort($data,create_function('$a,$b','return $a[0] - $b[0];'));
			$color_last = $bg['cur'];
			$arrow_last = $_referer_msg['msg_Chr_uarr'];
//			$sort_last = '0d';
			break;
		case '1d': // 1d �����Ͽ����(�����)
			usort($data,create_function('$a,$b','return $b[1] - $a[1];'));
			$color_1st = $bg['cur'];
			$arrow_1st = $_referer_msg['msg_Chr_darr'];
			$sort_1st = '1a';
			break;
		case '1a': // 1a �����Ͽ����(���ս�)
			usort($data,create_function('$a,$b','return $a[1] - $b[1];'));
			$color_1st = $bg['cur'];
			$arrow_1st = $_referer_msg['msg_Chr_uarr'];
//			$sort_1st = '1d';
			break;
		case '2d': // 2d ������(�礭����)
			usort($data,create_function('$a,$b','return $b[2] - $a[2];'));
			$color_ctr = $bg['cur'];
			$arrow_ctr = $_referer_msg['msg_Chr_darr'];
			$sort_ctr = '2a';
			break;
		case '2a': // 2a ������(��������)
			usort($data,create_function('$a,$b','return $a[2] - $b[2];'));
			$color_ctr = $bg['cur'];
			$arrow_ctr = $_referer_msg['msg_Chr_uarr'];
//			$sort_ctr = '2d';
			break;
		case '3': // 3 Referer
			usort($data,create_function('$a,$b','return ($a[3] == $b[3]) ? 0 : (($a[3] > $b[3]) ? 1 : -1);'));
			$color_ref = $bg['cur'];
			break;
	}

	$body = '';
	foreach ($data as $arr)
	{
		// 0:�ǽ���������, 1:�����Ͽ����, 2:���ȥ�����, 3:Referer �إå�, 4:���Ѳ��ݥե饰(1��ͭ��)
		list($ltime,$stime,$count,$url,$enable) = $arr;

		// ��ASCII����饯��(����)��URL���󥳡��ɤ��Ƥ��� BugTrack/440
		$e_url = htmlspecialchars(preg_replace('/([" \x80-\xff]+)/e','rawurlencode("$1")',$url));
		$s_url = htmlspecialchars(mb_convert_encoding(rawurldecode($url),SOURCE_ENCODING,'auto'));

		$lpass = get_passage($ltime,FALSE); // �ǽ�������������ηв����
		$spass = get_passage($stime,FALSE); // �����Ͽ��������ηв����
		$ldate = get_date($_referer_msg['msg_Fmt_Date'],$ltime); // �ǽ���������ʸ����
		$sdate = get_date($_referer_msg['msg_Fmt_Date'],$stime); // �����Ͽ����ʸ����

		$body .= " <tr>\n  <td>$ldate</td>\n  <td>$lpass</td>\n";
		$body .= ($count == 1) ? "  <td colspan=\"2\">N/A</td>\n" : "  <td>$sdate</td>\n  <td>$spass</td>\n";
		$body .= "  <td style=\"text-align:right;\">$count</td>\n";

		// Ŭ���Բĥǡ����ΤȤ��ϥ��󥫡���Ĥ��ʤ�
		$body .= referer_ignore_check($url) ?
			"  <td>$s_url</td>\n" : "  <td><a href=\"$e_url\">$s_url</a></td>\n";
		$body .= " </tr>\n";
	}
	$href = $script.'?plugin=referer&amp;page='.rawurlencode($page);
	return <<<EOD
<table border="1" cellspacing="1" summary="Referer">
 <tr>
  <td style="background-color:$color_last" colspan="2">
   <a href="$href&amp;sort=$sort_last">{$_referer_msg['msg_Hed_LastUpdate']}$arrow_last</a>
  </td>
  <td style="background-color:$color_1st" colspan="2">
   <a href="$href&amp;sort=$sort_1st">{$_referer_msg['msg_Hed_1stDate']}$arrow_1st</a>
  </td>
  <td style="background-color:$color_ctr;text-align:right">
   <a href="$href&amp;sort=$sort_ctr">{$_referer_msg['msg_Hed_RefCounter']}$arrow_ctr</a>
  </td>
  <td style="background-color:$color_ref">
   <a href="$href&amp;sort=3">{$_referer_msg['msg_Hed_Referer']}</a>
   </td>
 </tr>
 $body
</table>
EOD;
}

function referer_set_color()
{
	static $color;

	if (!isset($color))
	{
		// �ǥե���ȥ��顼
		$color = array(
			'cur' => '#88ff88',
			'etc' => '#cccccc'
		);

		$config = new Config(CONFIG_REFERER);
		$config->read();
		$pconfig_color = $config->get('COLOR');
		unset($config);
		foreach ($pconfig_color as $x)
		{
			// BGCOLOR(#88ff88)
			$color[$x[0]] = htmlspecialchars(
				preg_match('/BGCOLOR\(([^)]+)\)/si',$x[1],$matches) ? $matches[1] : $x[1]
			);
		}
	}
	return $color;
}

function referer_ignore_check($url)
{
	static $ignore_url;

	// config.php
	if (!isset($ignore_url))
	{
		$config = new Config(CONFIG_REFERER);
		$config->read();
		$ignore_url = $config->get('IGNORE');
		unset($config);
	}

	foreach ($ignore_url as $x)
	{
		if (strpos($url,$x) !== FALSE)
		{
			return 1;
		}
	}
	return 0;
}
?>
