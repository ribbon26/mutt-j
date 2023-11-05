<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: showrss.inc.php,v 1.11 2003/12/03 12:30:02 arino Exp $
//
// modified by PANDA <panda@arino.jp>
//

/**
 *
 * showrss �ץ饰����
 * 
 * �饤���󥹤� PukiWiki ���Τ�Ʊ���� GNU General Public License (GPL) �Ǥ���
 * http://www.gnu.org/licenses/gpl.txt
 *
 * pukiwiki�ѤΥץ饰����Ǥ���
 * pukiwiki1.3.2�ʾ��ư���Ȼפ��ޤ���
 * 
 * ���ΤȤ���ư����뤿��ˤ�PHP �� xml extension ��ɬ�ܤǤ���PHP���Ȥ߹��ޤ�Ƥʤ����Ϥ��ä��ʤ����顼���Ф�Ȼפ��ޤ���
 * ����ɽ�� or ʸ����ؿ��Ǥʤ�Ȥ��ʤ�ʤ���ʤ����ʤ�Ǥ������פäƤɤ줯�餤����Τ��狼�餤�Τ���α�Ǥ���
 * mbstring �⤢��ۤ��������Ǥ���
 * 
 * �ʤ����ϡ� jcode.phps ����礳�äȤ����ä� mb_convert_encoding �Ȥ����ؿ���������Ƥ����ФȤꤢ��������äݤ��Ѵ��Ǥ��뤫��Ǥ���
 * http://www.spencernetwork.org/
 * 
 * ��Ϣ����:
 * do3ob wiki   ->   http://do3ob.com/
 * email        ->   hiro_do3ob@yahoo.co.jp
 * 
 * �����       ->   http://do3ob.s20.xrea.com/
 *
 * version: Id:showrss.inc.php,v 1.40 2003/03/18 11:52:58 hiro Exp
 * 
 */

// showrss�ץ饰���󤬻��Ѳ�ǽ���ɤ�����ɽ��
function plugin_showrss_action()
{
	$xml_extension = extension_loaded('xml');
	$mbstring_extension = extension_loaded('mbstring');

	$xml_msg      = $xml_extension      ? 'xml extension is loaded' : 'COLOR(RED){xml extension is not loaded}';
	$mbstring_msg = $mbstring_extension ? 'mbstring extension is loaded' : 'COLOR(RED){mbstring extension is not loaded}';

	$showrss_info = '';
	$showrss_info .= "| xml parser | $xml_msg |\n";
	$showrss_info .= "| multibyte | $mbstring_msg |\n";

	return array('msg' => 'showrss_info', 'body' => convert_html($showrss_info));
}

function plugin_showrss_convert()
{
	if (func_num_args() == 0)
	{
		// �������ʤ����ϥ��顼
		return "<p>showrss: no parameter(s).</p>\n";
	}
	if (!extension_loaded('xml'))
	{
		// xml ��ĥ��ǽ��ͭ���Ǥʤ���硣
		return "<p>showrss: xml extension is not loaded</p>\n";
	}

	$array = func_get_args();
	$rssurl = $tmplname = $usecache = $usetimestamp = '';

	switch (func_num_args())
	{
		case 4:
			$usetimestamp = trim($array[3]);
		case 3:
			$usecache = $array[2];
		case 2:
			$tmplname = strtolower(trim($array[1]));
		case 1:
			$rssurl = trim($array[0]);
	}

	// RSS �ѥ����ͥ����å�
	if (!is_url($rssurl))
	{
		return '<p>showrss: syntax error. '.htmlspecialchars($rssurl)."</p>\n";
	}

	$class = "ShowRSS_html_$tmplname";
	if (!class_exists($class))
	{
		$class = 'ShowRSS_html';
	}

	list($rss,$time) = plugin_showrss_get_rss($rssurl,$usecache);
	if ($rss === FALSE)
	{
		return "<p>showrss: cannot get rss from server.</p>\n";
	}

	$obj = new $class($rss);

	$timestamp = '';
	if ($usetimestamp > 0)
	{
		$time = get_date('Y/m/d H:i:s',$time);
		$timestamp = "<p style=\"font-size:10px; font-weight:bold\">Last-Modified:$time</p>";
	}
	return $obj->toString($timestamp);
}
// rss���󤫤�html����
class ShowRSS_html
{
	var $items = array();
	var $class = '';

	function ShowRSS_html($rss)
	{
		foreach ($rss as $date=>$items)
		{
			foreach ($items as $item)
			{
				$link = $item['LINK'];
				$title = $item['TITLE'];
				$passage = get_passage($item['_TIMESTAMP']);
				$link = "<a href=\"$link\" title=\"$title $passage\">$title</a>";
				$this->items[$date][] = $this->format_link($link);
			}
		}
	}
	function format_link($link)
	{
		return "$link<br />\n";
	}
	function format_list($date,$str)
	{
		return $str;
	}
	function format_body($str)
	{
		return $str;
	}
	function toString($timestamp)
	{
		$retval = '';
		foreach ($this->items as $date=>$items)
		{
			$retval .= $this->format_list($date,join('',$items));
		}
		$retval = $this->format_body($retval);
		return <<<EOD
<div{$this->class}>
$retval$timestamp
</div>
EOD;
	}
}
class ShowRSS_html_menubar extends ShowRSS_html
{
	var $class = ' class="small"';

	function format_link($link)
	{
		return "<li>$link</li>\n";
	}
	function format_body($str)
	{
		return "<ul class=\"recent_list\">\n$str</ul>\n";
	}
}
class ShowRSS_html_recent extends ShowRSS_html
{
	var $class = ' class="small"';

