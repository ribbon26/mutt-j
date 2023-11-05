<?php
/*
 * PukiWiki calendar_viewer�ץ饰����
 * $Id: calendar_viewer.inc.php,v 1.25 2004/08/11 14:19:48 henoheno Exp $
 * calendarrecent�ץ饰����򸵤˺���
 */

/**
 * ����
  calendar�ץ饰�����calendar2�ץ饰����Ǻ��������ڡ��������ɽ�����뤿��Υץ饰����Ǥ���

 * �Ȥ���
    #calendar_viewer(pagename,(this|yyyy-mm|n|x*y),[mode],[separater])

 ** pagename
  calendar or calendar2�ץ饰����򵭽Ҥ��Ƥ�ڡ���̾

 ** (yyyy-mm|n|this)
  - this
  -- ����Υڡ��������ɽ��
  - yyyy-mm
  -- yyyy-mm�ǻ��ꤷ��ǯ��Υڡ��������ɽ��
  - n
  -- ��Ƭ����n������ɽ��
  - x*n
  -- ��Ƭ�������� x �ڡ�����(��Ƭ��0)���顢y��Ťİ���ɽ��

 ** [mode]
  ��ά��ǽ�Ǥ�����ά���Υǥե���Ȥ�past
  - past
  -- ���������Υڡ����ΰ���ɽ���⡼�ɡ������������������
  - future
  -- �����ʹߤΥڡ����ΰ���ɽ���⡼�ɡ����٥��ͽ��䥹�����塼�����
  - view
  -- ����̤��ؤΰ���ɽ���⡼�ɡ�ɽ���޻ߤ���ڡ����Ϥ���ޤ���

 ** [separater]
  - ǯ��������ڤ륻�ѥ졼������ꡣ
  - ��ά��ǽ���ǥե���Ȥ�-����calendar2�ʤ��ά��OK��

 * TODO
  past or future �Ƿ�ñ��ɽ������Ȥ��ˡ����줾�������ΰ����ؤΥ�󥯤�ɽ�����ʤ��褦�ˤ���

 */

