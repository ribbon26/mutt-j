<?php
// $Id: tb.inc.php,v 1.13 2004/07/31 13:41:20 henoheno Exp $
/*
 * PukiWiki TrackBack �ץ����
 * (C) 2003, Katsumi Saito <katsumi@jo1upk.ymt.prug.or.jp>
 * License: GPL
 *
 * plugin_tb_action()    action
 * tb_save()             TrackBack Ping �ǡ�����¸(����)
 * tb_xml_msg($rc, $msg) XML ��̽���
 * tb_mode_rss($tb_id)   ?__mode=rss ����
 * tb_mode_view($tb_id)  ?__mode=view ����
 */

function plugin_tb_action()
{
	global $vars, $trackback;

	// POST: TrackBack Ping ����¸����
	if (!empty($vars['url']))
		tb_save();

	if ($trackback and !empty($vars['__mode']) and !empty($vars['tb_id'])) {
		switch ($vars['__mode']) {
			case 'rss':  tb_mode_rss($vars['tb_id']);  break;
			case 'view': tb_mode_view($vars['tb_id']); break;
		}
	}
	$pages = get_existpages(TRACKBACK_DIR, '.txt');

	if (count($pages) == 0) {
		return array('msg'=>'', 'body'=>'');
	} else {
		return array('msg'=>'trackback list', 'body'=>page_list($pages, 'read', FALSE));
	}
}

// TrackBack Ping �ǡ�����¸(����)
function tb_save()
{
	global $script, $vars, $trackback;
	static $fields = array( /* UTIME, */ 'url', 'title', 'excerpt', 'blog_name');

	// ���Ĥ��Ƥ��ʤ��Τ˸ƤФ줿�����б�
	if (!$trackback) {
		tb_xml_msg(1, 'Feature inactive.');
	}
	// TrackBack Ping �ˤ����� URL �ѥ�᡼����ɬ�ܤǤ��롣
	if (empty($vars['url'])) {
		tb_xml_msg(1, 'It is an indispensable parameter. URL is not set up.');
	}
	// Query String ������
	if (empty($vars['tb_id'])) {
		tb_xml_msg(1, 'TrackBack Ping URL is inaccurate.');
	}

	$url   = $vars['url'];
	$tb_id = $vars['tb_id'];

	// �ڡ���¸�ߥ����å�
	$page = tb_id2page($tb_id);
	if ($page === FALSE) {
		tb_xml_msg(1, 'TrackBack ID is invalid.');
	}

	// URL �����������å� (����������Ƚ������֤����꤬�Ǥ�)
	$result = http_request($url, 'HEAD');
	if ($result['rc'] !== 200) {
		tb_xml_msg(1, 'URL is fictitious.');
	}

	// TRACKBACK_DIR ��¸�ߤȽ񤭹��߲�ǽ���γ�ǧ
	if (!file_exists(TRACKBACK_DIR)) {
		tb_xml_msg(1, 'No such directory');
	}
	if (!is_writable(TRACKBACK_DIR)) {
		tb_xml_msg(1, 'Permission denied');
	}

	// TrackBack Ping �Υǡ����򹹿�
	$filename = tb_get_filename($page);
	$data = tb_get($filename);

	$items = array(UTIME);
	foreach ($fields as $key)
	{
		$value = isset($vars[$key]) ? $vars[$key] : '';
		if (preg_match("/[,\"\n\r]/", $value)) {
			$value = '"' . str_replace('"', '""', $value) . '"';
		}
		$items[$key] = $value;
	}
	$data[rawurldecode($items['url'])] = $items;

	$fp = fopen($filename, 'w');
	set_file_buffer($fp, 0);
	flock($fp, LOCK_EX);
	rewind($fp);
	foreach ($data as $line) {
		$line = preg_replace('/[\r\n]/s', '', $line); // One line, one ping
		fwrite($fp, join(',', $line) . "\n");
	}
	flock($fp, LOCK_UN);
	fclose($fp);

	tb_xml_msg(0, '');
}

// XML ��̽���
function tb_xml_msg($rc, $msg)
{
	header('Content-Type: text/xml');
	echo '<?xml version="1.0" encoding="iso-8859-1"?>';
	echo <<<EOD

<response>
 <error>$rc</error>
 <message>$msg</message>
</response>
EOD;
	die;
}

