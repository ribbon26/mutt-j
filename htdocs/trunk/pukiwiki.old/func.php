<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: func.php,v 1.62 2004/03/21 15:03:18 arino Exp $
//

// ʸ����InterWikiName���ɤ���
function is_interwiki($str)
{
	global $InterWikiName;

	return preg_match("/^$InterWikiName$/",$str);
}
// ʸ���󤬥ڡ���̾���ɤ���
function is_pagename($str)
{
	global $BracketName,$WikiName;
	
	$is_pagename = (!is_interwiki($str) and preg_match("/^(?!\/)$BracketName$(?<!\/$)/",$str)
		and !preg_match('/(^|\/)\.{1,2}(\/|$)/',$str));
	
	if (defined('SOURCE_ENCODING'))
	{
		if (SOURCE_ENCODING == 'UTF-8')
		{
			$is_pagename = ($is_pagename and preg_match('/^(?:[\x00-\x7F]|(?:[\xC0-\xDF][\x80-\xBF])|(?:[\xE0-\xEF][\x80-\xBF][\x80-\xBF]))+$/',$str)); // UTF-8
		}
		else if (SOURCE_ENCODING == 'EUC-JP')
		{
			$is_pagename = ($is_pagename and preg_match('/^(?:[\x00-\x7F]|(?:[\x8E\xA1-\xFE][\xA1-\xFE])|(?:\x8F[\xA1-\xFE][\xA1-\xFE]))+$/',$str)); // EUC-JP
		}
	}
	
	return $is_pagename;
}
// ʸ����URL���ɤ���
function is_url($str,$only_http=FALSE)
{
	$scheme = $only_http ? 'https?' : 'https?|ftp|news';
	return preg_match('/^('.$scheme.')(:\/\/[-_.!~*\'()a-zA-Z0-9;\/?:\@&=+\$,%#]*)$/', $str);
}
// �ڡ�����¸�ߤ��뤫
function is_page($page,$reload=FALSE)
{
	global $InterWikiName;
	static $is_page = array();
	
	if ($reload or !array_key_exists($page,$is_page))
	{
		$is_page[$page] = file_exists(get_filename($page));
	}
	
	return $is_page[$page];
}
// �ڡ������Խ���ǽ��
function is_editable($page)
{
	global $cantedit;
	static $is_editable = array();
	
	if (!array_key_exists($page,$is_editable))
	{
		$is_editable[$page] = (
			is_pagename($page) and
			!is_freeze($page) and
			!in_array($page,$cantedit)
		);
	}
	
	return $is_editable[$page];
}

// �ڡ�������뤵��Ƥ��뤫
function is_freeze($page)
{
	global $function_freeze;

	if (!$function_freeze or !is_page($page))
	{
		return FALSE;
	}

	list($lines) = get_source($page);
	return (rtrim($lines) == '#freeze');
}

