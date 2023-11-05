<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: file.php,v 1.41 2004/03/20 13:43:58 arino Exp $
//

// �����������
function get_source($page=NULL)
{
	if (!is_page($page))
	{
		return array();
	}
	return str_replace("\r",'',file(get_filename($page)));
}

// �ڡ����ι������������
function get_filetime($page)
{
	if (!is_page($page))
	{
		return 0;
	}
	return filemtime(get_filename($page)) - LOCALZONE;
}

// �ڡ����Υե�����̾������
function get_filename($page)
{
	return DATA_DIR.encode($page).'.txt';
}

// �ڡ����ν���
function page_write($page,$postdata,$notimestamp=FALSE)
{
	$postdata = make_str_rules($postdata);
	
	// ��ʬ�ե�����κ���
	$oldpostdata = is_page($page) ? join('',get_source($page)) : '';
	$diffdata = do_diff($oldpostdata,$postdata);
	file_write(DIFF_DIR,$page,$diffdata);
	
	// �Хå����åפκ���
	make_backup($page,$postdata == '');
	
	// �ե�����ν񤭹���
	file_write(DATA_DIR,$page,$postdata,$notimestamp);
	
	// TrackBack Ping ������
	// ���ɲá׹Ԥ����
	$lines = join("\n",preg_replace('/^\+/','',preg_grep('/^\+/',explode("\n",$diffdata))));
	tb_send($page,$lines);
	
	// link�ǡ����١����򹹿�
	links_update($page);
}

// �桼������롼��(���������ִ�����)
function make_str_rules($str)
{
	global $str_rules,$fixed_heading_anchor;
	
	$arr = explode("\n",$str);
	
	$retvars = array();
	foreach ($arr as $str)
	{
		if ($str != '' and $str{0} != ' ' and $str{0} != "\t")
		{
			foreach ($str_rules as $rule => $replace)
			{
				$str = preg_replace("/$rule/",$replace,$str);
			}
		}
		// ���Ф��˸�ͭID����Ϳ����
		if ($fixed_heading_anchor and
			preg_match('/^(\*{1,3}(.(?!\[#[A-Za-z][\w-]+\]))+)$/',$str,$matches))
		{
			// ��ͭID����������
			// ������ʱѻ�(1ʸ��)+md5�ϥå���Υ��������ʬʸ����(7ʸ��)
			$anchor = chr(mt_rand(ord('a'),ord('z'))).
				substr(md5(uniqid(substr($matches[1],0,100),1)),mt_rand(0,24),7);
			$str = rtrim($matches[1])." [#$anchor]";
		}
		$retvars[] = $str;
	}
	
	return join("\n",$retvars);
}

