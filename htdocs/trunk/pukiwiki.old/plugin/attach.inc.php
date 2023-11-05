<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
//  $Id: attach.inc.php,v 1.39 2004/03/18 10:02:13 arino Exp $
//

/*
 �ץ饰���� attach

 changed by Y.MASUI <masui@hisec.co.jp> http://masui.net/pukiwiki/
 modified by PANDA <panda@arino.jp> http://home.arino.jp/
*/

// max file size for upload on PHP(PHP default 2MB)
ini_set('upload_max_filesize','2M');

// max file size for upload on script of PukiWiki(default 1MB)
define('MAX_FILESIZE',1000000);

// �����Ԥ�����ź�եե�����򥢥åץ��ɤǤ���褦�ˤ���
define('ATTACH_UPLOAD_ADMIN_ONLY',FALSE); // FALSE or TRUE
// �����Ԥ�����ź�եե���������Ǥ���褦�ˤ���
define('ATTACH_DELETE_ADMIN_ONLY',FALSE); // FALSE or TRUE
// �����Ԥ�ź�եե������������Ȥ��ϡ��Хå����åפ���ʤ�
// ATTACH_DELETE_ADMIN_ONLY=TRUE�ΤȤ�ͭ��
define('ATTACH_DELETE_ADMIN_NOBACKUP',FALSE); // FALSE or TRUE

// ���åץ���/������˥ѥ���ɤ��׵᤹��(ADMIN_ONLY��ͥ��)
define('ATTACH_PASSWORD_REQUIRE',FALSE); // FALSE or TRUE

// �ե�����Υ���������
define('ATTACH_FILE_MODE',0644);
//define('ATTACH_FILE_MODE',0604); // for XREA.COM

// file icon image
if (!defined('FILE_ICON'))
{
	define('FILE_ICON','<img src="./image/file.png" width="20" height="20" alt="file" style="border-width:0px" />');
}

// mime-type�򵭽Ҥ����ڡ���
define('ATTACH_CONFIG_PAGE_MIME','plugin/attach/mime-type');

//-------- convert
function plugin_attach_convert()
{
	global $vars;
	
	if (!ini_get('file_uploads'))
	{
		return 'file_uploads disabled';
	}
	
	$nolist = $noform = FALSE;
	
	if (func_num_args() > 0)
	{
		foreach (func_get_args() as $arg)
		{
			$arg = strtolower($arg);
			$nolist |= ($arg == 'nolist');
			$noform |= ($arg == 'noform');
		}
	}
	$ret = '';
	if (!$nolist)
	{
		$obj = &new AttachPages($vars['page']);
		$ret .= $obj->toString($vars['page'],TRUE);
	}
	if (!$noform)
	{
		$ret .= attach_form($vars['page']);
	}
	
	return $ret;
}