// ��ư�ƥ�ץ졼��
function auto_template($page)
{
	global $auto_template_func,$auto_template_rules;
	
	if (!$auto_template_func)
	{
		return '';
	}

	$body = '';
	foreach ($auto_template_rules as $rule => $template)
	{
		if (preg_match("/$rule/",$page,$matches))
		{
			$template_page = preg_replace("/$rule/",$template,$page);
			if (is_page($template_page))
			{
				$body = join('',get_source($template_page));
				// ���Ф��θ�ͭID������
				$body = preg_replace('/^(\*{1,3}.*)\[#[A-Za-z][\w-]+\](.*)$/m','$1$2',$body);
				// #freeze����
				$body = preg_replace('/^#freeze\s*$/m','',$body);
				
				for ($i = 0; $i < count($matches); $i++)
				{
					$body = str_replace("\$$i",$matches[$i],$body);
				}
				break;
			}
		}
	}
	return $body;
}
// �������Ÿ������
function get_search_words($words,$special=FALSE)
{
	$retval = array();
	// Perl��� - �������ѥ�����ޥå�������
	// http://www.din.or.jp/~ohzaki/perl.htm#JP_Match
	$eucpre = $eucpost = '';
	if (SOURCE_ENCODING == 'EUC-JP')
	{
		$eucpre = '(?<!\x8F)';
		// # JIS X 0208 �� 0ʸ���ʾ�³���� # ASCII, SS2, SS3 �ޤ��Ͻ�ü
		$eucpost = '(?=(?:[\xA1-\xFE][\xA1-\xFE])*(?:[\x00-\x7F\x8E\x8F]|\z))';
	}
	$quote_func = create_function('$str','return preg_quote($str,"/");');
	// LANG=='ja'�ǡ�mb_convert_kana���Ȥ������mb_convert_kana�����
	$convert_kana = create_function('$str,$option',
		(LANG == 'ja' and function_exists('mb_convert_kana')) ?
			'return mb_convert_kana($str,$option);' : 'return $str;'
	);
	
	foreach ($words as $word)
	{
		// �ѿ�����Ⱦ��,�������ʤ�����,�Ҥ餬�ʤϥ������ʤ�
		$word_zk = $convert_kana($word,'aKCV');
		$chars = array();
		for ($pos = 0; $pos < mb_strlen($word_zk);$pos++)
		{
			$char = mb_substr($word_zk,$pos,1);
			// $special : htmlspecialchars()���̤���
			$arr = array($quote_func($special ? htmlspecialchars($char) : $char));
			if (strlen($char) == 1) // �ѿ���
			{
				foreach (array(strtoupper($char),strtolower($char)) as $_char)
				{
					if ($char != '&')
					{
						$arr[] = $quote_func($_char);
					}
					$ord = ord($_char);
					$arr[] = sprintf('&#(?:%d|x%x);',$ord,$ord); // ���λ���
					$arr[] = $quote_func($convert_kana($_char,"A")); // ����
				}
			}
			else // �ޥ���Х���ʸ��
			{
				$arr[] = $quote_func($convert_kana($char,"c")); // �Ҥ餬��
				$arr[] = $quote_func($convert_kana($char,"k")); // Ⱦ�ѥ�������
			}
			$chars[] = '(?:'.join('|',array_unique($arr)).')';
		}
		$retval[$word] = $eucpre.join('',$chars).$eucpost;
	}
	return $retval;
}
// ����
function do_search($word,$type='AND',$non_format=FALSE)
{
	global $script,$vars,$whatsnew,$non_list,$search_non_list;
	global $_msg_andresult,$_msg_orresult,$_msg_notfoundresult;
	global $search_auth;
	
	$database = array();
	$retval = array();

	$b_type = ($type == 'AND'); // AND:TRUE OR:FALSE
	$keys = get_search_words(preg_split('/\s+/',$word,-1,PREG_SPLIT_NO_EMPTY));
	
	$_pages = get_existpages();
	$pages = array();
	
	foreach ($_pages as $page)
	{
		if ($page == $whatsnew
			or (!$search_non_list and preg_match("/$non_list/",$page)))
		{
			continue;
		}
		
		// �����оݥڡ��������¤򤫤��뤫�ɤ��� (�ڡ���̾�����³�)
		if ($search_auth and !check_readable($page,false,false)) {
			$source = get_source(); // �����оݥڡ������Ƥ���ˡ�
		} else {
			$source = get_source($page);
		}
		if (!$non_format)
		{
			array_unshift($source,$page); // �ڡ���̾�⸡���оݤ�
		}
		
		$b_match = FALSE;
		foreach ($keys as $key)
		{
			$tmp = preg_grep("/$key/",$source);
			$b_match = (count($tmp) > 0);
			if ($b_match xor $b_type)
			{
				break;
			}
		}
		if ($b_match)
		{
			$pages[$page] = get_filetime($page);
		}
	}
	if ($non_format)
	{
		return array_keys($pages);
	}
	$r_word = rawurlencode($word);
	$s_word = htmlspecialchars($word);
	if (count($pages) == 0)
	{
		return str_replace('$1',$s_word,$_msg_notfoundresult);
	}
	ksort($pages);
	$retval = "<ul>\n";
	foreach ($pages as $page=>$time)
	{
		$r_page = rawurlencode($page);
		$s_page = htmlspecialchars($page);
		$passage = get_passage($time);
		$retval .= " <li><a href=\"$script?cmd=read&amp;page=$r_page&amp;word=$r_word\">$s_page</a>$passage</li>\n";
	}
	$retval .= "</ul>\n";
	
	$retval .= str_replace('$1',$s_word,str_replace('$2',count($pages),
		str_replace('$3',count($_pages),$b_type ? $_msg_andresult : $_msg_orresult)));
	
	return $retval;
}

