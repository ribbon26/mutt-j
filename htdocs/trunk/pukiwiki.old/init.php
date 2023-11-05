<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: init.php,v 1.73 2004/04/03 17:08:41 arino Exp $
//

/////////////////////////////////////////////////
// ������� (���顼���ϥ�٥�)
// (E_WARNING | E_NOTICE)��������Ƥ��ޤ���
error_reporting(E_ERROR | E_PARSE);

/////////////////////////////////////////////////
// ������� (ʸ�����󥳡��ɡ�����)
define('SOURCE_ENCODING','EUC-JP');
define('LANG','ja');

mb_language('Japanese');
mb_internal_encoding(SOURCE_ENCODING);
ini_set('mbstring.http_input', 'pass');
mb_http_output('pass');
mb_detect_order('auto');


/////////////////////////////////////////////////
// �������(����ե�����ξ��)
define('INI_FILE','./pukiwiki.ini.php');

/////////////////////////////////////////////////
// ������� (�С������/���)
define('S_VERSION','1.4.3');
define('S_COPYRIGHT','
<strong>"PukiWiki" '.S_VERSION.'</strong> Copyright &copy; 2001,2002,2003
<a href="http://pukiwiki.org">PukiWiki Developers Team</a>.
License is <a href="http://www.gnu.org/">GNU/GPL</a>.<br />
Based on "PukiWiki" 1.3 by <a href="http://factage.com/sng/">sng</a>
');

/////////////////////////////////////////////////
// ������� (�������ѿ�)
foreach (array('HTTP_USER_AGENT','PHP_SELF','SERVER_NAME','SERVER_SOFTWARE','SERVER_ADMIN') as $key) {
	define($key,array_key_exists($key,$_SERVER) ? $_SERVER[$key] : '');
}

/////////////////////////////////////////////////
// ������� (�����Х��ѿ�)
// �����Ф�������ѿ�
$vars = array();
// ����
$foot_explain = array();
// ��Ϣ����ڡ���
$related = array();
// <head>����ɲä��륿��
$head_tags = array();

/////////////////////////////////////////////////
// �������(����)
define('LOCALZONE',date('Z'));
define('UTIME',time() - LOCALZONE);
define('MUTIME',getmicrotime());

/////////////////////////////////////////////////
// ����ե������ɤ߹���
if (!file_exists(LANG.'.lng')||!is_readable(LANG.'.lng')) {
	die_message(LANG.'.lng(language file) is not found.');
}
require(LANG.'.lng');

/////////////////////////////////////////////////
// ����ե������ɤ߹���
if (!file_exists(INI_FILE)||!is_readable(INI_FILE)) {
	die_message(INI_FILE.' is not found.');
}
require(INI_FILE);

/////////////////////////////////////////////////
// �������($script)
if (!isset($script) or $script == '') {
	$script = get_script_uri();
}
if ($script === FALSE or (php_sapi_name() == 'cgi' and !is_url($script,TRUE)))
{
	die_message('please set "$script" in pukiwiki.ini.php.');
}

/////////////////////////////////////////////////
// ����ե������ɤ߹���(UserAgent)
foreach ($agents as $agent) {
	if (preg_match($agent['pattern'],HTTP_USER_AGENT,$matches)) {
		$agent['matches'] = $matches;
		$user_agent = $agent;
		break;
	}
}
define('UA_INI_FILE',$user_agent['name'].'.ini.php');

if (!file_exists(UA_INI_FILE)||!is_readable(UA_INI_FILE)) {
	die_message(UA_INI_FILE.' is not found.');
}
require(UA_INI_FILE);

/////////////////////////////////////////////////
// ����ե�������ѿ������å�
if(!is_writable(DATA_DIR)) {
	die_message('DATA_DIR is not found or not writable.');
}
if(!is_writable(DIFF_DIR)) {
	die_message('DIFF_DIR is not found or not writable.');
}
if($do_backup && !is_writable(BACKUP_DIR)) {
	die_message('BACKUP_DIR is not found or not writable.');
}
if(!is_writable(CACHE_DIR)) {
	die_message('CACHE_DIR is not found or not writable.');
}
$wrong_ini_file = '';
if (!isset($rss_max)) $wrong_ini_file .= '$rss_max ';
if (!isset($page_title)) $wrong_ini_file .= '$page_title ';
if (!isset($note_hr)) $wrong_ini_file .= '$note_hr ';
if (!isset($related_link)) $wrong_ini_file .= '$related_link ';
if (!isset($show_passage)) $wrong_ini_file .= '$show_passage ';
if (!isset($rule_related_str)) $wrong_ini_file .= '$rule_related_str ';
if (!isset($load_template_func)) $wrong_ini_file .= '$load_template_func ';
if (!defined('LANG')) $wrong_ini_file .= 'LANG ';
if (!defined('PLUGIN_DIR')) $wrong_ini_file .= 'PLUGIN_DIR ';
if ($wrong_ini_file) {
	die_message('The setting file runs short of information.<br />The version of a setting file may be old.<br /><br />These option are not found : '.$wrong_ini_file);
}
if (!is_page($defaultpage)) {
	touch(get_filename($defaultpage));
}
if (!is_page($whatsnew)) {
	touch(get_filename($whatsnew));
}
if (!is_page($interwiki)) {
	touch(get_filename($interwiki));
}

/////////////////////////////////////////////////
// �������餯���ѿ��򥵥˥�����
$get    = sanitize($_GET);
$post   = sanitize($_POST);
$cookie = sanitize($_COOKIE);

/////////////////////////////////////////////////
// ʸ�������ɤ��Ѵ�

// <form> ���������줿ʸ�� (�֥饦�������󥳡��ɤ����ǡ���) �Υ����ɤ��Ѵ�
// post �Ͼ�� <form> �ʤΤǡ�ɬ���Ѵ�
if (array_key_exists('encode_hint',$post))
{
	// html.php ����ǡ�<form> �� encode_hint ��Ź���Ǥ���Τǡ�ɬ�� encode_hint ������Ϥ���
	// encode_hint �Τߤ��Ѥ��ƥ����ɸ��Ф��롣
	// ���Τ򸫤ƥ����ɸ��Ф���ȡ������¸ʸ���䡢̯�ʥХ��ʥꥳ���ɤ������������ˡ�
	// �����ɸ��Ф˼��Ԥ��붲�줬���뤿�ᡣ
	$encode = mb_detect_encoding($post['encode_hint']);
	mb_convert_variables(SOURCE_ENCODING,$encode,$post);
}
else if (array_key_exists('charset',$post))
{
	// TrackBack Ping�˴ޤޤ�Ƥ��뤳�Ȥ�����
	// ���ꤵ�줿���ϡ��������Ƥ��Ѵ����ߤ�
	if (mb_convert_variables(SOURCE_ENCODING,$post['charset'],$post) !== $post['charset'])
	{
		// ���ޤ������ʤ��ä����ϥ����ɸ��Ф�������Ѵ����ʤ���
		mb_convert_variables(SOURCE_ENCODING,'auto',$post);
	}
}
else if (count($post) > 0)
{
	// encode_hint ��̵���Ȥ������Ȥϡ�̵���Ϥ���
	// �ǥХå��Ѥˡ���ꤢ�������ٹ��å�������Ф��Ƥ����ޤ���
// 	echo "<p>Warning: 'encode_hint' field is not found in the posted data.</p>\n";
	// �����ޤȤ�ơ������ɸ��С��Ѵ�
	mb_convert_variables(SOURCE_ENCODING,'auto',$post);
}

// get �� <form> ����ξ��ȡ�<a href="http;//script/?query> �ξ�礬����
if (array_key_exists('encode_hint',$get))
{
	// <form> �ξ��ϡ��֥饦�������󥳡��ɤ��Ƥ���Τǡ������ɸ��С��Ѵ���ɬ�ס�
	// encode_hint ���ޤޤ�Ƥ���Ϥ��ʤΤǡ�����򸫤ơ������ɸ��Ф����塢�Ѵ����롣
	// ��ͳ�ϡ�post ��Ʊ��
	$encode = mb_detect_encoding($get['encode_hint']);
	mb_convert_variables(SOURCE_ENCODING,$encode,$get);
}	
// <a href...> �ξ��ϡ������С��� rawurlencode ���Ƥ���Τǡ��������Ѵ�������

// QUERY_STRING�����
// cmd��plugin����ꤵ��Ƥ��ʤ����ϡ�QUERY_STRING��ڡ���̾��InterWikiName�Ǥ���Ȥߤʤ���
// �ޤ���URI �� urlencode �����˼��Ǥ������Ϥ��������н褹���
$arg = '';
if (array_key_exists('QUERY_STRING',$_SERVER) and $_SERVER['QUERY_STRING'] != '')
{
	$arg = $_SERVER['QUERY_STRING'];
}
else if (array_key_exists('argv',$_SERVER) and count($_SERVER['argv']))
{
	$arg = $_SERVER['argv'][0];
}

// ���˥����� (\0 ����)
$arg = sanitize($arg);

// URI ���Ǥξ�硢�������Ѵ�����get[] �˾��
// mb_convert_variables�ΥХ�(?)�к� ������Ϥ��ʤ��������
$arg = array($arg);
mb_convert_variables(SOURCE_ENCODING,'auto',$arg);
$arg = $arg[0];

foreach (explode('&',$arg) as $tmp_string)
{
	if (preg_match('/^([^=]+)=(.+)/',$tmp_string,$matches)
		and mb_detect_encoding($matches[2]) != 'ASCII')
	{
		$get[$matches[1]] = $matches[2];
	}
}

if (!empty($get['page']))
{
	$get['page']  = strip_bracket($get['page']);
}
if (!empty($post['page']))
{
	$post['page'] = strip_bracket($post['page']);
}
if (!empty($post['msg']))
{
	$post['msg']  = str_replace("\r",'',$post['msg']);
}

$vars = array_merge($post,$get);
if (!array_key_exists('page',$vars))
{
	$get['page'] = $post['page'] = $vars['page'] = '';
}

// �����ߴ��� (?md5=...)
if (array_key_exists('md5',$vars) and $vars['md5'] != '')
{
	$vars['cmd'] = 'md5';
}

// TrackBack Ping
if (array_key_exists('tb_id',$vars) and $vars['tb_id'] != '')
{
	$vars['cmd'] = 'tb';
}

// cmd��plugin����ꤵ��Ƥ��ʤ����ϡ�QUERY_STRING��ڡ���̾��InterWikiName�Ǥ���Ȥߤʤ�
if (!array_key_exists('cmd',$vars)  and !array_key_exists('plugin',$vars))
{
	if ($arg == '')
	{
		//�ʤˤ���ꤵ��Ƥ��ʤ��ä�����$defaultpage��ɽ��
		$arg = $defaultpage;
	}		
	$arg = rawurldecode($arg);
	$arg = strip_bracket($arg);
	$arg = sanitize($arg);

	$get['cmd'] = $post['cmd'] = $vars['cmd'] = 'read';
	$get['page'] = $post['page'] = $vars['page'] = $arg;
}

/////////////////////////////////////////////////
// �������($WikiName,$BracketName�ʤ�)
// $WikiName = '[A-Z][a-z]+(?:[A-Z][a-z]+)+';
// $WikiName = '\b[A-Z][a-z]+(?:[A-Z][a-z]+)+\b';
// $WikiName = '(?<![[:alnum:]])(?:[[:upper:]][[:lower:]]+){2,}(?![[:alnum:]])';
// $WikiName = '(?<!\w)(?:[A-Z][a-z]+){2,}(?!\w)';
// BugTrack/304�����н�
$WikiName = '(?:[A-Z][a-z]+){2,}(?!\w)';
// $BracketName = ':?[^\s\]#&<>":]+:?';
$BracketName = '(?!\s):?[^\r\n\t\f\[\]<>#&":]+:?(?<!\s)';
// InterWiki
$InterWikiName = "(\[\[)?((?:(?!\s|:|\]\]).)+):(.+)(?(1)\]\])";
// ���
$NotePattern = '/\(\(((?:(?>(?:(?!\(\()(?!\)\)(?:[^\)]|$)).)+)|(?R))*)\)\)/ex';

/////////////////////////////////////////////////
// �������(�桼������롼���ɤ߹���)
require('rules.ini.php');

/////////////////////////////////////////////////
// �������(����¾�Υ����Х��ѿ�)
// ���߻���
$now = format_date(UTIME);
// skin���DTD������ڤ��ؤ���Τ˻��ѡ�paint.inc.php�к�
// FALSE:XHTML 1.1
// TRUE :XHTML 1.0 Transitional
$html_transitional = FALSE;
// �ե������ޡ�����$line_rules�˲ä���
if ($usefacemark)
{
	$line_rules += $facemark_rules;
}
unset($facemark_rules);
// ���λ��ȥѥ����󤪤�ӥ����ƥ�ǻ��Ѥ���ѥ������$line_rules�˲ä���
//$entity_pattern = '[a-zA-Z0-9]{2,8}';
$entity_pattern = trim(join('',file(CACHE_DIR.'entities.dat')));
$line_rules = array_merge(array(
	'&amp;(#[0-9]+|#x[0-9a-f]+|'.$entity_pattern.');'=>'&$1;',
	"\r"=>"<br />\n", /* �����˥�����ϲ��� */
	'#related$'=>'<del>#related</del>',
	'^#contents$'=>'<del>#contents</del>'
),$line_rules);
?>
