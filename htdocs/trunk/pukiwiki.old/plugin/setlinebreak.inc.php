<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: setlinebreak.inc.php,v 1.1 2004/03/18 10:45:47 arino Exp $
//
// ���Ԥ�<br />���ִ�����ե饰($line_break)�����ꤹ��
//
// #setlinebreak(on) : ����ʹߡ����Ԥ�<br />���ִ�����
// #setlinebreak(off) : ����ʹߡ����Ԥ�<br />���ִ����ʤ�
// #setlinebreak : ����ʹߡ����Ԥ�<br />���ִ�����/���ʤ����ڤ��ؤ�
// #setlinebreak(default) : ����ʹߡ����Ԥΰ����򥷥��ƥ�������᤹

function plugin_setlinebreak_convert()
{
	global $line_break;
	static $default;
	
	if (!isset($default))
	{
		$default = $line_break;
	}
	if (func_num_args() == 0)
	{
		$line_break = !$line_break;
		return '';
	}
	
	$args = func_get_args();
	
	switch (strtolower($args[0]))
	{
		case 'on':
		case 'true':
		case '1':
			$line_break = 1;
			break;
		
		case 'off':
		case 'false':
		case '0':
			$line_break = 0;
			break;
		case 'default':
			$line_break = $default;
			break;
		
		default:
			return FALSE;
	}
	return '';
}
?>