//-------- action
function plugin_attach_action()
{
	global $vars;
	
	// backward compatible
	if (array_key_exists('openfile',$vars))
	{
		$vars['pcmd'] = 'open';
		$vars['file'] = $vars['openfile'];
	}
	if (array_key_exists('delfile',$vars))
	{
		$vars['pcmd'] = 'delete';
		$vars['file'] = $vars['delfile'];
	}
	
	$age = array_key_exists('age',$vars) ? $vars['age'] : 0;
	$pcmd = array_key_exists('pcmd',$vars) ? $vars['pcmd'] : '';
	
	// Authentication
	if (array_key_exists('refer',$vars) and is_pagename($vars['refer']))
	{
		$read_cmds = array('info','open','list');
		in_array($pcmd,$read_cmds) ?
			check_readable($vars['refer']) : check_editable($vars['refer']);
	}
	
	// Upload
	if (array_key_exists('attach_file',$_FILES))
	{
		$pass = array_key_exists('pass',$vars) ? md5($vars['pass']) : NULL;
		return attach_upload($_FILES['attach_file'],$vars['refer'],$pass);
	}
	
	switch ($pcmd)
	{
		case 'info':    return attach_info();
		case 'delete':  return attach_delete();
		case 'open':    return attach_open();
		case 'list':    return attach_list();
		case 'freeze':  return attach_freeze(TRUE);
		case 'unfreeze':return attach_freeze(FALSE);
		case 'upload':  return attach_showform();
	}
	if ($vars['page'] == '' or !is_page($vars['page']))
	{
		return attach_list();
	}
	
	return attach_showform();
}
//-------- call from skin
function attach_filelist()
{
	global $vars,$_attach_messages;
	
	$obj = &new AttachPages($vars['page'],0);

	if (!array_key_exists($vars['page'],$obj->pages))
	{
		return '';
	}
	return $_attach_messages['msg_file'].': '.$obj->toString($vars['page'],TRUE)."\n";
}
//-------- ����
//�ե����륢�åץ���
function attach_upload($file,$page,$pass=NULL)
{
// $pass=NULL : �ѥ���ɤ����ꤵ��Ƥ��ʤ�
// $pass=TRUE : ���åץ��ɵ���
	global $adminpass,$_attach_messages;
	
	if ($file['tmp_name'] == '' or !is_uploaded_file($file['tmp_name']))
	{
		return array('result'=>FALSE);
	}		
	if ($file['size'] > MAX_FILESIZE)
	{
		return array('result'=>FALSE,'msg'=>$_attach_messages['err_exceed']);
	}
	if (!is_pagename($page) or ($pass !== TRUE and !is_editable($page)))
	{
		return array('result'=>FALSE,'msg'=>$_attach_messages['err_noparm']);
	}
	if (ATTACH_UPLOAD_ADMIN_ONLY and $pass !== TRUE
		and ($pass === NULL or $pass != $adminpass))
	{
		return array('result'=>FALSE,'msg'=>$_attach_messages['err_adminpass']);
	}
	
	$obj = &new AttachFile($page,$file['name']);	
	
	if ($obj->exist)
	{
		return array('result'=>FALSE,'msg'=>$_attach_messages['err_exists']);
	}
	if (move_uploaded_file($file['tmp_name'],$obj->filename))
	{
		chmod($obj->filename,ATTACH_FILE_MODE);
	}
	
	if (is_page($page))
	{
		touch(get_filename($page));
	}
	
	$obj->getstatus();
	$obj->status['pass'] = ($pass !== TRUE and $pass !== NULL) ? $pass : '';
	$obj->putstatus();

	return array('result'=>TRUE,'msg'=>$_attach_messages['msg_uploaded']);
}
//�ܺ٥ե������ɽ��
function attach_info($err='')
{
	global $vars,$_attach_messages;
	
	foreach (array('refer','file','age') as $var)
	{
		$$var = array_key_exists($var,$vars) ? $vars[$var] : '';
	}
	
	$obj = &new AttachFile($refer,$file,$age);
	return $obj->getstatus() ? $obj->info($err) : array('msg'=>$_attach_messages['err_notfound']);
}
//���
function attach_delete()
{
	global $vars,$_attach_messages;
	
	foreach (array('refer','file','age','pass') as $var)
	{
		$$var = array_key_exists($var,$vars) ? $vars[$var] : '';
	}
	
	if (is_freeze($refer) or !is_editable($refer))
	{
		return array('msg'=>$_attach_messages['err_noparm']);
	}
	
	$obj = &new AttachFile($refer,$file,$age);
	return $obj->getstatus() ? $obj->delete($pass) : array('msg'=>$_attach_messages['err_notfound']);
}
//���
function attach_freeze($freeze)
{
	global $vars,$_attach_messages;
	
	foreach (array('refer','file','age','pass') as $var)
	{
		$$var = array_key_exists($var,$vars) ? $vars[$var] : '';
	}
	
	if (is_freeze($refer) or !is_editable($refer))
	{
		return array('msg'=>$_attach_messages['err_noparm']);
	}
	
	$obj = &new AttachFile($refer,$file,$age);
	return $obj->getstatus() ? $obj->freeze($freeze,$pass) : array('msg'=>$_attach_messages['err_notfound']);
}
//���������
function attach_open()
{
	global $vars,$_attach_messages;
	
	foreach (array('refer','file','age') as $var)
	{
		$$var = array_key_exists($var,$vars) ? $vars[$var] : '';
	}
	
	$obj = &new AttachFile($refer,$file,$age);
	return $obj->getstatus() ? $obj->open() : array('msg'=>$_attach_messages['err_notfound']);
}
//��������
function attach_list()
{
	global $vars;
	global $_attach_messages;
	
	$refer = array_key_exists('refer',$vars) ? $vars['refer'] : '';
	
	$obj = &new AttachPages($refer);
	
	$msg = $_attach_messages[$refer == '' ? 'msg_listall' : 'msg_listpage'];
	$body = ($refer == '' or array_key_exists($refer,$obj->pages)) ?
		$obj->toString($refer,FALSE) :
		$_attach_messages['err_noexist'];
	return array('msg'=>$msg,'body'=>$body);
}
//���åץ��ɥե������ɽ��
function attach_showform()
{
	global $vars;
	global $_attach_messages;
	
	$vars['refer'] = $vars['page'];
	$body = ini_get('file_uploads') ? attach_form($vars['page']) : 'file_uploads disabled.';
	
	return array('msg'=>$_attach_messages['msg_upload'],'body'=>$body);
}

