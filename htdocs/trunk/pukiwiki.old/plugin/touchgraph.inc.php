<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: touchgraph.inc.php,v 1.3 2004/03/18 09:33:23 arino Exp $
//
// TouchGraph WikiBrowser�ѤΥ���ǥå�������ޤ���
//
// Usage:
//   java -Dfile.encoding=EUC-JP \
//    -cp TGWikiBrowser.jar;BrowserLauncher.jar com.touchgraph.wikibrowser.TGWikiBrowser \
//    http://<pukiwiki site>/pukiwiki.php?plugin=touchgraph \
//    http://<pukiwiki site>/pukiwiki.php? \
//    FrontPage 2 true
//
// TouchGraph http://www.touchgraph.com/index.html

function plugin_touchgraph_action()
{
	header('Content-type: text/plain');
	plugin_touchgraph_rel();
//	plugin_touchgraph_ref(); // reverse
	
	die;	
}
function plugin_touchgraph_rel()
{
	foreach (get_existpages() as $page)
	{
		if (file_exists(CACHE_DIR.encode($page).'.rel'))
		{
			$data = file(CACHE_DIR.encode($page).'.rel');
			echo $page.' '.str_replace("\t",' ',trim($data[0]))."\n";
		}
	}
}
function plugin_touchgraph_ref()
{
	foreach (get_existpages() as $page)
	{
		if (file_exists(CACHE_DIR.encode($page).'.ref'))
		{
			$data = file(CACHE_DIR.encode($page).'.ref');
			$node = $page;
			foreach ($data as $line)
			{
				list($name) = explode("\t",$line);
				$node .= " $name";
			}
			echo "$node\n";
		}
	}
}
?>
