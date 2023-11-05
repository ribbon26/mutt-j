<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: ls2.inc.php,v 1.15 2003/07/03 05:21:48 arino Exp $
//

/*
*�ץ饰���� ls2
�۲��Υڡ����θ��Ф�(*,**,***)�ΰ�����ɽ������

*Usage
 #ls2(�ѥ�����[,�ѥ�᡼��])

*�ѥ�᡼��
-�ѥ�����(�ǽ�˻���)~
��ά����Ȥ��⥫��ޤ�ɬ��
-title~
���Ф��ΰ�����ɽ������
-include~
���󥯥롼�ɤ��Ƥ���ڡ����θ��Ф���Ƶ�Ū����󤹤�
-link~
action�ץ饰�����ƤӽФ���󥯤�ɽ��
-reverse~
�ڡ������¤ӽ��ȿž�����߽�ˤ���
-compact~
���Ф���٥��Ĵ������
LS2_LIST_COMPACT��TRUE�λ���̵��(�Ѳ����ʤ�)
*/

//���Ф����󥫡��ν�
define('LS2_CONTENT_HEAD','#content_1_');

//���Ф����󥫡��γ����ֹ�
define('LS2_ANCHOR_ORIGIN',0);

//���Ф���٥��Ĵ������(�ǥե������)
define('LS2_LIST_COMPACT',FALSE);

function plugin_ls2_action()
{
	global $vars;
	global $_ls2_msg_title;
	
	$params = array();
	foreach (array('title','include','reverse') as $key)
	{
		$params[$key] = array_key_exists($key,$vars);
	}
	$prefix = array_key_exists('prefix',$vars) ? $vars['prefix'] : '';
	$body = ls2_show_lists($prefix,$params);
	
	return array(
		'body'=>$body,
		'msg'=>str_replace('$1',htmlspecialchars($prefix),$_ls2_msg_title)
	);
}

function plugin_ls2_convert()
{
	global $script,$vars;
	global $_ls2_msg_title;

	$prefix = '';
	if (func_num_args())
	{
		$args = func_get_args();
		$prefix = array_shift($args);
	}
	else
	{
		$args = array();
	}
	if ($prefix == '')
	{
		$prefix = strip_bracket($vars['page']).'/';
	}

	$params = array(
		'link'    => FALSE,
		'title'   => FALSE,
		'include' => FALSE,
		'reverse' => FALSE,
		'compact' => LS2_LIST_COMPACT,
		'_args'   => array(),
		'_done'   => FALSE
	);
	array_walk($args, 'ls2_check_arg', &$params);
	$title = (count($params['_args']) > 0) ?
		join(',', $params['_args']) :
		str_replace('$1',htmlspecialchars($prefix),$_ls2_msg_title);

	if ($params['link'])
	{
		$tmp = array();
		$tmp[] = 'plugin=ls2&amp;prefix='.$prefix;
		if (isset($params['title']))
		{
			$tmp[] = 'title=1';
		}
		if (isset($params['include']))
		{
			$tmp[] = 'include=1';
		}
		return '<p><a href="'.$script.'?'.join('&amp;',$tmp).'">'.$title.'</a></p>'."\n";
	}
	return ls2_show_lists($prefix,$params);
}
function ls2_show_lists($prefix,&$params)
{
	global $_ls2_err_nopages;
	
	if (strlen($prefix))
	{
		$pages = array();
		foreach (get_existpages() as $_page)
		{
			if (strpos($_page,$prefix) === 0)
			{
				$pages[] = $_page;
			}
		}
	}
	else
	{
		$pages = get_existpages();
	}
	natcasesort($pages);
	
	if ($params['reverse'])
	{
		$pages = array_reverse($pages);
	}
	foreach ($pages as $page)
	{
		$params["page_$page"] = 0;
	}
	if (count($pages) == 0)
	{
		return str_replace('$1',htmlspecialchars($prefix),$_ls2_err_nopages);
	}
	
	$params['result'] = array();
	$params['saved'] = array();
	foreach ($pages as $page)
	{
		ls2_get_headings($page,$params,1);
	}
	return join("\n",$params['result']).join("\n",$params['saved']);
}