// �ե�����ؤν���
function file_write($dir,$page,$str,$notimestamp=FALSE)
{
	global $post,$update_exec;
	global $_msg_invalidiwn;
	global $notify,$notify_diff_only,$notify_to,$notify_from,$notify_subject,$notify_header;
	global $smtp_server,$smtp_auth;
	
	if (!is_pagename($page))
	{
		die_message(
			str_replace('$1',htmlspecialchars($page),
				str_replace('$2','WikiName',$_msg_invalidiwn)
			)
		);
	}
	$page = strip_bracket($page);
	$timestamp = FALSE;
	$file = $dir.encode($page).'.txt';
	
	if ($dir == DATA_DIR and $str == '' and file_exists($file))
	{
		unlink($file);
		put_recentdeleted($page);
	}
	if ($str != '')
	{
		$str = preg_replace("/\r/",'',$str);
		$str = rtrim($str)."\n";
		
		if ($notimestamp and file_exists($file))
		{
			$timestamp = filemtime($file) - LOCALZONE;
		}
		
		$fp = fopen($file,'w')
			or die_message('cannot write page file or diff file or other'.htmlspecialchars($page).'<br />maybe permission is not writable or filename is too long');
		set_file_buffer($fp, 0);
		flock($fp,LOCK_EX);
		rewind($fp);
		fputs($fp,$str);
		flock($fp,LOCK_UN);
		fclose($fp);
		if ($timestamp)
		{
			touch($file,$timestamp + LOCALZONE);
		}
	}
	
	// is_page�Υ���å���򥯥ꥢ���롣
	is_page($page,TRUE);
	
	if (!$timestamp and $dir == DATA_DIR)
	{
		put_lastmodified();
	}
	
	if ($update_exec and $dir == DATA_DIR)
	{
		system($update_exec.' > /dev/null &');
	}
	
	if ($notify and $dir == DIFF_DIR)
	{
		if ($notify_diff_only)
		{
			// ��ʬ��������������
			$str = preg_replace('/^[^-+].*\n/m','',$str);
		}
		if ($smtp_auth)
		{
			pop_before_smtp();
		}
 		$subject = str_replace('$page',$page,$notify_subject);
		ini_set('SMTP',$smtp_server);
 		mb_language(LANG);
 		mb_send_mail($notify_to,$subject,$str,$notify_header);
	}
}
// �������ڡ����ι���
function put_recentdeleted($page)
{
	global $whatsdeleted,$maxshow_deleted;
	
	if ($maxshow_deleted == 0)
	{
		return;
	}
	// update RecentDeleted
	$lines = array();
	foreach (get_source($whatsdeleted) as $line)
	{
		if (preg_match('/^-(.+) - (\[\[.+\]\])$/',$line,$matches))
		{
			$lines[$matches[2]] = $line;
		}
	}
	$_page = "[[$page]]";
	if (array_key_exists($_page,$lines))
	{
		unset($lines[$_page]);
	}
	array_unshift($lines,'-'.format_date(UTIME)." - $_page\n");
	$lines = array_splice($lines,0,$maxshow_deleted);
	$fp = fopen(get_filename($whatsdeleted),'w')
		or die_message('cannot write page file '.htmlspecialchars($whatsdeleted).'<br />maybe permission is not writable or filename is too long');
	set_file_buffer($fp, 0);
	flock($fp,LOCK_EX);
	rewind($fp);
	fputs($fp,join('',$lines));
	fputs($fp,"#norelated\n"); // :)
	flock($fp,LOCK_UN);
	fclose($fp);
}

// �ǽ������ڡ����ι���
function put_lastmodified()
{
	global $maxshow,$whatsnew,$non_list,$autolink;

	$pages = get_existpages();
	$recent_pages = array();
	foreach($pages as $page)
	{
		if ($page != $whatsnew and !preg_match("/$non_list/",$page))
		{
			$recent_pages[$page] = get_filetime($page);
		}
	}
	
	//����߽�ǥ�����
	arsort($recent_pages,SORT_NUMERIC);
	
	// create recent.dat (for recent.inc.php)
	$fp = fopen(CACHE_DIR.'recent.dat','w')
		or die_message('cannot write cache file '.CACHE_DIR.'recent.dat<br />maybe permission is not writable or filename is too long');
	set_file_buffer($fp, 0);
	flock($fp,LOCK_EX);
	rewind($fp);
	foreach ($recent_pages as $page=>$time)
	{
		fputs($fp,"$time\t$page\n");
	}
	flock($fp,LOCK_UN);
	fclose($fp);

	// create RecentChanges
	$fp = fopen(get_filename($whatsnew),'w')
		or die_message('cannot write page file '.htmlspecialchars($whatsnew).'<br />maybe permission is not writable or filename is too long');
	set_file_buffer($fp, 0);
	flock($fp,LOCK_EX);
	rewind($fp);
	foreach (array_splice(array_keys($recent_pages),0,$maxshow) as $page)
	{
		$time = $recent_pages[$page];
		$s_lastmod = htmlspecialchars(format_date($time));
		$s_page = htmlspecialchars($page);
		fputs($fp, "-$s_lastmod - [[$s_page]]\n");
	}
	fputs($fp,"#norelated\n"); // :)
	flock($fp,LOCK_UN);
	fclose($fp);
	
	// for autolink
	if ($autolink)
	{
		list($pattern,$pattern_a,$forceignorelist) = get_autolink_pattern($pages);
		
		$fp = fopen(CACHE_DIR.'autolink.dat','w')
			or die_message('cannot write autolink file '.CACHE_DIR.'/autolink.dat<br />maybe permission is not writable');
		set_file_buffer($fp, 0);
		flock($fp,LOCK_EX);
		rewind($fp);
		fputs($fp,$pattern."\n");
		fputs($fp,$pattern_a."\n");
		fputs($fp,join("\t",$forceignorelist)."\n");
		flock($fp,LOCK_UN);
		fclose($fp);
	}
}