function plugin_calendar_viewer_convert()
{
	global $vars, $get, $post, $hr, $script;
	global $_err_calendar_viewer_param, $_err_calendar_viewer_param2;
	global $_msg_calendar_viewer_right, $_msg_calendar_viewer_left;
	global $_msg_calendar_viewer_restrict;

	static $viewed = array();

	// �����γ�ǧ
	if (func_num_args() < 2)
		return '#calendar_viewer(): ' . $_err_calendar_viewer_param . '<br />';

	$func_args = func_get_args();

	// �ǥե������
	$pagename    = $func_args[0];	// ���Ȥʤ�ڡ���̾
	$page_YM     = '';	// ����ɽ������ǯ��
	$limit_base  = 0;	// ��Ƭ��������Ʋ��ڡ����ܤ���ɽ�����뤫 (��Ƭ)
	$limit_pitch = 0;	// ����Ť�ɽ�����뤫
	$limit_page  = 0;	// ����������ڡ�����
	$mode        = 'past';	// ư��⡼��
	$date_sep    = '-';	// ���դΥ��ѥ졼�� calendar2�ʤ� '-', calendar�ʤ� ''

	// $func_args[1] �Υ����å�
	if (preg_match('/[0-9]{4}' . $date_sep . '[0-9]{2}/', $func_args[1])) {
		// ����ǯ��ΰ���ɽ��
		$page_YM     = $func_args[1];
		$limit_page  = 31;
	} else if (preg_match('/this/si', $func_args[1])) {
		// ����ΰ���ɽ��
		$page_YM     = get_date('Y' . $date_sep . 'm');
		$limit_page  = 31;
	} else if (preg_match('/^[0-9]+$/', $func_args[1])) {
		// n��ʬɽ��
		$limit_pitch = $func_args[1];
		$limit_page  = $func_args[1];
	} else if (preg_match('/(-?[0-9]+)\*([0-9]+)/', $func_args[1], $matches)) {
		// ��Ƭ�������� x �ڡ����ܤ��顢y��Ť�ɽ��
		$limit_base  = $matches[1];
		$limit_pitch = $matches[2];
		$limit_page  = $matches[1] + $matches[2]; // �ɤ����Ф� + ɽ������
	} else {
		return '#calendar_viewer(): ' . $_err_calendar_viewer_param2 . '<br />';
	}

	if (isset($func_args[2]) &&
	    preg_match('/^(past|view|future)$/si', $func_args[2])) {
		// �⡼�ɻ���
		$mode = $func_args[2];
	}

	if (isset($func_args[3])) {
		$date_sep = $func_args[3];
	}

	// Avoid Loop etc.
	if (isset($viewed[$pagename])) {
		$s_page = htmlspecialchars($pagename);
		return "#calendar_viewer(): You already view: $s_page<br />";
	} else {
		$viewed[$pagename] = TRUE; // Valid
	}

	// ����ɽ������ڡ���̾�ȥե�����̾�Υѥ����󡡥ե�����̾�ˤ�ǯ���ޤ�
	if ($pagename == '') {
		// pagename̵����yyyy-mm-dd���б����뤿��ν���
		$pagepattern     = '';
		$pagepattern_len = 0;
		$filepattern     = encode($page_YM);
		$filepattern_len = strlen($filepattern);
	} else {
		$pagepattern     = strip_bracket($pagename) . '/';
		$pagepattern_len = strlen($pagepattern);
		$filepattern     = encode($pagepattern . $page_YM);
		$filepattern_len = strlen($filepattern);
	}

	//echo "$pagename:$page_YM:$mode:$date_sep:$limit_base:$limit_page";
	// �ڡ����ꥹ�Ȥμ���
	//echo $pagepattern;
	//echo $filepattern;
	$pagelist = array();
	if ($dir = @opendir(DATA_DIR)) {
		$_date = get_date('Y' . $date_sep . 'm' . $date_sep . 'd');
		while($file = readdir($dir)) {
			if ($file == '..' || $file == '.') continue;
			if (substr($file, 0, $filepattern_len) != $filepattern) continue;

			//echo "OK";
			$page = decode(trim(preg_replace('/\.txt$/', ' ', $file)));

			// $page���������������ʤΤ������å� �ǥե���ȤǤϡ� yyyy-mm-dd
			$page = strip_bracket($page);
			if (plugin_calendar_viewer_isValidDate(substr($page, $pagepattern_len), $date_sep) == false) continue;

			// mode����̾��ǤϤ���

			// past mode�Ǥ�̤��Υڡ�����NG
			if (((substr($page, $pagepattern_len)) > $_date) &&
			    ($mode == 'past')) continue;

			// future mode�Ǥϲ��Υڡ�����NG
			if (((substr($page, $pagepattern_len)) < $_date) &&
				($mode == 'future')) continue;

			// view mode�ʤ�all OK
			$pagelist[] = $page;
		}
	}
	closedir($dir);
	//echo count($pagelist);

	// ������
	if ($mode == 'past') {
		rsort($pagelist);	// past mode�ǤϿ�����
	} else {
		sort($pagelist);	// view mode �� future mode �Ǥϡ��좪��
	}

	// �������饤�󥯥롼�ɳ���
	$tmppage = $vars['page'];
	$return_body = '';

	// $limit_page�η���ޤǥ��󥯥롼��
	$tmp = max($limit_base, 0); // Skip minus
	while ($tmp < $limit_page) {
		if (! isset($pagelist[$tmp])) break;

		$page = $pagelist[$tmp];
		$get['page'] = $post['page'] = $vars['page'] = $page;

		// �����Ǳ������Ĥ����������ɽ������
		if (check_readable($page, false, false)) {
			$body = convert_html(get_source($page));
		} else {
			$body = str_replace('$1', $page, $_msg_calendar_viewer_restrict);
		}

		$r_page = rawurlencode($page);
		$s_page = htmlspecialchars($page);
		$link   = "<a href=\"$script?cmd=edit&amp;page=$r_page\">$s_page</a>";
		$head   = "<h1>$link</h1>\n";
		$return_body .= $head . $body;

		++$tmp;
	}

	// �����ǡ�����Υ�󥯤�ɽ��
	// ?plugin=calendar_viewer&file=�ڡ���̾&date=yyyy-mm
	$enc_pagename = rawurlencode(substr($pagepattern, 0, $pagepattern_len - 1));

	if ($page_YM != '') {
		// ǯ��ɽ����
		$date_sep_len = strlen($date_sep);
		$this_year    = substr($page_YM, 0, 4);
		$this_month   = substr($page_YM, 4 + $date_sep_len, 2);

		// ����
		$next_year  = $this_year;
		$next_month = $this_month + 1;
		if ($next_month > 12) {
			++$next_year;
			$next_month = 1;
		}
		$next_YM = sprintf('%04d%s%02d', $next_year, $date_sep, $next_month);

		// ����
		$prev_year  = $this_year;
		$prev_month = $this_month - 1;
		if ($prev_month < 1) {
			--$prev_year;
			$prev_month = 12;
		}
		$prev_YM = sprintf('%04d%s%02d', $prev_year, $date_sep, $prev_month);
		if ($mode == 'past') {
			$right_YM   = $prev_YM;
			$right_text = $prev_YM . '&gt;&gt;'; // >>
			$left_YM    = $next_YM;
			$left_text  = '&lt;&lt;' . $next_YM; // <<
		} else {
			$left_YM    = $prev_YM;
			$left_text  = '&lt;&lt;' . $prev_YM; // <<
			$right_YM   = $next_YM;
			$right_text = $next_YM . '&gt;&gt;'; // >>
		}
	} else {
		// n��ɽ����
		if ($limit_base < 0) {
			$left_YM = ''; // ɽ�����ʤ� (���������ι��ܤϤʤ�)
		} else {
			$left_YM   = $limit_base - $limit_pitch . '*' . $limit_pitch;
			$left_text = sprintf($_msg_calendar_viewer_left, $limit_pitch);

		}
		if ($limit_base + $limit_pitch >= count($pagelist)) {
			$right_YM = ''; // ɽ�����ʤ� (�������ι��ܤϤʤ�)
		} else {
			$right_YM   = $limit_base + $limit_pitch . '*' . $limit_pitch;
			$right_text = sprintf($_msg_calendar_viewer_right, $limit_pitch);
		}
	}

	// �ʥӥ������ѤΥ�󥯤��������ɲ�
	if ($left_YM != '' || $right_YM != '') {
		$s_date_sep = htmlspecialchars($date_sep);
		$left_link = $right_link = '';
		$link = "$script?plugin=calendar_viewer&amp;mode=$mode&amp;" .
			"file=$enc_pagename&amp;date_sep=$s_date_sep&amp;";
		if ($left_YM != '') {
			$left_link = '<a href="' . $link .
				"date=$left_YM\">$left_text</a>";
		}
		if ($right_YM != '') {
			$right_link = '<a href="' . $link .
				"date=$right_YM\">$right_text</a>";
		}
		// past mode��<<�� ��>> ¾��<<�� ��>>
		$return_body .=
			'<table width ="100%"><tr>' .
			'<td align="left">'  . $left_link  . '</td>' .
			'<td align="right">' . $right_link . '</td>' .
			'</tr></table>';
	}

	$get['page'] = $post['page'] = $vars['page'] = $tmppage;

	return $return_body;
}

