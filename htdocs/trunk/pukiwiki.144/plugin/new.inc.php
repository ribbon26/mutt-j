<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: new.inc.php,v 1.4 2004/07/31 03:09:20 henoheno Exp $
//

// ���Τ�ɽ���ե����ޥå�
define('NEW_MESSAGE','<span class="comment_date">%s</span>');

function plugin_new_init()
{
	global $_plugin_new_elapses;

	// �в��ÿ� => ����ɽ������
	$messages = array(
		'_plugin_new_elapses' => array(
			1*60*60*24 => ' <span class="new1" title="%s">New!</span>',
			5*60*60*24 => ' <span class="new5" title="%s">New</span>',
		),
	);
	set_plugin_messages($messages);
}
function plugin_new_inline()
{
	global $vars,$_plugin_new_elapses;

	if (func_num_args() < 1)
	{
		return FALSE;
	}
	$retval = '';
	$args = func_get_args();
	$date = strip_htmltag(array_pop($args)); // {}��ʬ�ΰ���
	if ($date != '' and ($timestamp = strtotime($date)) !== -1)
	{
		$nodate = in_array('nodate',$args);
		$timestamp -= ZONETIME;
		$retval = $nodate ? '' : htmlspecialchars($date);
	}
	else
	{
		$name = strip_bracket(count($args) ? array_shift($args) : $vars['page']);
		$page = get_fullname($name,$vars['page']);
		$nolink = in_array('nolink',$args);
		$timestamp = 0;
		if (substr($page,-1) == '/')
		{
			foreach (preg_grep('/^'.preg_quote($page,'/').'/',get_existpages()) as $page)
			{
				$_timestamp = get_filetime($page);
				if ($timestamp < $_timestamp)
				{
					$retval = $nolink ? '' : make_pagelink($page); // �Ǥ⿷�����ڡ�����ɽ��
					$timestamp = $_timestamp;
				}
			}
		}
		else if (is_page($page))
		{
			$retval = $nolink ? '' : make_pagelink($page,$name);
			$timestamp = get_filetime($page);
		}
		if ($timestamp == 0)
		{
			return '';
		}
	}

	$erapse = UTIME - $timestamp;
	foreach ($_plugin_new_elapses as $limit=>$tag)
	{
		if ($erapse <= $limit)
		{
			$retval .= sprintf($tag,get_passage($timestamp));
			break;
		}
	}
	return sprintf(NEW_MESSAGE,$retval);
}
?>