//-------- �����ӥ�
//mime-type�η���
function attach_mime_content_type($filename)
{
	$type = 'application/octet-stream'; //default
	
	if (!file_exists($filename))
	{
		return $type;
	}
	$size = @getimagesize($filename);
	if (is_array($size))
	{
		switch ($size[2])
		{
			case 1:
				return 'image/gif';
			case 2:
				return 'image/jpeg';
			case 3:
				return 'image/png';
			case 4:
				return 'application/x-shockwave-flash';
		}
	}
	
	if (!preg_match('/_((?:[0-9A-F]{2})+)(?:\.\d+)?$/',$filename,$matches))
	{
		return $type;
	}
	$filename = decode($matches[1]);
	
	// mime-type����ɽ�����
	$config = new Config(ATTACH_CONFIG_PAGE_MIME);
	$table = $config->read() ? $config->get('mime-type') : array();
	unset($config); // ��������
	
	foreach ($table as $row)
	{
		$_type = trim($row[0]);
		$exts = preg_split('/\s+|,/',trim($row[1]),-1,PREG_SPLIT_NO_EMPTY);
		
		foreach ($exts as $ext)
		{
			if (preg_match("/\.$ext$/i",$filename))
			{
				return $_type;
			}
		}
	}
	
	return $type;
}
//���åץ��ɥե�����
function attach_form($page)
{
	global $script,$vars;
	global $_attach_messages;
	
	$r_page = rawurlencode($page);
	$s_page = htmlspecialchars($page);
	$navi = <<<EOD
  <span class="small">
   [<a href="$script?plugin=attach&amp;pcmd=list&amp;refer=$r_page">{$_attach_messages['msg_list']}</a>]
   [<a href="$script?plugin=attach&amp;pcmd=list">{$_attach_messages['msg_listall']}</a>]
  </span><br />
EOD;

	if (!(bool)ini_get('file_uploads'))
	{
		return $navi;
	}
	
	$maxsize = MAX_FILESIZE;
	$msg_maxsize = sprintf($_attach_messages['msg_maxsize'],number_format($maxsize/1000)."KB");

	$pass = '';
	if (ATTACH_PASSWORD_REQUIRE or ATTACH_UPLOAD_ADMIN_ONLY)
	{
		$title = $_attach_messages[ATTACH_UPLOAD_ADMIN_ONLY ? 'msg_adminpass' : 'msg_password'];
		$pass = '<br />'.$title.': <input type="password" name="pass" size="8" />';
	}
	return <<<EOD
<form enctype="multipart/form-data" action="$script" method="post">
 <div>
  <input type="hidden" name="plugin" value="attach" />
  <input type="hidden" name="pcmd" value="post" />
  <input type="hidden" name="refer" value="$s_page" />
  <input type="hidden" name="max_file_size" value="$maxsize" />
  $navi
  <span class="small">
   $msg_maxsize
  </span><br />
  {$_attach_messages['msg_file']}: <input type="file" name="attach_file" />
  $pass
  <input type="submit" value="{$_attach_messages['btn_upload']}" />
 </div>
</form>
EOD;
}
//-------- ���饹
//�ե�����
class AttachFile
{
	var $page,$file,$age,$basename,$filename,$logname;
	var $time = 0;
	var $size = 0;
	var $time_str = '';
	var $size_str = '';
	var $status = array('count'=>array(0),'age'=>'','pass'=>'','freeze'=>FALSE);
	