	function format_link($link)
	{
		return "<li>$link</li>\n";
	}
	function format_list($date,$str)
	{
		return "<strong>$date</strong>\n<ul class=\"recent_list\">\n$str</ul>\n";
	}
}
// rss���������
function plugin_showrss_get_rss($target,$usecache)
{
	$buf = '';
	$time = NULL;
	if ($usecache)
	{
		// �����ڤ�Υ���å���򥯥ꥢ
		plugin_showrss_cache_expire($usecache);

		// ����å��夬����м�������
		$filename = CACHE_DIR . encode($target) . '.tmp';
		if (is_readable($filename))
		{
			$buf = join('',file($filename));
			$time = filemtime($filename) - LOCALZONE;
		}
	}
	if ($time === NULL)
	{
		// rss���Τ����
		$data = http_request($target);
		if ($data['rc'] !== 200)
		{
			return array(FALSE,0);
		}
		$buf = $data['data'];
		$time = UTIME;
		// ����å������¸
		if ($usecache)
		{
			$fp = fopen($filename, 'w');
			fwrite($fp,$buf);
			fclose($fp);
		}
	}

	// parse
	$obj = new ShowRSS_XML();
	return array($obj->parse($buf),$time);
}
// �����ڤ�Υ���å���򥯥ꥢ
function plugin_showrss_cache_expire($usecache)
{
	$expire = $usecache * 60 * 60; // Hour

	$dh = dir(CACHE_DIR);
	while (($file = $dh->read()) !== FALSE)
	{
		if (substr($file,-4) != '.tmp')
		{
			continue;
		}
		$file = CACHE_DIR.$file;
		$last = time() - filemtime($file);

		if ($last > $expire)
		{
			unlink($file);
		}
	}
	$dh->close();
}
// rss�����������
class ShowRSS_XML
{
	var $items;
	var $item;
	var $is_item;
	var $tag;

	function parse($buf)
	{
		// �����
		$this->items = array();
		$this->item = array();
		$this->is_item = FALSE;
		$this->tag = '';

		$xml_parser = xml_parser_create();
		xml_set_element_handler($xml_parser,array(&$this,'start_element'),array(&$this,'end_element'));
		xml_set_character_data_handler($xml_parser,array(&$this,'character_data'));

		if (!xml_parse($xml_parser,$buf,1))
		{
			return(sprintf('XML error: %s at line %d in %s',
				xml_error_string(xml_get_error_code($xml_parser)),
				xml_get_current_line_number($xml_parser),$buf));
		}
		xml_parser_free($xml_parser);

		return $this->items;
	}
	function escape($str)
	{
		// RSS��� "&lt; &gt; &amp;" �ʤɤ� ��ö "< > &" ���ᤷ�� �� "&amp;" �� "&amp;amp;" �ˤʤä��㤦���к�
		// ���θ��ä���"< > &"�ʤɤ�"&lt; &gt; &amp;"�ˤ���  �� XSS�к���
		$str = strtr($str, array_flip(get_html_translation_table(ENT_COMPAT)));
		$str = htmlspecialchars($str);

		// ʸ���������Ѵ�
		$str = mb_convert_encoding($str, SOURCE_ENCODING, 'auto');

		return trim($str);
	}

	// ��������
	function start_element($parser,$name,$attrs)
	{
		if ($this->is_item)
		{
			$this->tag = $name;
		}
		else if ($name == 'ITEM')
		{
			$this->is_item = TRUE;
		}
	}
	// ������λ
	function end_element($parser,$name)
	{
		if (!$this->is_item or $name != 'ITEM')
		{
			return;
		}
		$item = array_map(array(&$this,'escape'),$this->item);

		$this->item = array();

		if (array_key_exists('DC:DATE',$item))
		{
			$time = plugin_showrss_get_timestamp($item['DC:DATE']);
		}
		else if (array_key_exists('PUBDATE',$item))
		{
			$time = plugin_showrss_get_timestamp($item['PUBDATE']);
		}
		else if (array_key_exists('DESCRIPTION',$item)
			and ($description = trim($item['DESCRIPTION'])) != ''
			and ($time = strtotime($description)) != -1)
		{
			$time -= LOCALZONE;
		}
		else
		{
			$time = time() - LOCALZONE;
		}
		$item['_TIMESTAMP'] = $time;
		$date = get_date('Y-m-d',$item['_TIMESTAMP']);

		$this->items[$date][] = $item;
		$this->is_item = FALSE;
	}
	// ����饯��
	function character_data($parser,$data)
	{
		if (!$this->is_item)
		{
			return;
		}
		if (!array_key_exists($this->tag,$this->item))
		{
			$this->item[$this->tag] = '';
		}
		$this->item[$this->tag] .= $data;
	}
}
function plugin_showrss_get_timestamp($str)
{
	if (($str = trim($str)) == '')
	{
		return UTIME;
	}
	if (!preg_match('/(\d{4}-\d{2}-\d{2})T(\d{2}:\d{2}:\d{2})(([+-])(\d{2}):(\d{2}))?/',$str,$matches))
	{
		$time = strtotime($str);
		return ($time == -1) ? UTIME : $time - LOCALZONE;
	}
	$str = $matches[1];
	$time = strtotime($matches[1].' '.$matches[2]);
	if (!empty($matches[3]))
	{
		$diff = ($matches[5]*60+$matches[6])*60;
		$time += ($matches[4] == '-' ? $diff : -$diff);
	}
	return $time;
}
?>
