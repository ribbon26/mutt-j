<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id$
//
// topicpath plugin for PukiWiki
//   available under the GPL


//	defaultpage����ֺǽ��ɽ�����뤫�ɤ�����TRUE:ɽ������ FALSE:ɽ�����ʤ�.
define('PLUGIN_TOPICPATH_TOP_DISPLAY',TRUE);
//	$defaultpage���Ф����٥�
define('PLUGIN_TOPICPATH_TOP_LABEL','Top');
//	���ؤ���ڤ륻�ѥ졼��
define('PLUGIN_TOPICPATH_TOP_SEPARATOR',' / ');
//	��ʬ�Υڡ������Ф����󥯤�ɽ�����뤫�ɤ���
define('PLUGIN_TOPICPATH_THIS_PAGE_DISPLAY',TRUE);
//	��ʬ�Υڡ������Ф��ƥ�󥯤��뤫�ɤ���
define('PLUGIN_TOPICPATH_THIS_PAGE_LINK',TRUE);

function plugin_topicpath_convert()
{
	return '<div>'.plugin_topicpath_inline().'</div>';
}

function plugin_topicpath_inline()
{
	global $script,$vars,$defaultpage;
	
	$args = func_get_args();
	
	$page = $vars['page'];
	
	if ($page == $defaultpage) { return ''; }
	
	$topic_path = array();
	$parts = explode('/', $page);

	if (!PLUGIN_TOPICPATH_THIS_PAGE_DISPLAY) { array_pop($parts); }

	$b_link = PLUGIN_TOPICPATH_THIS_PAGE_LINK;
	while (count($parts)) {
		$landing = join('/', $parts);
		$element = array_pop($parts);
		$topic_path[] = $b_link ? "<a href=\"$script?".rawurlencode($landing)."\">$element</a>" : htmlspecialchars($element);
		$b_link = TRUE;
	}
	if (PLUGIN_TOPICPATH_TOP_DISPLAY)
	{
		$topic_path[] = make_pagelink($defaultpage,PLUGIN_TOPICPATH_TOP_LABEL);
	}
	return join(PLUGIN_TOPICPATH_TOP_SEPARATOR, array_reverse($topic_path));
}
?>