function plugin_calendar_viewer_action()
{
	global $vars, $get, $post, $hr, $script;

	$date_sep = '-';

	$return_vars_array = array();

	$page = strip_bracket($vars['page']);
	$vars['page'] = '*';
	if (isset($vars['file'])) $vars['page'] = $vars['file'];

	$date_sep = $vars['date_sep'];

	$page_YM = $vars['date'];
	if ($page_YM == '') {
		$page_YM = get_date('Y' . $date_sep . 'm');
	}
	$mode = $vars['mode'];

	$args_array = array($vars['page'], $page_YM, $mode, $date_sep);
	$return_vars_array['body'] = call_user_func_array('plugin_calendar_viewer_convert', $args_array);

	//$return_vars_array['msg'] = 'calendar_viewer ' . $vars['page'] . '/' . $page_YM;
	$return_vars_array['msg'] = 'calendar_viewer ' . htmlspecialchars($vars['page']);
	if ($vars['page'] != '') {
		$return_vars_array['msg'] .= '/';
	}
	if (preg_match('/\*/', $page_YM)) {
		// ������n��ɽ���λ��Ϥʤ�ƥڡ���̾�ˤ����餤����
	} else {
		$return_vars_array['msg'] .= htmlspecialchars($page_YM);
	}

	$vars['page'] = $page;
	return $return_vars_array;
}

function plugin_calendar_viewer_isValidDate($aStr, $aSepList = '-/ .')
{
	if ($aSepList == '') {
		// $aSepList = ''�ʤ顢yyyymmdd�Ȥ��ƥ����å��ʼ�ȴ��(^^;��
		return checkdate(substr($aStr, 4, 2), substr($aStr, 6, 2), substr($aStr, 0, 4));
	}
	if ( ereg("^([0-9]{2,4})[$aSepList]([0-9]{1,2})[$aSepList]([0-9]{1,2})$", $aStr, $m) ) {
		return checkdate($m[2], $m[3], $m[1]);
	}
	return false;
}

?>