// ?__mode=rss ����
function tb_mode_rss($tb_id)
{
	global $script, $vars, $entity_pattern;

	$page = tb_id2page($tb_id);
	if ($page === FALSE) {
		return FALSE;
	}

	$items = '';
	foreach (tb_get(tb_get_filename($page)) as $arr)
	{
		// $utime and $blog_name is not used
		$utime = array_shift($arr);
		list ($url, $title, $excerpt, $blog_name) = array_map(
			create_function('$a', 'return htmlspecialchars($a);'), $arr);
		$items .= <<<EOD

   <item>
    <title>$title</title>
    <link>$url</link>
    <description>$excerpt</description>
   </item>
EOD;
	}

	$title = htmlspecialchars($page);
	$link = "$script?" . rawurlencode($page);
	$vars['page'] = $page;
	$excerpt = strip_htmltag(convert_html(get_source($page)));
	$excerpt = preg_replace("/&$entity_pattern;/", '', $excerpt);
	$excerpt = mb_strimwidth(preg_replace("/[\r\n]/", ' ', $excerpt), 0, 255, '...');

	$rc = <<<EOD

<response>
 <error>0</error>
 <rss version="0.91">
  <channel>
   <title>$title</title>
   <link>$link</link>
   <description>$excerpt</description>
   <language>ja-Jp</language>$items
  </channel>
 </rss>
</response>
EOD;
	$rc = mb_convert_encoding($rc, 'UTF-8', SOURCE_ENCODING);
	header('Content-Type: text/xml');
	echo '<?xml version="1.0" encoding="utf-8" ?>';
	echo $rc;
	die;
}

// ?__mode=view ����
function tb_mode_view($tb_id)
{
	global $script, $page_title;
	global $_tb_title, $_tb_header, $_tb_entry, $_tb_refer, $_tb_date;
	global $_tb_header_Excerpt, $_tb_header_Weblog, $_tb_header_Tracked;

	// TrackBack ID ����ڡ���̾�����
	$page = tb_id2page($tb_id);
	if ($page === FALSE) {
		return FALSE;
	}
	$r_page = rawurlencode($page);

	$tb_title = sprintf($_tb_title, $page);
	$tb_refer = sprintf($_tb_refer, "<a href=\"$script?$r_page\">'$page'</a>", "<a href=\"$script\">$page_title</a>");


	$data = tb_get(tb_get_filename($page));

	// �ǿ��Ǥ�������
	usort($data, create_function('$a,$b', 'return $b[0] - $a[0];'));

	$tb_body = '';
	foreach ($data as $x)
	{
		if (count($x) != 5) {
			continue; // record broken
		}
		list ($time, $url, $title, $excerpt, $blog_name) = $x;
		if ($title == '') {
			$title = 'no title';
		}
		$time = date($_tb_date, $time + LOCALZONE); // May 2, 2003 11:25 AM
		$tb_body .= <<<EOD
<div class="trackback-body">
 <span class="trackback-post"><a href="$url" target="new">$title</a><br />
  <strong>$_tb_header_Excerpt</strong> $excerpt<br />
  <strong>$_tb_header_Weblog</strong> $blog_name<br />
  <strong>$_tb_header_Tracked</strong> $time
 </span>
</div>
EOD;
	}
	$msg = <<<EOD
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
 <meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8" />
 <title>$tb_title</title>
 <link rel="stylesheet" href="skin/trackback.css" type="text/css" />
</head>
<body>
 <div id="banner-commentspop">$_tb_header</div>
 <div class="blog">
  <div class="trackback-url">
   $_tb_entry<br />
   $script?tb_id=$tb_id<br /><br />
   $tb_refer
  </div>
  $tb_body
 </div>
</body>
</html>
EOD;
	// BugTrack/466 ���󥳡��ɸ�ǧ�к�
	header('Content-type: text/html; charset=UTF-8');
	echo '<?xml version="1.0" encoding="UTF-8"?>';
	echo mb_convert_encoding($msg, 'UTF-8', SOURCE_ENCODING);
	die;
}
?>
