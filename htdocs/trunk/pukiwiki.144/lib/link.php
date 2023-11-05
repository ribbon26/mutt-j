<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: link.php,v 1.1 2004/08/01 01:54:35 henoheno Exp $
//

/*
 * �ǡ�������
 * CACHE_DIR/encode(�ڡ���̾).ref
 * ���ȸ��ڡ���̾<tab>AutoLink�ˤ���󥯤ΤߤΤȤ�1\n
 * ���ȸ��ڡ���̾<tab>AutoLink�ˤ���󥯤ΤߤΤȤ�1\n
 * ...
 *
 * CACHE_DIR/encode(�ڡ���̾).rel
 * ������ڡ���̾<tab>������ڡ���̾<tab>...
 *
 */

// �ǡ����١��������Ϣ�ڡ���������
function links_get_related_db($page)
{
	$links = array();
	$ref_name = CACHE_DIR.encode($page).'.ref';
	if (file_exists($ref_name))
	{
		foreach (file($ref_name) as $line)
		{
			list($_page) = explode("\t",rtrim($line));
			$links[$_page] = get_filetime($_page);
		}
	}
	return $links;
}
//�ڡ����δ�Ϣ�򹹿�����
function links_update($page)
{
	if (ini_get('safe_mode') == '0') set_time_limit(0);

	$time = is_page($page,TRUE) ? get_filetime($page) : 0;

	$rel_old = array();
	$rel_file = CACHE_DIR.encode($page).'.rel';
	if ($rel_file_exist = file_exists($rel_file))
	{
		$lines = file($rel_file);
		if (array_key_exists(0,$lines))
		{
			$rel_old = explode("\t",rtrim($lines[0]));
		}
		unlink($rel_file);
	}
	$rel_new = array(); // ������
	$rel_auto = array(); // �����ȥ�󥯤��Ƥ��뻲����
	$links = links_get_objects($page,TRUE);
	foreach ($links as $_obj)
	{
		if (!isset($_obj->type) or $_obj->type != 'pagename' or $_obj->name == $page or $_obj->name == '')
		{
			continue;
		}
		if (is_a($_obj,'Link_autolink')) // �Ե�������
		{
			$rel_auto[] = $_obj->name;
		}
		else
		{
			$rel_new[] = $_obj->name;
		}
	}
	$rel_new = array_unique($rel_new);
	// autolink���������Ƥ��ʤ��ڡ���
	$rel_auto = array_diff(array_unique($rel_auto),$rel_new);
	// ���Ƥλ�����ڡ���
	$rel_new = array_merge($rel_new,$rel_auto);

	// .rel:$page�����Ȥ��Ƥ���ڡ����ΰ���
	if ($time) // �ڡ�����¸�ߤ��Ƥ���
	{
		if (count($rel_new))
		{
    		$fp = fopen($rel_file,'w')
    			or die_message('cannot write '.htmlspecialchars($rel_file));
			fputs($fp,join("\t",$rel_new));
			fclose($fp);
		}
	}
	// .ref:$_page�򻲾Ȥ��Ƥ���ڡ����ΰ���
	links_add($page,array_diff($rel_new,$rel_old),$rel_auto);
	links_delete($page,array_diff($rel_old,$rel_new));

	global $WikiName,$autolink,$nowikiname,$search_non_list;
	// $page�������������줿�ڡ����ǡ�AutoLink���оݤȤʤ�������
	if ($time and !$rel_file_exist and $autolink
		and (preg_match("/^$WikiName$/",$page) ? $nowikiname : strlen($page) >= $autolink))
	{
		// $page�򻲾Ȥ��Ƥ������ʥڡ�������ƹ�������(����)
		$search_non_list = 1;
		$pages = do_search($page,'AND',TRUE);
		foreach ($pages as $_page)
		{
			if ($_page != $page)
			{
				links_update($_page);
			}
		}
	}
	$ref_file = CACHE_DIR.encode($page).'.ref';
	//$page��������줿�Ȥ��ˡ�
	if (!$time and file_exists($ref_file))
	{
		foreach (file($ref_file) as $line)
		{
			list($ref_page,$ref_auto) = explode("\t",rtrim($line));
			//$page��AutoLink�Ǥ������Ȥ��Ƥ��ʤ��ڡ�������ƹ�������(��������)
			if ($ref_auto)
			{
				links_delete($ref_page,array($page));
			}
		}
	}
}
//�ڡ����δ�Ϣ����������
function links_init()
{
	global $whatsnew;

	if (ini_get('safe_mode') == '0') set_time_limit(0);

	// �ǡ����١����ν����
	foreach (get_existfiles(CACHE_DIR,'.ref') as $cache)
	{
		unlink($cache);
	}
	foreach (get_existfiles(CACHE_DIR,'.rel') as $cache)
	{
		unlink($cache);
	}
	$pages = get_existpages();
	$ref = array(); // ���ȸ�
	$ref_notauto = array();
	foreach ($pages as $page)
	{
		if ($page == $whatsnew)
		{
			continue;
		}
		$rel = array(); // ������
		$links = links_get_objects($page);
		foreach ($links as $_obj)
		{
			if (!isset($_obj->type) or $_obj->type != 'pagename' or $_obj->name == $page or $_obj->name == '')
			{
				continue;
			}
			$rel[] = $_obj->name;
			if (!is_a($_obj,'Link_autolink'))
			{
				$ref_notauto[$_obj->name][$page] = TRUE;
			}
			$ref[$_obj->name][] = $page;
		}
		$rel = array_unique($rel);
		if (count($rel))
		{
			$fp = fopen(CACHE_DIR.encode($page).'.rel','w')
				or die_message('cannot write '.htmlspecialchars(CACHE_DIR.encode($page).'.rel'));
			fputs($fp,join("\t",$rel));
			fclose($fp);
		}
	}

	foreach ($ref as $page=>$arr)
	{
		if (count($arr) == 0)
		{
			continue;
		}
		$arr = array_unique($arr);
		$fp = fopen(CACHE_DIR.encode($page).'.ref','w')
			or die_message('cannot write '.htmlspecialchars(CACHE_DIR.encode($page).'.ref'));
		foreach ($arr as $ref_page)
		{
			$ref_auto = (array_key_exists($page,$ref_notauto)
				and array_key_exists($ref_page,$ref_notauto[$page])) ? 0 : 1;
			fputs($fp,"$ref_page\t$ref_auto\n");
		}
		fclose($fp);
	}
}
function links_add($page,$add,$rel_auto)
{
	$rel_auto = array_flip($rel_auto);
	foreach ($add as $_page)
	{
		$all_auto = array_key_exists($_page,$rel_auto);
		$is_page = is_page($_page);
		$ref = "$page\t".($all_auto ? 1 : 0)."\n";

		$ref_file = CACHE_DIR.encode($_page).'.ref';
		if (file_exists($ref_file))
		{
			foreach (file($ref_file) as $line)
			{
				list($ref_page,$ref_auto) = explode("\t",rtrim($line));
				if (!$ref_auto)
				{
					$all_auto = FALSE;
				}
				if ($ref_page != $page)
				{
					$ref .= $line;
				}
			}
			unlink($ref_file);
		}
		if ($is_page or !$all_auto)
		{
			$fp = fopen($ref_file,'w')
				 or die_message('cannot write '.htmlspecialchars($ref_file));
			fputs($fp,$ref);
			fclose($fp);
		}
	}
}
function links_delete($page,$del)
{
	foreach ($del as $_page)
	{
		$all_auto = TRUE;
		$is_page = is_page($_page);

		$ref_file = CACHE_DIR.encode($_page).'.ref';
		if (!file_exists($ref_file))
		{
			continue;
		}
		$ref = '';
		foreach (file($ref_file) as $line)
		{
			list($ref_page,$ref_auto) = explode("\t",rtrim($line));
			if ($ref_page != $page)
			{
				if (!$ref_auto)
				{
					$all_auto = FALSE;
				}
				$ref .= $line;
			}
		}
		unlink($ref_file);
		if ($is_page and !$all_auto and $ref != '')
		{
			$fp = fopen($ref_file,'w')
				or die_message('cannot write '.htmlspecialchars($ref_file));
			fputs($fp,$ref);
			fclose($fp);
		}
	}
}
function &links_get_objects($page,$refresh=FALSE)
{
	static $obj;

	if (!isset($obj) or $refresh)
	{
		$obj = &new InlineConverter(NULL,array('note'));
	}

	return $obj->get_objects(join('',preg_grep('/^(?!\/\/|\s)./',get_source($page))),$page);
}
?>