function ls2_get_headings($page,&$params,$level,$include = FALSE)
{
	global $script;
	static $_ls2_anchor = 0;
	
	$is_done = (isset($params["page_$page"]) and $params["page_$page"] > 0); //�ڡ�����ɽ���ѤߤΤȤ�True
	
	if (!$is_done)
	{
		$params["page_$page"] = ++$_ls2_anchor;
	}
	
	$r_page = rawurlencode($page);
	$s_page = htmlspecialchars($page);
	$title = $s_page.' '.get_pg_passage($page,FALSE);
	$href = $script.'?cmd=read&amp;page='.$r_page;
	
	ls2_list_push($params,$level);
	$ret = $include ? '<li>include ' : '<li>';
	if ($params['title'] and $is_done)
	{
		$ret .= "<a href=\"$href\" title=\"$title\">$s_page</a> ";
		$ret .= "<a href=\"#list_{$params["page_$page"]}\"><sup>&uarr;</sup></a>";
		array_push($params['result'],$ret);
		return;
	}
	else
	{
		$ret .= "<a id=\"list_{$params["page_$page"]}\" href=\"$href\" title=\"$title\">$s_page</a>";
		array_push($params['result'],$ret);
	}
	
	$anchor = LS2_ANCHOR_ORIGIN;
	foreach (get_source($page) as $line)
	{
		if ($params['title'] and preg_match('/^(\*{1,3})/',$line,$matches))
		{
			$id = make_heading($line);
			$level = strlen($matches[1]);
			$id = LS2_CONTENT_HEAD.$anchor++;
			ls2_list_push($params,$level + strlen($level));
			array_push($params['result'], "<li><a href=\"$href$id\">$line</a>");
		}
		else if ($params['include']
			and preg_match('/^#include\((.+)\)/',$line,$matches) and is_page($matches[1]))
		{
			ls2_get_headings($matches[1],$params,$level + 1,TRUE);
		}
	}
}
//�ꥹ�ȹ�¤���ۤ���
function ls2_list_push(&$params,$level)
{
	global $_ul_left_margin, $_ul_margin, $_list_pad_str;
	
	$result =& $params['result'];
	$saved  =& $params['saved'];
	$cont   = TRUE;
	$open   = "<ul%s>";
	$close  = '</li></ul>';
	
	while (count($saved) > $level or
		(count($saved) > 0 and $saved[0] != $close))
	{
		array_push($result, array_shift($saved));
	}
	
	$margin = $level - count($saved);
	
	while (count($saved) < ($level - 1))
	{
		array_unshift($saved, ''); //count($saved)�����䤹�����dummy
	}
	
	if (count($saved) < $level)
	{
		$cont = FALSE;
		array_unshift($saved, $close);
		
		$left = ($level == $margin) ? $_ul_left_margin : 0;
		if ($params['compact'])
		{
			// �ޡ���������
			$left += $_ul_margin;
			// ��٥����
			$level -= ($margin - 1);
		}
		else
		{
			$left += $margin * $_ul_margin;
		}
		$str = sprintf($_list_pad_str, $level, $left, $left);
		array_push($result, sprintf($open, $str));
	}
	if ($cont)
	{
		array_push($result, '</li>');
	}
}
//���ץ�������Ϥ���
function ls2_check_arg($val, $key, &$params)
{
	if ($val == '')
	{
		$params['_done'] = TRUE;
		return;
	}
	if (!$params['_done'])
	{
		foreach (array_keys($params) as $key)
		{
			if (strpos($key, strtolower($val)) === 0)
			{
				$params[$key] = TRUE;
				return;
			}
		}
		$params['_done'] = TRUE;
	}
	$params['_args'][] = $val;
}
?>