// �ץ����ؤΰ����Υ����å�
function arg_check($str)
{
	global $vars;
	
	return array_key_exists('cmd',$vars) and (strpos($vars['cmd'],$str) === 0);
}

// �ڡ���̾�Υ��󥳡���
function encode($key)
{
	return ($key == '') ? '' : strtoupper(join('',unpack('H*0',$key)));
}

// �ڡ���̾�Υǥ�����
function decode($key)
{
	return ($key == '') ? '' : substr(pack('H*','20202020'.$key),4);
}

// [[ ]] �������
function strip_bracket($str)
{
	if (preg_match('/^\[\[(.*)\]\]$/',$str,$match))
	{
		$str = $match[1];
	}
	return $str;
}

// �ڡ��������κ���
function page_list($pages, $cmd = 'read', $withfilename=FALSE)
{
	global $script,$list_index,$top;
	global $_msg_symbol,$_msg_other;
	global $pagereading_enable;
	
	// �����ȥ�������ꤹ�롣 ' ' < '[a-zA-Z]' < 'zz'�Ȥ�������
	$symbol = ' ';
	$other = 'zz';
	
	$retval = '';
	
	if($pagereading_enable) {
		mb_regex_encoding(SOURCE_ENCODING);
		$readings = get_readings($pages);
	}
	
	$list = array();
	foreach($pages as $file=>$page)
	{
		$r_page = rawurlencode($page);
		$s_page = htmlspecialchars($page,ENT_QUOTES);
		$passage = get_pg_passage($page);
		
		$str = "   <li><a href=\"$script?cmd=$cmd&amp;page=$r_page\">$s_page</a>$passage";
		
		if ($withfilename)
		{
			$s_file = htmlspecialchars($file);
			$str .= "\n    <ul><li>$s_file</li></ul>\n   ";
		}
		$str .= "</li>";
		
		if($pagereading_enable) {
			if(mb_ereg('^([A-Za-z])',mb_convert_kana($page,'a'),$matches)) {
				$head = $matches[1];
			}
			elseif(mb_ereg('^([��-��])',$readings[$page],$matches)) {
				$head = $matches[1];
			}
			elseif (mb_ereg('^[ -~]|[^��-��-��]',$page)) {
				$head = $symbol;
			}
			else {
				$head = $other;
			}
		}
		else {
			$head = (preg_match('/^([A-Za-z])/',$page,$matches)) ? $matches[1] :
				(preg_match('/^([ -~])/',$page,$matches) ? $symbol : $other);
		}
		
		$list[$head][$page] = $str;
	}
	ksort($list);
	
	$cnt = 0;
	$arr_index = array();
	$retval .= "<ul>\n";
	foreach ($list as $head=>$pages)
	{
		if ($head === $symbol)
		{
			$head = $_msg_symbol;
		}
		else if ($head === $other)
		{
			$head = $_msg_other;
		}
		
		if ($list_index)
		{
			$cnt++;
			$arr_index[] = "<a id=\"top_$cnt\" href=\"#head_$cnt\"><strong>$head</strong></a>";
			$retval .= " <li><a id=\"head_$cnt\" href=\"#top_$cnt\"><strong>$head</strong></a>\n  <ul>\n";
		}
		ksort($pages);
		$retval .= join("\n",$pages);
		if ($list_index)
		{
			$retval .= "\n  </ul>\n </li>\n";
		}
	}
	$retval .= "</ul>\n";
	if ($list_index and $cnt > 0)
	{
		$top = array();
		while (count($arr_index) > 0)
		{
			$top[] = join(" | \n",array_splice($arr_index,0,16))."\n";
		}
		$retval = "<div id=\"top\" style=\"text-align:center\">\n".
			join("<br />",$top)."</div>\n".$retval;
	}
	return $retval;
}

// �ƥ����������롼���ɽ������
function catrule()
{
	global $rule_page;
	
	if (!is_page($rule_page))
	{
		return "<p>sorry, $rule_page unavailable.</p>";
	}
	return convert_html(get_source($rule_page));
}

// ���顼��å�������ɽ������
function die_message($msg)
{
	$title = $page = 'Runtime error';
	
	$body = <<<EOD
<h3>Runtime error</h3>
<strong>Error message : $msg</strong>
EOD;
	
	if(defined('SKIN_FILE') && file_exists(SKIN_FILE) && is_readable(SKIN_FILE))
	{
	  catbody($title,$page,$body);
	}
	else
	{
		header('Content-Type: text/html; charset=euc-jp');
		print <<<EOD
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
 <head>
  <title>$title</title>
  <meta http-equiv="content-type" content="text/html; charset=euc-jp">
 </head>
 <body>
 $body
 </body>
</html>
EOD;
	}
	die();
}

