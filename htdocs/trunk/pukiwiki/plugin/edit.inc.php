<?php
// PukiWiki - Yet another WikiWikiWeb clone.
// $Id: edit.inc.php,v 1.30 2005/01/23 05:22:25 henoheno Exp $
//
// Edit plugin
// cmd=edit

function plugin_edit_action()
{
	global $vars, $_title_edit, $load_template_func;

	if (PKWK_READONLY) die_message('PKWK_READONLY prohibits editing');

	$page = isset($vars['page']) ? $vars['page'] : '';

	check_editable($page, true, true);

	if (isset($vars['preview']) || ($load_template_func && isset($vars['template']))) {
		return plugin_edit_preview();
	} else if (isset($vars['write'])) {
		return plugin_edit_write();
	} else if (isset($vars['cancel'])) {
		return plugin_edit_cancel();
	}

	$postdata = @join('', get_source($page));
	if ($postdata == '') $postdata = auto_template($page);

	return array('msg'=>$_title_edit, 'body'=>edit_form($page, $postdata));
}

// Preview
function plugin_edit_preview()
{
	global $vars;
	global $_title_preview, $_msg_preview, $_msg_preview_delete;

	$page = isset($vars['page']) ? $vars['page'] : '';

	// Loading template
	if (isset($vars['template_page']) && is_page($vars['template_page'])) {

		$vars['msg'] = join('', get_source($vars['template_page']));

		// Cut fixed anchors
		$vars['msg'] = preg_replace('/^(\*{1,3}.*)\[#[A-Za-z][\w-]+\](.*)$/m', '$1$2', $vars['msg']);
	}

	// ��񤭤�#freeze����
	$vars['msg'] = preg_replace('/^#freeze\s*$/im', '', $vars['msg']);
	$postdata = $vars['msg'];

	if (isset($vars['add']) && $vars['add']) {
		if (isset($vars['add_top']) && $vars['add_top']) {
			$postdata  = $postdata . "\n\n" . @join('', get_source($page));
		} else {
			$postdata  = @join('', get_source($page)) . "\n\n" . $postdata;
		}
	}

	$body = "$_msg_preview<br />\n";
	if ($postdata == '')
		$body .= "<strong>$_msg_preview_delete</strong>";
	$body .= "<br />\n";

	if ($postdata) {
		$postdata = make_str_rules($postdata);
		$postdata = explode("\n", $postdata);
		$postdata = drop_submit(convert_html($postdata));
		$body .= '<div id="preview">' . $postdata . '</div>' . "\n";
	}
	$body .= edit_form($page, $vars['msg'], $vars['digest'], FALSE);

	return array('msg'=>$_title_preview, 'body'=>$body);
}