	function AttachFile($page,$file,$age=0)
	{
		$this->page = $page;
		$this->file = basename($file);
		$this->age = is_numeric($age) ? $age : 0;
		
		$this->basename = UPLOAD_DIR.encode($page).'_'.encode($this->file);
		$this->filename = $this->basename . ($age ? '.'.$age : '');
		$this->logname = $this->basename.'.log';
		$this->exist = file_exists($this->filename);
		$this->time = $this->exist ? filemtime($this->filename) - LOCALZONE : 0;
		$this->md5hash = $this->exist ? md5_file($this->filename) : '';
	}
	// �ե�����������
	function getstatus()
	{
		if (!$this->exist)
		{
			return FALSE;
		}
		// ���ե��������
		if (file_exists($this->logname))
		{
			$data = file($this->logname);
			foreach ($this->status as $key=>$value)
			{
				$this->status[$key] = chop(array_shift($data));
			}
			$this->status['count'] = explode(',',$this->status['count']);
		}
		$this->time_str = get_date('Y/m/d H:i:s',$this->time);
		$this->size = filesize($this->filename);
		$this->size_str = sprintf('%01.1f',round($this->size)/1000,1).'KB';
		$this->type = attach_mime_content_type($this->filename);
		
		return TRUE;
	}
	//���ơ�������¸
	function putstatus()
	{
		$this->status['count'] = join(',',$this->status['count']);
		$fp = fopen($this->logname,'wb')
			or die_message('cannot write '.$this->logname);
		set_file_buffer($fp, 0);
		flock($fp,LOCK_EX);
		rewind($fp);
		foreach ($this->status as $key=>$value)
		{
			fwrite($fp,$value."\n");
		}
		flock($fp,LOCK_UN);
		fclose($fp);
	}
	// ���դ���Ӵؿ�
	function datecomp($a,$b)
	{
		return ($a->time == $b->time) ? 0 : (($a->time > $b->time) ? -1 : 1);
	}
	function toString($showicon,$showinfo)
	{
		global $script,$date_format,$time_format,$weeklabels;
		global $_attach_messages;
		
		$this->getstatus();
		$param  = '&amp;file='.rawurlencode($this->file).'&amp;refer='.rawurlencode($this->page).
			($this->age ? '&amp;age='.$this->age : '');
		$title = $this->time_str.' '.$this->size_str;
		$label = ($showicon ? FILE_ICON : '').htmlspecialchars($this->file);
		if ($this->age)
		{
			$label .= ' (backup No.'.$this->age.')';
		}
		$info = $count = '';
		if ($showinfo)
		{
			$_title = str_replace('$1',rawurlencode($this->file),$_attach_messages['msg_info']);
			$info = "\n<span class=\"small\">[<a href=\"$script?plugin=attach&amp;pcmd=info$param\" title=\"$_title\">{$_attach_messages['btn_info']}</a>]</span>";
			$count = ($showicon and !empty($this->status['count'][$this->age])) ?
				sprintf($_attach_messages['msg_count'],$this->status['count'][$this->age]) : '';
		}
		return "<a href=\"$script?plugin=attach&amp;pcmd=open$param\" title=\"$title\">$label</a>$count$info";
	}
	// ����ɽ��
	function info($err)
	{
		global $script,$_attach_messages;
		
		$r_page = rawurlencode($this->page);
		$s_page = htmlspecialchars($this->page);
		$s_file = htmlspecialchars($this->file);
		$s_err = ($err == '') ? '' : '<p style="font-weight:bold">'.$_attach_messages[$err].'</p>';
		
		if ($this->age)
		{
			$msg_freezed = '';
			$msg_delete  = '<input type="radio" name="pcmd" value="delete" />'.$_attach_messages['msg_delete'];
			$msg_delete .= $_attach_messages['msg_require'];
			$msg_delete .= '<br />';
			$msg_freeze  = '';
		}
		else
		{
			if ($this->status['freeze'])
			{
				$msg_freezed = "<dd>{$_attach_messages['msg_isfreeze']}</dd>";
				$msg_delete = '';
				$msg_freeze  = '<input type="radio" name="pcmd" value="unfreeze" />'.$_attach_messages['msg_unfreeze'];
				$msg_freeze .= $_attach_messages['msg_require'].'<br />';
			}
			else
			{
				$msg_freezed = '';
				$msg_delete = '<input type="radio" name="pcmd" value="delete" />'.$_attach_messages['msg_delete'];
				if (ATTACH_DELETE_ADMIN_ONLY or $this->age)
				{
					$msg_delete .= $_attach_messages['msg_require'];
				}
				$msg_delete .= '<br />';
				$msg_freeze  = '<input type="radio" name="pcmd" value="freeze" />'.$_attach_messages['msg_freeze'];
				$msg_freeze .= "{$_attach_messages['msg_require']}<br />";
			}
		}
		$info = $this->toString(TRUE,FALSE);
		
		$retval = array('msg'=>sprintf($_attach_messages['msg_info'],htmlspecialchars($this->file)));
		$retval['body'] = <<< EOD
<p class="small">
 [<a href="$script?plugin=attach&amp;pcmd=list&amp;refer=$r_page">{$_attach_messages['msg_list']}</a>]
 [<a href="$script?plugin=attach&amp;pcmd=list">{$_attach_messages['msg_listall']}</a>]
</p>
<dl>
 <dt>$info</dt>
 <dd>{$_attach_messages['msg_page']}:$s_page</dd>
 <dd>{$_attach_messages['msg_filename']}:{$this->filename}</dd>
 <dd>{$_attach_messages['msg_md5hash']}:{$this->md5hash}</dd>
 <dd>{$_attach_messages['msg_filesize']}:{$this->size_str} ({$this->size} bytes)</dd>
 <dd>Content-type:{$this->type}</dd>
 <dd>{$_attach_messages['msg_date']}:{$this->time_str}</dd>
 <dd>{$_attach_messages['msg_dlcount']}:{$this->status['count'][$this->age]}</dd>
 $msg_freezed
</dl>
<hr />
$s_err
<form action="$script" method="post">
 <div>
  <input type="hidden" name="plugin" value="attach" />
  <input type="hidden" name="refer" value="$s_page" />
  <input type="hidden" name="file" value="$s_file" />
  <input type="hidden" name="age" value="{$this->age}" />
  $msg_delete
  $msg_freeze
  {$_attach_messages['msg_password']}: <input type="password" name="pass" size="8" />
  <input type="submit" value="{$_attach_messages['btn_submit']}" />
 </div>
</form>
EOD;
		return $retval;
	}
	function delete($pass)
	{
		global $adminpass,$_attach_messages;
				
		if ($this->status['freeze'])
		{
			return attach_info('msg_isfreeze');
		}
		
		if (md5($pass) != $adminpass)
		{
			if (ATTACH_DELETE_ADMIN_ONLY or $this->age)
			{
				return attach_info('err_adminpass');
			}
			else if (ATTACH_PASSWORD_REQUIRE and md5($pass) != $this->status['pass'])
			{
				return attach_info('err_password');
			}
		}
		//�Хå����å�
		if ($this->age or
			(ATTACH_DELETE_ADMIN_ONLY and ATTACH_DELETE_ADMIN_NOBACKUP))
		{
			@unlink($this->filename);
		}
		else
		{
			do
			{
				$age = ++$this->status['age'];
			}
			while (file_exists($this->basename.'.'.$age));
			
			if (!rename($this->basename,$this->basename.'.'.$age))
			{
				// ������� why?
				return array('msg'=>$_attach_messages['err_delete']);
			}
			
			$this->status['count'][$age] = $this->status['count'][0];
			$this->status['count'][0] = 0;
			$this->putstatus();
		}
		if (is_page($this->page))
		{
			touch(get_filename($this->page));
		}
		
		return array('msg'=>$_attach_messages['msg_deleted']);
	}
	function freeze($freeze,$pass)
	{
		global $adminpass;
		
		if (md5($pass) != $adminpass)
		{
			return attach_info('err_adminpass');
		}
		
		$this->getstatus();
		$this->status['freeze'] = $freeze;
		$this->putstatus();
		
		return array('msg'=>$_attach_messages[$freeze ? 'msg_freezed' : 'msg_unfreezed']);
	}
	function open()
	{
		$this->getstatus();
		$this->status['count'][$this->age]++;
		$this->putstatus();
		
		// for japanese (???)
		$filename = htmlspecialchars(mb_convert_encoding($this->file,'SJIS','auto'));
		
		ini_set('default_charset','');
		mb_http_output('pass');
		
		header('Content-Disposition: inline; filename="'.$filename.'"');
		header('Content-Length: '.$this->size);
		header('Content-Type: '.$this->type);
		@readfile($this->filename);
		exit;
	}
}

