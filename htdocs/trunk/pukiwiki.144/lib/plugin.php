<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: plugin.php,v 1.3 2004/08/09 14:09:42 henoheno Exp $
//

// �ץ饰�����Ѥ�̤����Υ����Х��ѿ�������
function set_plugin_messages($messages)
{
	foreach ($messages as $name=>$val) {
		if (! isset($GLOBALS[$name])) $GLOBALS[$name] = $val;
	}
}

//�ץ饰����¸�ߤ��뤫
function exist_plugin($name)
{
	$name = strtolower($name);	// ��ʸ���Ⱦ�ʸ������̤��ʤ��ե����륷���ƥ��к�
	if (preg_match('/^\w{1,64}$/', $name) &&
	    file_exists(PLUGIN_DIR . $name . '.inc.php')) {
		require_once(PLUGIN_DIR . $name . '.inc.php');
		return TRUE;
	} else {
		return FALSE;
	}
}

//�ץ饰����ؿ�(action)��¸�ߤ��뤫
function exist_plugin_action($name) {
	return	function_exists('plugin_' . $name . '_action') ? TRUE : exist_plugin($name) ?
		function_exists('plugin_' . $name . '_action') : FALSE;
}

//�ץ饰����ؿ�(convert)��¸�ߤ��뤫
function exist_plugin_convert($name) {
	return	function_exists('plugin_' . $name . '_convert') ? TRUE : exist_plugin($name) ?
		function_exists('plugin_' . $name . '_convert') : FALSE;
}

//�ץ饰����ؿ�(inline)��¸�ߤ��뤫
function exist_plugin_inline($name) {
	return	function_exists('plugin_' . $name . '_inline') ? TRUE : exist_plugin($name) ?
		function_exists('plugin_' . $name . '_inline') : FALSE;
}

//�ץ饰����ν������¹�
function do_plugin_init($name)
{
	static $checked = array();

	if (! isset($checked[$name])) {
		$func = 'plugin_' . $name . '_init';
		if (function_exists($func)) {
			// TRUE or FALSE or NULL (return nothing)
			$checked[$name] = call_user_func($func);
		} else {
			// Not exists
			$checked[$name] = null;
		}
	}
	return $checked[$name];
}

//�ץ饰����(action)��¹�
function do_plugin_action($name)
{
	if (! exist_plugin_action($name)) return array();

	if(do_plugin_init($name) === FALSE)
		die_message("Plugin init failed: $name");

	$retvar = call_user_func('plugin_' . $name . '_action');

	// ʸ�����󥳡��ǥ��󥰸����� hidden �ե�����ɤ�����
	return preg_replace('/(<form[^>]*>)/',
		"$1\n" . '<div><input type="hidden" name="encode_hint" value="��" /></div>',
		$retvar);
}

//�ץ饰����(convert)��¹�
function do_plugin_convert($name, $args = '')
{
	global $digest;

	if(do_plugin_init($name) === FALSE)
		return "[Plugin init failed: $name]";

	if ($args !== '') {
		$aryargs = csv_explode(',', $args);
	} else {
		$aryargs = array();
	}

	$_digest = $digest;  // ����
	$retvar  = call_user_func_array('plugin_' . $name . '_convert', $aryargs);
	$digest  = $_digest; // ����

	if ($retvar === FALSE) {
		return htmlspecialchars('#' . $name . ($args ? "($args)" : ''));
	} else {
		// ʸ�����󥳡��ǥ��󥰸����� hidden �ե�����ɤ�����
		return preg_replace('/(<form[^>]*>)/',
			"$1\n" . '<div><input type="hidden" name="encode_hint" value="��" /></div>',
			$retvar);
	}
}

//�ץ饰����(inline)��¹�
function do_plugin_inline($name, $args, & $body)
{
	global $digest;

	if(do_plugin_init($name) === FALSE)
		return "[Plugin init failed: $name]";

	if ($args !== '') {
		$aryargs = csv_explode(',', $args);
	} else {
		$aryargs = array();
	}
	$aryargs[] = & $body; // Added reference of $body

	$_digest = $digest;  // ����
	$retvar  = call_user_func_array('plugin_' . $name . '_inline', $aryargs);
	$digest  = $_digest; // ����

	if($retvar === FALSE) {
		return htmlspecialchars("&${name}" . ($args ? "($args)" : '') . ';');
	} else {
		return $retvar;
	}
}
?>