// Inline: Show edit (or unfreeze text) link
function plugin_edit_inline()
{
	static $usage = '&edit(pagename#anchor[[,noicon],nolabel])[{label}];';

	global $script, $vars, $fixed_heading_anchor_edit;

	if (PKWK_READONLY) return ''; // Show nothing 

	// Arguments
	$args = func_get_args();
	$s_label = array_pop($args); // {label}
	$page    = array_shift($args);
	if($page == NULL) $page = '';
	$_noicon = $_nolabel = FALSE;
	foreach($args as $arg){
		switch($arg){
		case '': break;
		case 'nolabel': $_nolabel = TRUE; break;
		case 'noicon':  $_noicon  = TRUE; break;
		default: return $usage;
		}
	}

	// Separate a page-name and a fixed anchor
	list($s_page, $id, $editable) = anchor_explode($page, TRUE);
	// Default: This one
	if ($s_page == '') $s_page = isset($vars['page']) ? $vars['page'] : '';
	// $s_page fixed
	$isfreeze = is_freeze($s_page);
	$ispage   = is_page($s_page);

	// Paragraph edit enabled or not
	$short = htmlspecialchars('Edit');
	if ($fixed_heading_anchor_edit && $editable && $ispage && ! $isfreeze) {
		// Paragraph editing
		$id    = rawurlencode($id);
		$title = htmlspecialchars(sprintf('Edit %s', $page));
		$icon = '<img src="' . IMAGE_DIR . 'paraedit.png' .
			'" width="9" height="9" alt="' .
			$short . '" title="' . $title . '" /> ';
		$class = ' class="anchor_super"';
	} else {
		// Normal editing / unfreeze
		$id    = '';
		if ($isfreeze) {
			$title = 'Unfreeze %s';
			$icon  = 'unfreeze.png';
		} else {
			$title = 'Edit %s';
			$icon  = 'edit.png';
		}
		$title = htmlspecialchars(sprintf($title, $s_page));
		$icon = '<img src="' . IMAGE_DIR . $icon .
			'" width="20" height="20" alt="' .
			$short . '" title="' . $title . '" />';
		$class = '';
	}
	if ($_noicon) $icon = ''; // No more icon
	if ($_nolabel) {
		if (!$_noicon) {
			$s_label = '';     // No label with an icon
		} else {
			$s_label = $short; // Short label without an icon
		}
	} else {
		if ($s_label == '') $s_label = $title; // Rich label with an icon
	}

	// URL
	if ($isfreeze) {
		$url   = $script . '?cmd=unfreeze&amp;page=' . rawurlencode($s_page);
	} else {
		if ($id != '') {
			$s_id = '&amp;id=' . $id;
		} else {
			$s_id = '';
		}
		$url  = $script . '?cmd=edit&amp;page=' . rawurlencode($s_page) . $s_id;
	}
	$atag  = '<a' . $class . ' href="' . $url . '" title="' . $title . '">';
	static $atags = '</a>';

	if ($ispage) {
		// Normal edit link
		return $atag . $icon . $s_label . $atags;
	} else {
		// Dangling edit link
		return '<span class="noexists">' . $atag . $icon . $atags .
			$s_label . $atag . '?' . $atags . '</span>';
	}
}

// Write, add, or insert new comment
function plugin_edit_write()
{
	global $vars;
	global $_title_collided, $_msg_collided_auto, $_msg_collided, $_title_deleted;

//spamcheck
if (! isset($vars['spam']) || $vars['spam'] != 'no') {
  return FALSE;
}
	$page = isset($vars['page']) ? $vars['page'] : '';
	$retvars = array();

	// ��񤭤�#freeze����
	$vars['msg'] = preg_replace('/^#freeze\s*$/im','',$vars['msg']);
	$postdata = $postdata_input = $vars['msg'];

	if (isset($vars['add']) && $vars['add']) {
		if (isset($vars['add_top']) && $vars['add_top']) {
			$postdata  = $postdata . "\n\n" . @join('', get_source($page));
		} else {
			$postdata  = @join('', get_source($page)) . "\n\n" . $postdata;
		}
	}

	$oldpagesrc = join('', get_source($page));
	$oldpagemd5 = md5($oldpagesrc);

	if (! isset($vars['digest']) || $vars['digest'] != $oldpagemd5) {
		$vars['digest'] = $oldpagemd5;

		$retvars['msg'] = $_title_collided;
		list($postdata_input, $auto) = do_update_diff($oldpagesrc, $postdata_input, $vars['original']);

		$retvars['body'] = ($auto ? $_msg_collided_auto : $_msg_collided) . "\n";

		if (TRUE) {
			global $do_update_diff_table;
			$retvars['body'] .= $do_update_diff_table;
		}

		$retvars['body'] .= edit_form($page, $postdata_input, $oldpagemd5, FALSE);
	}
	else {
		$notimestamp = (isset($vars['notimestamp']) && $vars['notimestamp'] != '');
		page_write($page, $postdata, $notimestamp);

		if ($postdata) {
			pkwk_headers_sent();
			header('Location: ' . get_script_uri() . '?' . rawurlencode($page));
			exit;
		}

		$retvars['msg'] = $_title_deleted;
		$retvars['body'] = str_replace('$1', htmlspecialchars($page), $_title_deleted);
		tb_delete($page);
	}

	return $retvars;
}

// Cancel (Back to the page / Escape edit page)
function plugin_edit_cancel()
{
	global $vars;
	pkwk_headers_sent();
	header('Location: ' . get_script_uri() . '?' . rawurlencode($vars['page']));
	exit;
}

?>