// ���߻����ޥ������äǼ���
function getmicrotime()
{
	list($usec, $sec) = explode(' ',microtime());
	return ((float)$sec + (float)$usec);
}

// ����������
function get_date($format,$timestamp = NULL)
{
	$time = ($timestamp === NULL) ? UTIME : $timestamp;
	$time += ZONETIME;
	
	$format = preg_replace('/(?<!\\\)T/',preg_replace('/(.)/','\\\$1',ZONE),$format);
	
	return date($format,$time);
}

// ����ʸ�������
function format_date($val, $paren = FALSE)
{
	global $date_format,$time_format,$weeklabels;
	
	$val += ZONETIME;
	
	$ins_date = date($date_format,$val);
	$ins_time = date($time_format,$val);
	$ins_week = '('.$weeklabels[date('w',$val)].')';
	
	$ins = "$ins_date $ins_week $ins_time";
	return $paren ? "($ins)" : $ins;
}

// �в����ʸ�������
function get_passage($time, $paren = TRUE)
{
	static $units = array('m'=>60,'h'=>24,'d'=>1);
	
	$time = max(0,(UTIME - $time) / 60); //minutes
	
	foreach ($units as $unit=>$card)
	{
		if ($time < $card)
		{
			break;
		}
		$time /= $card;
	}
	$time = floor($time).$unit;
	
	return $paren ? "($time)" : $time;
}

//<input type="(submit|button|image)"...>�򱣤�
function drop_submit($str)
{
	return preg_replace(
		'/<input([^>]+)type="(submit|button|image)"/i',
		'<input$1type="hidden"',
		$str
	);
}

// AutoLink�Υѥ��������������
// thx for hirofummy
function get_autolink_pattern(&$pages)
{
	global $WikiName,$autolink,$nowikiname;
	
	$config = &new Config('AutoLink');
	$config->read();
	$ignorepages = $config->get('IgnoreList');
	$forceignorepages = $config->get('ForceIgnoreList');
	unset($config);
	$auto_pages = array_merge($ignorepages,$forceignorepages);
	
	foreach ($pages as $page)
	{
		if (preg_match("/^$WikiName$/",$page) ?
			$nowikiname : strlen($page) >= $autolink)
		{
			$auto_pages[] = $page;
		}
	}

	if (count($auto_pages) == 0)
	{
		return $nowikiname ? '(?!)' : $WikiName;
	}
	
	$auto_pages = array_unique($auto_pages);
	sort($auto_pages,SORT_STRING);
	$auto_pages_a = array_values(preg_grep('/^[A-Z]+$/i',$auto_pages));
	$auto_pages = array_values(array_diff($auto_pages,$auto_pages_a));
	
	$result = get_autolink_pattern_sub($auto_pages,0,count($auto_pages),0);
	$result_a = get_autolink_pattern_sub($auto_pages_a,0,count($auto_pages_a),0);
	
	return array($result,$result_a,$forceignorepages);
}
function get_autolink_pattern_sub(&$pages,$start,$end,$pos)
{
	if ($end == 0)
	{
		return '(?!)';
	}
	
	$result = '';
	$count = 0;
	$x = (mb_strlen($pages[$start]) <= $pos);
	
	if ($x)
	{
		$start++;
	}
	for ($i = $start; $i < $end; $i = $j)
	{
		$char = mb_substr($pages[$i],$pos,1);
		for ($j = $i; $j < $end; $j++)
		{
			if (mb_substr($pages[$j],$pos,1) != $char)
			{
				break;
			}
		}
		if ($i != $start)
		{
			$result .= '|';
		}
		if ($i >= ($j - 1))
		{
			$result .= str_replace(' ','\\ ',preg_quote(mb_substr($pages[$i],$pos),'/')); 
		}
		else
		{
			$result .=
				str_replace(' ','\\ ',preg_quote($char,'/')).
				get_autolink_pattern_sub($pages,$i,$j,$pos + 1);
		}
		$count++;
	}
	if ($x or $count > 1)
	{
		$result = '(?:'.$result.')';
	}
	if ($x)
	{
		$result .= '?';
	}
	return $result;
}
// pukiwiki.php������ץȤ�absolute-uri������
function get_script_uri()
{
	// scheme
	$script  = ($_SERVER['SERVER_PORT'] == 443 ? 'https://' : 'http://');
	// host
	$script .= $_SERVER['SERVER_NAME'];
	// port
	$script .= ($_SERVER['SERVER_PORT'] == 80 ? '' : ':'.$_SERVER['SERVER_PORT']);
	// path
	$path = $_SERVER['SCRIPT_NAME'];
	// path��'/'�ǻϤޤäƤ��ʤ����(cgi�ʤ�) REQUEST_URI��ȤäƤߤ�
	if ($path{0} != '/')
	{
		if (!array_key_exists('REQUEST_URI',$_SERVER) or $_SERVER['REQUEST_URI']{0} != '/')
		{
			return FALSE;
		}
		// REQUEST_URI��ѡ�������path��ʬ��������Ф�
		$parse_url = parse_url($script.$_SERVER['REQUEST_URI']);
		if (!isset($parse_url['path']) or $parse_url['path']{0} != '/')
		{
			return FALSE;
		}
		$path = $parse_url['path'];
	}
	$script .= $path;
	
	return $script;
}
/*
�ѿ����null(\0)�Х��Ȥ�������
PHP��fopen("hoge.php\0.txt")��"hoge.php"�򳫤��Ƥ��ޤ��ʤɤ����ꤢ��

http://ns1.php.gr.jp/pipermail/php-users/2003-January/012742.html
[PHP-users 12736] null byte attack

2003-05-16: magic quotes gpc����������������
2003-05-21: Ϣ������Υ�����binary safe
*/ 
function sanitize($param)
{
	if (is_array($param))
	{
		$result = array_map('sanitize',$param);
	}
	else
	{
		$result = str_replace("\0",'',$param);
		if (get_magic_quotes_gpc())
		{
			$result = stripslashes($result);
		}
	}
	return $result;
}

