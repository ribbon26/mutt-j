<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: rules.ini.php,v 1.2 2004/03/20 13:32:29 arino Exp $
//
// PukiWiki setting file

/////////////////////////////////////////////////
// �桼������롼��
//
//  ����ɽ���ǵ��Ҥ��Ƥ���������?(){}-*./+\$^|�ʤ�
//  �� \? �Τ褦�˥������Ȥ��Ƥ���������
//  �����ɬ�� / ��ޤ�Ƥ�����������Ƭ����� ^ ��Ƭ�ˡ�
//  ��������� $ ����ˡ�
//
/////////////////////////////////////////////////
// �桼������롼��(ľ�ܥ��������ִ�)
$str_rules = array(
	'now\?' => format_date(UTIME),
	'date\?' => get_date($date_format),
	'time\?' => get_date($time_format),
	'&now;' => format_date(UTIME),
	'&date;' => get_date($date_format),
	'&time;' => get_date($time_format),
	'&page;' => array_pop(explode('/',$vars['page'])),
	'&fpage;' => $vars['page'],
	'&t;' => "\t",
);

/////////////////////////////////////////////////
// �ե������ޡ�������롼��(����С��Ȼ����ִ�)
// $usefacemark = 1�ʤ�ե������ޡ������ִ�����ޤ�
// ʸ�����XD�ʤɤ����ä�����facemark���ִ�����Ƥ��ޤ��Τ�
// ɬ�פΤʤ����� $usefacemark��0�ˤ��Ƥ���������
$facemark_rules = array(
'\s(\:\))' => ' <img src="./face/smile.png" alt="$1" />',
'\s(\:D)' => ' <img src="./face/bigsmile.png" alt="$1" />',
'\s(\:p)' => ' <img src="./face/huh.png" alt="$1" />',
'\s(\:d)' => ' <img src="./face/huh.png" alt="$1" />',
'\s(XD)' => ' <img src="./face/oh.png" alt="$1" />',
'\s(X\()' => ' <img src="./face/oh.png" alt="$1" />',
'\s(;\))' => ' <img src="./face/wink.png" alt="$1" />',
'\s(;\()' => ' <img src="./face/sad.png" alt="$1" />',
'\s(\:\()' => ' <img src="./face/sad.png" alt="$1" />',
'&amp;(smile);' => ' <img src="./face/smile.png" alt="$1" />',
'&amp;(bigsmile);' => ' <img src="./face/bigsmile.png" alt="$1" />',
'&amp;(huh);' => ' <img src="./face/huh.png" alt="$1" />',
'&amp;(oh);' => ' <img src="./face/oh.png" alt="$1" />',
'&amp;(wink);' => ' <img src="./face/wink.png" alt="$1" />',
'&amp;(sad);' => ' <img src="./face/sad.png" alt="$1" />',
'&amp;(heart);' => '<img src="./face/heart.png" alt="$1" />',
);
?>
