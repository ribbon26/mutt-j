<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: ruby.inc.php,v 1.4 2004/07/31 03:09:20 henoheno Exp $
//

function plugin_ruby_inline()
{
	if (func_num_args() != 2)
	{
		return FALSE;
	}

	list($ruby,$body) = func_get_args();

	if ($ruby == '' or $body == '')
	{
		return FALSE;
	}

	$s_ruby = htmlspecialchars($ruby);
	return "<ruby><rb>$body</rb><rp>(</rp><rt>$s_ruby</rt><rp>)</rp></ruby>";
}
?>