// �ե����륳��ƥ�
class AttachFiles
{
	var $page;
	var $files = array();
	
	function AttachFiles($page)
	{
		$this->page = $page;
	}
	function add($file,$age)
	{
		$this->files[$file][$age] = &new AttachFile($this->page,$file,$age);
	}
	// �ե�������������
	function toString($flat)
	{
		global $_title_cannotread;
		
		if (!check_readable($this->page,FALSE,FALSE))
		{
			return str_replace('$1',make_pagelink($this->page),$_title_cannotread);
		}
		if ($flat)
		{
			return $this->to_flat();
		}	
		$ret = '';
		$files = array_keys($this->files);
		sort($files);
		foreach ($files as $file)
		{
			$_files = array();
			foreach (array_keys($this->files[$file]) as $age)
			{
				$_files[$age] = $this->files[$file][$age]->toString(FALSE,TRUE);
			}
			if (!array_key_exists(0,$_files))
			{
				$_files[0] = htmlspecialchars($file);
			}
			ksort($_files);
			$_file = $_files[0];
			unset($_files[0]);
			$ret .= " <li>$_file\n";
			if (count($_files))
			{
				$ret .= "<ul>\n<li>".join("</li>\n<li>",$_files)."</li>\n</ul>\n";
			}
			$ret .= " </li>\n";
		}
		return make_pagelink($this->page)."\n<ul>\n$ret</ul>\n";
	}
	// �ե�������������(inline)
	function to_flat()
	{
		$ret = '';
		$files = array();
		foreach (array_keys($this->files) as $file)
		{
			if (array_key_exists(0,$this->files[$file]))
			{
				$files[$file] = &$this->files[$file][0];
			}
		}
		uasort($files,array('AttachFile','datecomp'));
		foreach (array_keys($files) as $file)
		{
			$ret .= $files[$file]->toString(TRUE,TRUE).' ';
		}
		
		return $ret;
	}
}
// �ڡ�������ƥ�
class AttachPages
{
	var $pages = array();
	
