<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: rename.inc.php,v 1.11 2004/03/20 13:37:43 arino Exp $
//

/*
 * �ץ饰���� rename
 * �ڡ�����̾�����Ѥ���
 * 
 * Usage
 *  http:.../pukiwiki.php?plugin=rename(&refer=�ڡ���̾)
 *
 * �ѥ�᡼��
 * &refer=�ڡ���̾
 * �ڡ��������
 *
 */

define('RENAME_LOGPAGE',':RenameLog');

function plugin_rename_action()
{
	global $adminpass,$whatsnew;
	global $_rename_messages;
	
	$method = rename_getvar('method');
	if ($method == 'regex')
	{
		$src = rename_getvar('src');
		$dst = rename_getvar('dst');
		if ($src == '')
		{
			return rename_phase1();
		}
		$src_pattern = '/'.preg_quote($src,'/').'/';
		$arr0 = preg_grep($src_pattern,get_existpages());
		if (!is_array($arr0) or count($arr0) == 0)
		{
			return rename_phase1('nomatch');
		}
		$arr1 = preg_replace($src_pattern,$dst,$arr0);
		foreach ($arr1 as $page)
		{
			if (!is_pagename($page))
			{
				return rename_phase1('notvalid');
			}
		}
		return rename_regex($arr0,$arr1);
	}
	else // $method == 'page'
	{
		$page = rename_getvar('page');
		$refer = rename_getvar('refer');
		if ($refer == '')
		{
			return rename_phase1();
		}
		if (!is_page($refer))
		{
			return rename_phase1('notpage',$refer);
		}
		if ($refer == $whatsnew)
		{
			return rename_phase1('norename',$refer);
		}
		if ($page == '' or $page == $refer)
		{
			return rename_phase2();
		}
		if (!is_pagename($page))
		{
			return rename_phase2('notvalid');
		}
		return rename_refer();
	}
}
// �ѿ����������
function rename_getvar($key)
{
	global $vars;
	
	return array_key_exists($key,$vars) ? $vars[$key] : '';
}
// ���顼��å���������
function rename_err($err,$page='')
{
	global $_rename_messages;
	
	if ($err == '')
	{
		return '';
	}
	$body = $_rename_messages["err_$err"];
	if (is_array($page))
	{
		$tmp = '';
		foreach ($page as $_page)
		{
			$tmp .= "<br />$_page";
		}
		$page = $tmp;
	}
	if ($page != '')
	{
		$body = sprintf($body,htmlspecialchars($page));
	}
	$msg = sprintf($_rename_messages['err'],$body);
	return $msg;
}
//����ʳ�:�ڡ���̾�ޤ�������ɽ��������
function rename_phase1($err='',$page='')
{
	global $script,$_rename_messages;
	
	$msg = rename_err($err,$page);
	$refer = rename_getvar('refer');
	$method = rename_getvar('method');

	$radio_regex = $radio_page = '';
	if ($method == 'regex')
	{
		$radio_regex =' checked'; 
	}
	else
	{
		$radio_page = ' checked'; 
	}
	$select_refer = rename_getselecttag($refer);
	
	$s_src = htmlspecialchars(rename_getvar('src'));
	$s_dst = htmlspecialchars(rename_getvar('dst'));
	
	$ret['msg'] = $_rename_messages['msg_title'];
	$ret['body'] = <<<EOD
$msg
<form action="$script" method="post">
 <div>
  <input type="hidden" name="plugin" value="rename" />
  <input type="radio" name="method" value="page"$radio_page />
  {$_rename_messages['msg_page']}:$select_refer<br />
  <input type="radio" name="method" value="regex"$radio_regex />
  {$_rename_messages['msg_regex']}:<br />
  From:<br />
  <input type="text" name="src" size="80" value="$s_src" /><br />
  To:<br />
  <input type="text" name="dst" size="80" value="$s_dst" /><br />
  <input type="submit" value="{$_rename_messages['btn_next']}" /><br />
 </div>
</form>
EOD;
	return $ret;
}
//�����ʳ�:������̾��������
function rename_phase2($err='')
{
	global $script,$_rename_messages;
	
	$msg = rename_err($err);
	$page = rename_getvar('page');
	$refer = rename_getvar('refer');
	if ($page == '')
	{
		$page = $refer;
	}
	
	$related = rename_getrelated($refer);
	$msg_related = '';
	if (count($related) > 0)
	{
		$msg_related = $_rename_messages['msg_do_related'].
			'<input type="checkbox" name="related" value="1" checked="checked" /><br />';
	}
	$msg_rename = sprintf($_rename_messages['msg_rename'],make_pagelink($refer));
	
	$s_page = htmlspecialchars($page);
	$s_refer = htmlspecialchars($refer);
	
	$ret['msg'] = $_rename_messages['msg_title'];
	$ret['body'] = <<<EOD
$msg
<form action="$script" method="post">
 <div>
  <input type="hidden" name="plugin" value="rename" />
  <input type="hidden" name="refer" value="$s_refer" />
  $msg_rename<br />
  {$_rename_messages['msg_newname']}:<input type="text" name="page" size="80" value="$s_page" /><br />
  $msg_related
  <input type="submit" value="{$_rename_messages['btn_next']}" /><br />
 </div>
</form>
EOD;
	if (count($related) > 0)
	{
		$ret['body'].= "<hr /><p>{$_rename_messages['msg_related']}</p><ul>";
		sort($related);
		foreach ($related as $name)
		{
			$ret['body'].= '<li>'.make_pagelink($name).'</li>'; 
		}
		$ret['body'].= '</ul>';
	}
	return $ret;
}
//�ڡ���̾�ȴ�Ϣ����ڡ�������󤷡�phase3��
function rename_refer()
{
	$page = rename_getvar('page');
	$refer = rename_getvar('refer');
	
	$pages[encode($refer)] = encode($page);
	if (rename_getvar('related') != '')
	{
		$from = strip_bracket($refer);
		$to =   strip_bracket($page);
		foreach (rename_getrelated($refer) as $_page)
		{
			$pages[encode($_page)] = encode(str_replace($from,$to,$_page));
		}
	}
	return rename_phase3($pages);
}
//����ɽ���ǥڡ������ִ�
function rename_regex($arr_from,$arr_to)
{
	$exists = array();
	foreach ($arr_to as $page)
	{
		if (is_page($page))
		{
			$exists[] = $page;
		}
	}
	if (count($exists) > 0)
	{
		return rename_phase1('already',$exists);
	}
	
	$pages = array();
	foreach ($arr_from as $refer)
	{
		$pages[encode($refer)] = encode(array_shift($arr_to));
	}
	return rename_phase3($pages);
}
function rename_phase3($pages)
{
	global $script,$adminpass,$_rename_messages;
	
	$msg = $input = '';
	$files = rename_get_files($pages);
	
	$exists = array();
	foreach ($files as $_page=>$arr)
	{
		foreach ($arr as $old=>$new)
		{
			if (file_exists($new))
			{
				$exists[$_page][$old] = $new;
			}
		}
	}
	$pass = rename_getvar('pass');
	if (md5($pass) == $adminpass)
	{
		return rename_proceed($pages,$files,$exists);
	}
	else if ($pass != '')
	{
		$msg = rename_err('adminpass');
	}
	$method = rename_getvar('method');
	if ($method == 'regex')
	{
		$s_src = htmlspecialchars(rename_getvar('src'));
		$s_dst = htmlspecialchars(rename_getvar('dst'));
		$msg .= $_rename_messages['msg_regex']."<br />";
		$input .= "<input type=\"hidden\" name=\"method\" value=\"regex\" />";
		$input .= "<input type=\"hidden\" name=\"src\" value=\"$s_src\" />";
		$input .= "<input type=\"hidden\" name=\"dst\" value=\"$s_dst\" />";
	}
	else
	{
		$s_refer = htmlspecialchars(rename_getvar('refer'));
		$s_page = htmlspecialchars(rename_getvar('page'));
		$s_related = htmlspecialchars(rename_getvar('related'));
		$msg .= $_rename_messages['msg_page']."<br />";
		$input .= "<input type=\"hidden\" name=\"method\" value=\"page\" />";
		$input .= "<input type=\"hidden\" name=\"refer\" value=\"$s_refer\" />";
		$input .= "<input type=\"hidden\" name=\"page\" value=\"$s_page\" />";
		$input .= "<input type=\"hidden\" name=\"related\" value=\"$s_related\" />";
	}
	
	if (count($exists) > 0)
	{
		$msg .= $_rename_messages['err_already_below']."<ul>";
		foreach ($exists as $page=>$arr)
		{
			$msg .= '<li>';
			$msg .= make_pagelink(decode($page));
			$msg .= $_rename_messages['msg_arrow'];
			$msg .= htmlspecialchars(decode($pages[$page]));
			if (count($arr) > 0)
			{
				$msg .= "<ul>\n";
				foreach ($arr as $ofile=>$nfile)
				{
					$msg .= '<li>'.$ofile.$_rename_messages['msg_arrow'].$nfile."</li>\n";
				}
				$msg .= '</ul>';
			}
			$msg .= "</li>\n";
		}
		$msg .= "</ul><hr />\n";
	
		$input .= '<input type="radio" name="exist" value="0" checked />'.$_rename_messages['msg_exist_none'].'<br />';
		$input .= '<input type="radio" name="exist" value="1" />'.$_rename_messages['msg_exist_overwrite'].'<br />';
	}
	$ret['msg'] = $_rename_messages['msg_title'];
	$ret['body'] = <<<EOD
<p>$msg</p>
<form action="$script" method="post">
 <div>
  <input type="hidden" name="plugin" value="rename" />
  $input
  {$_rename_messages['msg_adminpass']}
  <input type="password" name="pass" value="" />
  <input type="submit" value="{$_rename_messages['btn_submit']}" />
 </div>
</form>
<p>{$_rename_messages['msg_confirm']}</p>
EOD;
	
	ksort($pages);
	$ret['body'] .= "<ul>\n";
	foreach ($pages as $old=>$new)
	{
		$ret['body'] .= "<li>".
			make_pagelink(decode($old)).
			$_rename_messages['msg_arrow'].
			htmlspecialchars(decode($new)).
			"</li>\n";
	}
	$ret['body'] .= "</ul>\n";
	return $ret;
}
function rename_get_files($pages)
{
	$files = array();
	$dirs = array(BACKUP_DIR,DIFF_DIR,DATA_DIR);
	if (exist_plugin_convert('attach'))
	{
		$dirs[] = UPLOAD_DIR; 
	}
	if (exist_plugin_convert('counter'))
	{
		$dirs[] = COUNTER_DIR;
	}
	// and more ...
	
	foreach ($dirs as $path)
	{
		if (!$dir = opendir($path))
		{
			continue;
		}
		while ($file = readdir($dir))
		{
			if ($file == '.' or $file == '..')
			{
				continue; 
			}
			foreach ($pages as $from=>$to)
			{
				$pattern = '/^'.str_replace('/','\/',$from).'([._].+)$/';
				if (!preg_match($pattern,$file,$matches))
				{
					continue; 
				}
				$newfile = $to.$matches[1];
				$files[$from][$path.$file] = $path.$newfile;
			}
		}
	}
	return $files;
}
function rename_proceed($pages,$files,$exists)
{
	global $script,$now,$_rename_messages;
	
	if (rename_getvar('exist') == '')
	{
		foreach ($exists as $key=>$arr)
		{
			unset($files[$key]); 
		}
	}
	
	set_time_limit(0);
	foreach ($files as $page=>$arr)
	{
		foreach ($arr as $old=>$new)
		{
			if ($exists[$page][$old])
			{
				unlink($new); 
			}
			rename($old,$new);
			
			// link�ǡ����١����򹹿����� BugTrack/327 arino
			links_update($old);
			links_update($new);
		}
	}
	
	$postdata = get_source(RENAME_LOGPAGE);
	$postdata[] = '*'.$now."\n";
	if (rename_getvar('method') == 'regex')
	{
		$postdata[] = '-'.$_rename_messages['msg_regex']."\n";
		$postdata[] = '--From:[['.rename_getvar('src')."]]\n";
		$postdata[] = '--To:[['.rename_getvar('dst')."]]\n";
	}
	else
	{
		$postdata[] = '-'.$_rename_messages['msg_page']."\n";
		$postdata[] = '--From:[['.rename_getvar('refer')."]]\n";
		$postdata[] = '--To:[['.rename_getvar('page')."]]\n";
	}
	if (count($exists) > 0)
	{
		$postdata[] = "\n".$_rename_messages['msg_result']."\n";
		foreach ($exists as $page=>$arr)
		{
			$postdata[] = '-'.decode($page).
				$_rename_messages['msg_arrow'].decode($pages[$page])."\n";
			foreach ($arr as $ofile=>$nfile)
			{
				$postdata[] = '--'.$ofile.
					$_rename_messages['msg_arrow'].$nfile."\n";
			}
		}
		$postdata[] = "----\n";
	}
	foreach ($pages as $old=>$new)
	{
		$postdata[] = '-'.decode($old).
			$_rename_messages['msg_arrow'].decode($new)."\n";
	}
	
	// �����ξ��ͤϥ����å����ʤ���
	
	// �ե�����ν񤭹���
	page_write(RENAME_LOGPAGE, join('',$postdata));
	
	//������쥯��
	$page = rename_getvar('page');
	if ($page == '')
	{
		$page = RENAME_LOGPAGE;
	}
	header("Location: $script?".rawurlencode($page));
	die();
}
function rename_getrelated($page)
{
	$related = array();
	$pages = get_existpages();
	$pattern = '/(?:^|\/)'.preg_quote(strip_bracket($page),'/').'(?:\/|$)/';
	foreach ($pages as $name)
	{
		if ($name == $page)
		{
			continue; 
		}
		if (preg_match($pattern,$name))
		{
			$related[] = $name;
		}
	}
	return $related;
}
function rename_getselecttag($page)
{
	global $whatsnew;
	
	$pages =array();
	foreach (get_existpages() as $_page)
	{
		if ($_page == $whatsnew)
		{
			continue; 
		}
		$selected = ($_page == $page) ? ' selected' : '';
		$s_page = htmlspecialchars($_page);
		$pages[$_page] = "<option value=\"$s_page\"$selected>$s_page</option>";
	}
	ksort($pages);
	$list = join("\n ",$pages);
	return <<<EOD
<select name="refer">
 <option value=""></option>
 $list
</select>
EOD;
	return $ret;
}
?>
