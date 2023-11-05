<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: topicpath.inc.php,v 1.4 2004/08/12 13:02:26 henoheno Exp $
// topicpath plugin for PukiWiki
//   available under the GPL

// $defaultpage�ؤΥ�󥯤�ɽ�����뤫�ɤ���
// TRUE:ɽ������ FALSE:ɽ�����ʤ�.
define('PLUGIN_TOPICPATH_TOP_DISPLAY', TRUE);
// $defaultpage���Ф����٥�
define('PLUGIN_TOPICPATH_TOP_LABEL', 'Top');

// ���ؤ���ڤ륻�ѥ졼��
define('PLUGIN_TOPICPATH_TOP_SEPARATOR', ' / ');

// ���Υڡ������Ȥ�ɽ�����뤫
define('PLUGIN_TOPICPATH_THIS_PAGE_DISPLAY', TRUE);
// ɽ�������硢��ʬ���Ȥ�ؤ���󥯤�ɽ�����뤫
define('PLUGIN_TOPICPATH_THIS_PAGE_LINK', FALSE);

function plugin_topicpath_convert()
{
	return '<div>' . plugin_topicpath_inline() . '</div>';
}

function plugin_topicpath_inline()
{
	global $script, $vars, $defaultpage;

	// $args = func_get_args();

	$page = isset($vars['page']) ? $vars['page'] : '';
	if ($page == '' || $page == $defaultpage) return '';

	$parts = explode('/', $page);

	if (PLUGIN_TOPICPATH_THIS_PAGE_DISPLAY) {
		$b_link = PLUGIN_TOPICPATH_THIS_PAGE_LINK;
	} else {
		array_pop($parts); // Remove itself
		$b_link = TRUE;    // Link to the parent
	}

	$topic_path = array();
	while (! empty($parts)) {
		$landing = rawurlencode(join('/', $parts));
		$element = htmlspecialchars(array_pop($parts));
		if ($b_link)  {
			$topic_path[] = "<a href=\"$script?$landing\">$element</a>";
		} else {
			$topic_path[] = $element;
			$b_link = TRUE; // Maybe reacheable once at a time
		}
	}

	if (PLUGIN_TOPICPATH_TOP_DISPLAY) {
		$topic_path[] = make_pagelink($defaultpage, PLUGIN_TOPICPATH_TOP_LABEL);
	}

	return join(PLUGIN_TOPICPATH_TOP_SEPARATOR, array_reverse($topic_path));
}
?>