// CSV������ʸ����������
function csv_explode($separator, $string)
{
	$_separator = preg_quote($separator,'/');
	if (!preg_match_all('/("[^"]*(?:""[^"]*)*"|[^'.$_separator.']*)'.$_separator.'/', $string.$separator, $matches))
	{
		return array();
	}

	$retval = array();
	foreach ($matches[1] as $str)
	{
		$len = strlen($str);
		if ($len > 1 and $str{0} == '"' and $str{$len - 1} == '"')
		{
			$str = str_replace('""', '"', substr($str, 1, -1));
		}
		$retval[] = $str;
	}
	return $retval;
}

// �����CSV������ʸ�����
function csv_implode($glue, $pieces)
{
	$_glue = ($glue != '') ? '\\'.$glue{0} : '';
	$arr = array();
	foreach ($pieces as $str)
	{
		if (ereg("[$_glue\"\n\r]",$str))
		{
			$str = '"'.str_replace('"', '""', $str).'"';
		}
		$arr[] = $str;
	}
	return join($glue, $arr);
}

//is_a
//(PHP 4 >= 4.2.0)
//
//is_a --  Returns TRUE if the object is of this class or has this class as one of its parents 

if (!function_exists('is_a'))
{
	function is_a($class, $match)
	{
		if (empty($class))
		{
			return false;
		}
		$class = is_object($class) ? get_class($class) : $class;
		if (strtolower($class) == strtolower($match))
		{
			return true;
		}
		return is_a(get_parent_class($class), $match);
	}
}

//array_fill
//(PHP 4 >= 4.2.0)
//
//array_fill -- Fill an array with values

if (!function_exists('array_fill'))
{
	function array_fill($start_index,$num,$value)
	{
		$ret = array();
		
		while ($num-- > 0)
		{
			$ret[$start_index++] = $value;
		}
		return $ret;
	}
}

//md5_file
//(PHP 4 >= 4.2.0)
//
//md5_file -- Calculates the md5 hash of a given filename

if (!function_exists('md5_file'))
{
	function md5_file($filename)
	{
		if (!file_exists($filename))
		{
			return FALSE;
		}
		$fd = fopen($filename, 'rb');
		$data = fread($fd, filesize($filename));
		fclose($fd);
		return md5($data);
	}
}
?>