// ���ꤵ�줿�ڡ����ηв����
function get_pg_passage($page,$sw=TRUE)
{
	global $show_passage;
	static $pg_passage = array();
	
	if (!$show_passage)
	{
		return '';
	}
	
	if (!array_key_exists($page,$pg_passage))
	{
		$pg_passage[$page] = (is_page($page) and $time = get_filetime($page)) ?
			get_passage($time) : '';
	}
	
	return $sw ? "<small>{$pg_passage[$page]}</small>" : " {$pg_passage[$page]}";
}

// Last-Modified �إå�
function header_lastmod($page=NULL)
{
	global $lastmod;
	
	if ($lastmod and is_page($page))
	{
		header('Last-Modified: '.date('D, d M Y H:i:s',get_filetime($page)).' GMT');
	}
}

// ���ڡ���̾�������
function get_existpages($dir=DATA_DIR,$ext='.txt')
{
	$aryret = array();
	
	$pattern = '^((?:[0-9A-F]{2})+)';
	if ($ext != '')
	{
		$pattern .= preg_quote($ext,'/').'$';
	}
	$dp = @opendir($dir)
		or die_message($dir. ' is not found or not readable.');
	while ($file = readdir($dp))
	{
		if (preg_match("/$pattern/",$file,$matches))
		{
			$aryret[$file] = decode($matches[1]);
		}
	}
	closedir($dp);
	return $aryret;
}
// �ڡ���̾���ɤߤ������
function get_readings()
{
	global $pagereading_enable, $pagereading_kanji2kana_converter;
	global $pagereading_kanji2kana_encoding, $pagereading_chasen_path;
	global $pagereading_kakasi_path, $pagereading_config_page;
	global $pagereading_config_dict;
	
	$pages = get_existpages();
	
	$readings = array();
	foreach ($pages as $page) {
		$readings[$page] = '';
	}
	$deletedPage = FALSE;
	foreach (get_source($pagereading_config_page) as $line) {
		$line = chop($line);
		if(preg_match('/^-\[\[([^]]+)\]\]\s+(.+)$/', $line, $matches)) {
			if(isset($readings[$matches[1]])) {
				// �ɤߤ������Υڡ���
				$readings[$matches[1]] = $matches[2];
			} else {
				// ������줿�ڡ���
				$deletedPage = TRUE;
			}
		}
	}
	if($pagereading_enable) {
		// ChaSen/KAKASI �ƤӽФ���ͭ�������ꤵ��Ƥ�����
		$unknownPage = FALSE;
		// �ɤߤ������Υڡ��������뤫�����å�
		foreach ($readings as $page => $reading) {
			if($reading=='') {
				$unknownPage = TRUE;
				break;
			}
		}
		if($unknownPage) {
			// �ɤߤ������Υڡ����������硢ChaSen/KAKASI ��¹�
			switch(strtolower($pagereading_kanji2kana_converter)) {
			case 'chasen':
				$tmpfname = tempnam(CACHE_DIR, 'PageReading');
				$fp = fopen($tmpfname, "w")
					or die_message("cannot write temporary file '$tmpfname'.\n");
				foreach ($readings as $page => $reading) {
					if($reading=='') {
						fputs($fp, mb_convert_encoding("$page\n", $pagereading_kanji2kana_encoding, SOURCE_ENCODING));
					}
				}
				fclose($fp);
				if(!file_exists($pagereading_chasen_path)) {
					unlink($tmpfname);
					die_message("CHASEN not found: $pagereading_chasen_path");
				}
				$fp = popen("$pagereading_chasen_path -F %y $tmpfname", "r");
				if(!$fp) {
					unlink($tmpfname);
					die_message("ChaSen execution failed: $pagereading_chasen_path -F %y $tmpfname");
				}
				foreach ($readings as $page => $reading) {
					if($reading=='') {
						$line = fgets($fp);
						$line = mb_convert_encoding($line, SOURCE_ENCODING, $pagereading_kanji2kana_encoding);
						$line = chop($line);
						$readings[$page] = $line;
					}
				}
				pclose($fp);
				unlink($tmpfname) or die_message("temporary file can not be removed: $tmpfname");
				break;
			case 'kakasi':
			case 'kakashi':
				$tmpfname = tempnam(CACHE_DIR, 'PageReading');
				$fp = fopen($tmpfname, "w")
					or die_message("cannot write temporary file '$tmpfname'.\n");
				foreach ($readings as $page => $reading) {
					if($reading=='') {
						fputs($fp, mb_convert_encoding("$page\n", $pagereading_kanji2kana_encoding, SOURCE_ENCODING));
					}
				}
				fclose($fp);
				if(!file_exists($pagereading_kakasi_path)) {
					unlink($tmpfname);
					die_message("KAKASI not found: $pagereading_kakasi_path");
				}					
				$fp = popen("$pagereading_kakasi_path -kK -HK -JK <$tmpfname", "r");
				if(!$fp) {
					unlink($tmpfname);
					die_message("KAKASI execution failed: $pagereading_kakasi_path -kK -HK -JK <$tmpfname");
				}
				foreach ($readings as $page => $reading) {
					if($reading=='') {
						$line = fgets($fp);
						$line = mb_convert_encoding($line, SOURCE_ENCODING, $pagereading_kanji2kana_encoding);
						$line = chop($line);
						$readings[$page] = $line;
					}
				}
				pclose($fp);
				unlink($tmpfname) or die_message("temporary file can not be removed: $tmpfname");
				break;
			case 'none':
				$patterns = array();
				$replacements = array();
				foreach (get_source($pagereading_config_dict) as $line) {
					$line = chop($line);
					if(preg_match('|^ /([^/]+)/,\s*(.+)$|', $line, $matches)) {
						$patterns[] = $matches[1];
						$replacements[] = $matches[2];
					}
				}
				foreach ($readings as $page => $reading) {
					if($reading=='') {
						$readings[$page] = $page;
						foreach ($patterns as $no => $pattern) {
							$readings[$page] = mb_convert_kana(mb_ereg_replace($pattern, $replacements[$no], $readings[$page]), "aKCV");
						}
					}
				}
				break;
			default:
				die_message("unknown kanji-kana converter: $pagereading_kanji2kana_converter.");
				break;
			}
		}
		if($unknownPage or $deletedPage) {
			// �ɤߤǥ�����
			asort($readings);
			
			// �ڡ�����񤭹���
			$body = '';
			foreach ($readings as $page => $reading) {
				$body .= "-[[$page]] $reading\n";
			}
			page_write($pagereading_config_page, $body);
		}
	}
	
	// �ɤ������Υڡ����ϡ����Τޤޥڡ���̾���֤� (ChaSen/KAKASI ��
	// �ӽФ���̵�������ꤵ��Ƥ�����䡢ChaSen/KAKASI �ƤӽФ���
	// ���Ԥ������ΰ�)
	foreach ($pages as $page) {
		if($readings[$page]=='') {
			$readings[$page] = $page;
		}
	}
	
	return $readings;
}
//�ե�����̾�ΰ����������(���󥳡��ɺѤߡ���ĥ�Ҥ����)
function get_existfiles($dir,$ext)
{
	$aryret = array();
	
	$pattern = '^(?:[0-9A-F]{2})+'.preg_quote($ext,'/').'$';
	$dp = @opendir($dir)
		or die_message($dir. ' is not found or not readable.');
	while ($file = readdir($dp)) {
		if (preg_match("/$pattern/",$file)) {
			$aryret[] = $dir.$file;
		}
	}
	closedir($dp);
	return $aryret;
}
//����ڡ����δ�Ϣ�ڡ���������
function links_get_related($page)
{
	global $vars,$related;
	static $links = array();
	
	if (array_key_exists($page,$links))
	{
		return $links[$page];
	}
	
	// ��ǽ�ʤ�make_link()������������Ϣ�ڡ����������
	$links[$page] = ($page == $vars['page']) ? $related : array();
	
	// �ǡ����١��������Ϣ�ڡ���������
	$links[$page] += links_get_related_db($vars['page']);
	
	return $links[$page];
}
?>
