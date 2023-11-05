<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: backup.inc.php,v 1.11 2004/03/13 13:48:27 arino Exp $
//
// �Хå����å�
function plugin_backup_action()
{
	global $script,$vars,$do_backup,$hr;
	global $_msg_backuplist,$_msg_diff,$_msg_nowdiff,$_msg_source,$_msg_backup;
	global $_msg_view,$_msg_goto,$_msg_deleted,$_msg_addline,$_msg_delline;
	global $_title_backupdiff,$_title_backupnowdiff,$_title_backupsource;
	global $_title_backup,$_title_pagebackuplist,$_title_backuplist;
	
	if (!$do_backup)
	{
		return;
	}
	
	check_readable($vars['page'], true, true);
	
	$page = $s_page = $r_page = '';
	if (array_key_exists('page',$vars))
	{
		$page   = $vars['page'];
		$s_page = htmlspecialchars($page);
		$r_page = rawurlencode($page);
	}
	$s_age = (array_key_exists('age',$vars) and is_numeric($vars['age'])) ? $vars['age'] : 0;
	$action = $s_action = $r_action = '';
	if (array_key_exists('action',$vars))
	{
		$action = $vars['action'];
		$s_action = htmlspecialchars($action);
		$r_action = rawurlencode($action);
	}
	
	$body = '';
	
	if ($page == '')
	{
		return array('msg'=>$_title_backuplist,'body'=>get_backup_list_all());
	}
	if ($action == 'delete')
	{
		return plugin_backup_delete($page);
	}
	if ($s_age == 0)
	{
		return array(
			'msg'  => $_title_pagebackuplist,
			'body' => get_backup_list($page)
		);
	}
	
	$body  = "<ul>\n";
	$body .= " <li><a href=\"$script?cmd=backup\">$_msg_backuplist</a></li>\n";

	$href = "$script?cmd=backup&amp;page=$r_page&amp;age=$s_age";
	
	if ($action != 'diff' and is_page($page))
	{
		$body .= " <li>".str_replace('$1',"<a href=\"$href&amp;action=diff\">$_msg_diff</a>",$_msg_view)."</li>\n";
	}
	if ($action != 'nowdiff' and is_page($page))
	{
		$body .= " <li>".str_replace('$1',"<a href=\"$href&amp;action=nowdiff\">$_msg_nowdiff</a>",$_msg_view)."</li>\n";
	}
	if ($action != 'source')
	{
		$body .= " <li>".str_replace('$1',"<a href=\"$href&amp;action=source\">$_msg_source</a>",$_msg_view)."</li>\n";
	}
	if ($action != '')
	{
		$body .= " <li>".str_replace('$1',"<a href=\"$href\">$_msg_backup</a>",$_msg_view)."</li>\n";
	}
	
	if (is_page($page))
	{
		$body .= " <li>".str_replace('$1',"<a href=\"$script?$r_page\">$s_page</a>",$_msg_goto)."\n";
	}
	else
	{
		$body .= " <li>".str_replace('$1',$s_page,$_msg_deleted)."\n";
	}

	$backups = get_backup($page);
	if (count($backups) > 0)
	{
		$body .= "  <ul>\n";
		foreach($backups as $age => $val)
		{
			$date = format_date($val['time'],TRUE);
			$body .= ($age == $s_age) ?
				"   <li><em>$age $date</em></li>\n" :
				"   <li><a href=\"$script?cmd=backup&amp;action=$r_action&amp;page=$r_page&amp;age=$age\">$age $date</a></li>\n";
		}
		$body .= "  </ul>\n";
	}
	$body .= " </li>\n";
	$body .= "</ul>\n";
	
	if ($action == 'diff')
	{
		$old = ($s_age > 1) ? join('',$backups[$s_age - 1]['data']) : '';
		$cur = join('',$backups[$s_age]['data']);
		$body .= plugin_backup_diff(do_diff($old,$cur));
		
		return array('msg'=>str_replace('$2',$s_age,$_title_backupdiff),'body'=>$body);
	}
	else if ($s_action == 'nowdiff')
	{
		$old = join('',$backups[$s_age]['data']);
		$cur = join('',get_source($page));
		$body .= plugin_backup_diff(do_diff($old,$cur));
		
		return array('msg'=>str_replace('$2',$s_age,$_title_backupnowdiff),'body'=>$body);
	}
	else if ($s_action == 'source')
	{
		$body .= "<pre>".htmlspecialchars(join('',$backups[$s_age]['data']))."</pre>\n";
		
		return array('msg'=>str_replace('$2',$s_age,$_title_backupsource),'body'=>$body);
	}
	// else
	$body .= "$hr\n".drop_submit(convert_html($backups[$s_age]['data']));
	
	return array('msg'=>str_replace('$2',$s_age,$_title_backup),'body'=>$body);
}
// �Хå����åפ���
function plugin_backup_delete($page)
{
	global $script,$post,$adminpass;
	global $_title_backup_delete,$_msg_backup_deleted,$_msg_backup_delete;
	global $_msg_backup_adminpass,$_btn_delete,$_msg_invalidpass;
	
	if (!backup_file_exists($page))
	{
		return;
	}
	$s_page = htmlspecialchars($page);
	$pass = array_key_exists('pass',$post) ? $post['pass'] : NULL;
	
	if (md5($pass) == $adminpass)
	{
		backup_delete($page);
		return array(
			'msg'  => $_title_backup_delete,
			'body' => str_replace('$1',make_pagelink($page),$_msg_backup_deleted)
		);
	}
	$body = ($pass === NULL) ? '' : "<p><strong>$_msg_invalidpass</strong></p>\n";

	$msg_delete = str_replace('$1',make_pagelink($page),$_msg_backup_delete);
	$body .= <<<EOD
<p>$_msg_backup_adminpass</p>
<form action="$script" method="post">
 <div>
  <input type="hidden" name="cmd" value="backup" />
  <input type="hidden" name="page" value="$s_page" />
  <input type="hidden" name="action" value="delete" />
  <input type="password" name="pass" size="12" />
  <input type="submit" name="ok" value="$_btn_delete" />
 </div>
</form>
EOD;
	return	array('msg'=>$_title_backup_delete,'body'=>$body);
}	
function plugin_backup_diff($str) 
{
	global $_msg_addline,$_msg_delline,$hr;
	
	$str = htmlspecialchars($str);
	$str = preg_replace('/^(\-)(.*)$/m','<span class="diff_removed"> $2</span>',$str);
	$str = preg_replace('/^(\+)(.*)$/m','<span class="diff_added"> $2</span>',$str);
	$str = trim($str);
	$str = <<<EOD
$hr
<ul>
 <li>$_msg_addline</li>
 <li>$_msg_delline</li>
</ul>
<pre>$str</pre>
EOD;
	
	return $str;
}