	function AttachPages($page='',$age=NULL)
	{

		$dir = opendir(UPLOAD_DIR)
			or die('directory '.UPLOAD_DIR.' is not exist or not readable.');
		
		$page_pattern = ($page == '') ? '(?:[0-9A-F]{2})+' : preg_quote(encode($page),'/');
		$age_pattern = ($age === NULL) ?
			'(?:\.([0-9]+))?' : ($age ?  "\.($age)" : '');
		$pattern = "/^({$page_pattern})_((?:[0-9A-F]{2})+){$age_pattern}$/";
		
		while ($file = readdir($dir))
		{
			if (!preg_match($pattern,$file,$matches))
			{
				continue;
			}
			$_page = decode($matches[1]);
			$_file = decode($matches[2]);
			$_age = array_key_exists(3,$matches) ? $matches[3] : 0;
			if (!array_key_exists($_page,$this->pages))
			{
				$this->pages[$_page] = &new AttachFiles($_page);
			}
			$this->pages[$_page]->add($_file,$_age);
		}
		closedir($dir);
	}
	function toString($page='',$flat=FALSE)
	{
		if ($page != '')
		{
			if (!array_key_exists($page,$this->pages))
			{
				return '';
			}
			return $this->pages[$page]->toString($flat);
		}
		$ret = '';
		$pages = array_keys($this->pages);
		sort($pages);
		foreach ($pages as $page)
		{
			$ret .= '<li>'.$this->pages[$page]->toString($flat)."</li>\n";
		}
		return "\n<ul>\n".$ret."</ul>\n";
		
	}
}		
?>
