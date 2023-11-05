<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: navi.inc.php,v 1.16 2004/07/31 03:09:20 henoheno Exp $
//

/*

*�ץ饰���� navi
DobBook���Υʥӥ��������С���ɽ������

*Usage
 #navi(page)

*�ѥ�᡼��
-page~
 HOME�Ȥʤ�ڡ�������ά����ȼ��ڡ�����HOME�Ȥ��롣

*ư��

-1���ܤλ���(HOME)~
 �ڡ���������ls.inc.php����ɽ������
-1���ܤλ���(HOME�ʳ�)
 header : �إå���
  prev       next
 -----------------
-2���ܤλ���
 footer : �եå���
 ------------------
  prev  home  next
  title  up  title

*/

// ��������ڡ��� (����ɽ����)
define('NAVI_EXCLUDE_PATTERN','');
#define('NAVI_EXCLUDE_PATTERN','/\/_/');

// <link>��������Ϥ��� (TRUE|FALSE)
define('NAVI_LINK_TAGS',FALSE);

function plugin_navi_convert()
{
	global $vars, $script, $head_tags;
	global $_navi_prev,$_navi_next,$_navi_up,$_navi_home;
	static $navi = array();

	$home = $current = $vars['page'];
	if (func_num_args())
	{
		list($home) = func_get_args();
		$home = strip_bracket($home);
	}
	$is_home = ($home == $current);

	// ���FALSE,2���ܰʹ�TRUE
	$footer = array_key_exists($home,$navi);
	if (!$footer)
	{
		$navi[$home] = array(
			'up'=>'',
			'prev'=>'',
			'prev1'=>'',
			'next'=>'',
			'next1'=>'',
			'home'=>'',
			'home1'=>'',
		);

		$pages = preg_grep('/^'.preg_quote($home,'/').'($|\/)/',get_existpages());
		// preg_grep(,,PREG_GREP_INVERT)���Ȥ���С�
		if (NAVI_EXCLUDE_PATTERN != '')
		{
			$pages = array_diff($pages,preg_grep(NAVI_EXCLUDE_PATTERN,$pages));
		}
		$pages[] = $current; // ��ʼ :)
		$pages = array_unique($pages);
		natcasesort($pages);
		$prev = $home;
		foreach ($pages as $page)
		{
			if ($page == $current)
			{
				break;
			}
			$prev = $page;
		}
		$next = current($pages);

		$pos = strrpos($current, '/');
		$up = '';
		if ($pos > 0)
		{
			$up = substr($current, 0, $pos);
			$navi[$home]['up'] = make_pagelink($up,$_navi_up);
		}
		if (!$is_home)
		{
			$navi[$home]['prev'] = make_pagelink($prev);
			$navi[$home]['prev1'] = make_pagelink($prev,$_navi_prev);
		}
		if ($next != '')
		{
			$navi[$home]['next'] = make_pagelink($next);
			$navi[$home]['next1'] = make_pagelink($next,$_navi_next);
		}
		$navi[$home]['home'] = make_pagelink($home);
		$navi[$home]['home1'] = make_pagelink($home,$_navi_home);

		// <link>�������������� : start next prev(previous) parent(up)
		// ̤�б� : contents(toc) search first(begin) last(end)
		if (NAVI_LINK_TAGS)
		{
			foreach (array('start'=>$home,'next'=>$next,'prev'=>$prev,'up'=>$up) as $rel=>$_page)
			{
				if ($_page != '')
				{
					$s_page = htmlspecialchars($_page);
					$r_page = rawurlencode($_page);
					$head_tags[] = " <link rel=\"$rel\" href=\"$script?$r_page\" title=\"$s_page\" />";
				}
			}
		}
	}

	$ret = '';
	if ($footer) // �եå�
	{
		$ret = <<<EOD
<hr class="full_hr" />
<ul class="navi">
 <li class="navi_left">{$navi[$home]['prev1']}<br />{$navi[$home]['prev']}</li>
 <li class="navi_right">{$navi[$home]['next1']}<br />{$navi[$home]['next']}</li>
 <li class="navi_none">{$navi[$home]['home1']}<br />{$navi[$home]['up']}</li>
</ul>
EOD;
	}
	else if ($is_home) // �ܼ�
	{
		$ret .= '<ul>';
		foreach ($pages as $page)
		{
			if ($page != $home)
			{
				$ret .= ' <li>'.make_pagelink($page).'</li>';
			}
		}
		$ret .= '</ul>';
	}
	else // �إå�
	{
		$ret = <<<EOD
<ul class="navi">
 <li class="navi_left">{$navi[$home]['prev1']}</li>
 <li class="navi_right">{$navi[$home]['next1']}</li>
 <li class="navi_none">{$navi[$home]['home']}</li>
</ul>
<hr class="full_hr" />
EOD;
	}
	return $ret;
}
?>