// �Хå����åװ��������
function get_backup_list($page)
{
	global $script;
	global $_msg_backuplist,$_msg_diff,$_msg_nowdiff,$_msg_source,$_msg_nobackup;
	global $_title_backup_delete;
	
	$r_page = rawurlencode($page);
	$s_page = htmlspecialchars($page);
	$retval = array();
	$retval[0] = <<<EOD
<ul>
 <li><a href="$script?cmd=backup">$_msg_backuplist</a>
  <ul>
EOD;
	$retval[1] = "\n";
	$retval[2] = <<<EOD
  </ul>
 </li>
</ul>
EOD;
	
	$backups = backup_file_exists($page) ? get_backup($page) : array();
	if (count($backups) == 0)
	{
		$msg = str_replace('$1',make_pagelink($page),$_msg_nobackup);
		$retval[1] .= "   <li>$msg</li>\n";
		return join('',$retval);
	}
	$retval[1] .= "   <li><a href=\"$script?cmd=backup&amp;action=delete&amp;page=$r_page\">";
	$retval[1] .= str_replace('$1',$s_page,$_title_backup_delete);
	$retval[1] .= "</a></li>\n";
	foreach ($backups as $age=>$data) {
		$date = format_date($data['time'],TRUE);
		$href = "$script?cmd=backup&amp;page=$r_page&amp;age=$age";
		$retval[1] .= <<<EOD
   <li><a href="$href">$age $date</a>
     [ <a href="$href&amp;action=diff">$_msg_diff</a>
     | <a href="$href&amp;action=nowdiff">$_msg_nowdiff</a>
     | <a href="$href&amp;action=source">$_msg_source</a>
     ]
   </li>
EOD;
	}
	return join('',$retval);
}
// ���ڡ����ΥХå����åװ��������
function get_backup_list_all($withfilename = FALSE)
{
	global $cantedit;
	
	$pages = array_diff(get_existpages(BACKUP_DIR,BACKUP_EXT),$cantedit);
	
	if (count($pages) == 0)
	{
		return '';
	}
	
	return page_list($pages,'backup',$withfilename);
}
?>
