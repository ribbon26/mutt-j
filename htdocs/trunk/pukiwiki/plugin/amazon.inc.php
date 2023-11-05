<?php
// PukiWiki - Yet another WikiWikiWeb clone.
// $Id: amazon.inc.php,v 1.11 2004/12/28 15:12:25 henoheno Exp $
// Id: amazon.inc.php,v 1.1 2003/07/24 13:00:00 �׼�
//
// Amazon plugin: Book-review maker via amazon.com/amazon.jp
//
// Copyright:
//	2004 PukiWiki Developer Team
//	2003 �׼� <raku@rakunet.org> (Original author)
//
// License: GNU/GPL
//
// ChangeLog:
// * 2004/04/03 PukiWiki Developer Team (arino <arino@users.sourceforge.jp>)
//        - replace plugin_amazon_get_page().
//        - PLUGIN_AMAZON_XML 'xml.amazon.com' -> 'xml.amazon.co.jp'
// * 0.6  URL ��¸�ߤ��ʤ���硢No image ��ɽ�����������֤ʤɽ�����
//        ����饤��ץ饰����θƤӽФ���������
//	  ASIN �ֹ���ʬ������å����롣
//	  �����������ȥ�Υ���å���ˤ��®�٤��������åס�
// * 0.7  �֥å���ӥ塼�����ΥǥХå���ǧ������ΰ���Υ��ꥢ��
// * 0.8  amazon �����ʤβ�����ɽ����
//	  ������������ ID ���б���
// * 0.9  RedHat9+php4.3.2+apache2.0.46 �ǲ���������ޤǤ����ɤ߹��ޤ�ʤ�������н衣
//        ���ܸ�ڡ����β��˥֥å���ӥ塼�����Ȥ����ʸ���������ƺ��ʤ�����β�衣
//        ���ҤǤʤ� CD �ʤɡ�ASIN ��ʬ��Ĺ���Ƥ⥿���ȥ�򤦤ޤ������褦�ˤ��롣
//        �̱ƤΤ߼�����ΤǤʤ���С�B000002G6J.01 �Ƚ񤫤� B000002G6J �Ƚ񤤤Ƥ�̱Ƥ��Ф�褦�ˤ��롣
//	  ASIN ���б����륭��å������/����å��奿���ȥ�򤽤줾�������뵡ǽ�ɲá�
//	  proxy �б�(�Ū)��
//	  proxy �����β����ǰ��̥桼���Τ���� AID �Ϥʤ��Ȥ⼫ư��������뤳�Ȥ��狼�ꡢ���������
// * 1.0  �֥å���ӥ塼�Ǥʤ�����ӥ塼�Ȥ��롣
//        �����Υ���å������������¤��ߤ��롣
//        �����ȥ롢�̱Ƥ� Web Services �� XML ������������ˡ�ˤ�ä� get ���뤳�Ȥǻ��֤�û�̤��롣
//        ��ӥ塼�ڡ��������Υ����ߥ󥰤ˤĤ����������롣
// * 1.1  �Խ����¤򤫤��Ƥ����硢�����Ԥ���ӥ塼�����Ȥ��ơ��ڡ����ϤǤ��ʤ��� ASIN4774110655.tit �ʤɤΥ���å��夬�Ǥ���Τ��衣
//        �����κǸ夬 01 �ξ�硢image ��������� noimage.jpg �ȤʤäƤ��ޤ��Х�������
//        1.0 ��Ƴ������ XML ���������Ϲ�®�������֤��������󤬥����ʤΤǡ�09 ������ʤ� 01 ��ȥ饤���롢�ǻ���Ū�˲�衣
//
// Caution!:
// * �������Ϣ����١�www.amazon.co.jp �Υ����������ȥץ������ǧ�ξ头���Ѳ�������
// * ��ӥ塼�ϡ�amazon �ץ饰���󤬸ƤӽФ��Խ����̤Ϥ⤦����� PukiWiki ����Ͽ����Ƥ���Τǡ�
//   ��ߤ���ʤ���ʸ�������ƥڡ����ι����ܥ���򲡤����ȡ�
// * ���� PLUGIN_AMAZON_AID��PROXY �����Ф���ʬ��expire ����ʬ��Ŭ�����Խ����ƻ��Ѥ��Ƥ�������(¾�Ϥ��ΤޤޤǤ� Ok)��
//
// Thanks to: Reimy and PukiWiki Developers Team
//

/////////////////////////////////////////////////
// Settings

// Amazon associate ID
//define('PLUGIN_AMAZON_AID',''); // None
define('PLUGIN_AMAZON_AID','');

// Expire caches per ? days
define('PLUGIN_AMAZON_EXPIRE_IMAGECACHE',   1);
define('PLUGIN_AMAZON_EXPIRE_TITLECACHE', 356);

// Alternative image for 'Image not found'
define('PLUGIN_AMAZON_NO_IMAGE', IMAGE_DIR . 'noimage.png');

// URI prefixes
switch(LANG){
case 'ja':
	// Amazon shop
	define('PLUGIN_AMAZON_SHOP_URI', 'http://www.amazon.co.jp/exec/obidos/ASIN/');

	// Amazon information inquiry (dev-t = default value in the manual)
	define('PLUGIN_AMAZON_XML', 'http://xml.amazon.co.jp/onca/xml3?t=webservices-20&' .
		'dev-t=GTYDRES564THU&type=lite&page=1&f=xml&locale=jp&AsinSearch=');
	break;
default:
	// Amazon shop
	define('PLUGIN_AMAZON_SHOP_URI', 'http://www.amazon.com/exec/obidos/ASIN/');

	// Amazon information inquiry (dev-t = default value in the manual)
	define('PLUGIN_AMAZON_XML', 'http://xml.amazon.com/onca/xml3?t=webservices-20&' .
		'dev-t=GTYDRES564THU&type=lite&page=1&f=xml&locale=us&AsinSearch=');
	break;
}

/////////////////////////////////////////////////

function plugin_amazon_init()
{
	global $amazon_aid, $amazon_body;

	if (PLUGIN_AMAZON_AID == '') {
		$amazon_aid = '';
	} else {
		$amazon_aid = PLUGIN_AMAZON_AID . '/';
	}
	$amazon_body = <<<EOD
-���: [[�����Խ��Τ���]]
-ɾ��: ��̾��
-����: &date;
**�������о�
[[�����Խ��Τ���]]

#amazon(,clear)
**����
[[�����Խ��Τ���]]

// �ޤ������Υ�ӥ塼��ߤ���硢��ʸ���������ڡ�����[�����ܥ���]�򲡤��Ƥ���������(PukiWiki �ˤϤ⤦��Ͽ����Ƥ��ޤ�)
// ³����ʤ顢��Ρ�[[�����Խ��Τ���]]��ʬ���̤�ޤ�ƺ��������ľ���Ƥ���������
// ��̾������ʬ�Ϥ���ʬ��̾�����ѹ����Ƥ�������������ȡ��׼ˡ��Ǥ���
// **�������оݡ�����ϡ��������Ԥ��ɲä��ʤ��Ǥ����������ܼ������˻��Ѥ���Τǡ�
// //�ǻϤޤ륳���ȹԤϡ��ǽ�Ū���������åȤ��Ƥ����������ܼ�������˺����Ǥ��ʤ���ǽ��������ޤ���
#comment
EOD;
}

function plugin_amazon_convert()
{
	global $script, $vars, $asin, $asin_all;

	if (func_num_args() > 3) {
		return '#amazon([ASIN-number][,left|,right]' .
			'[,book-title|,image|,delimage|,deltitle|,delete])';
	} else if (func_num_args() == 0) {
		// ��ӥ塼����
		$s_page = htmlspecialchars($vars['page']);
		if ($s_page == '') $s_page = $vars['refer'];
		$ret = <<<EOD
<form action="$script" method="post">
 <div>
  <input type="hidden" name="plugin" value="amazon" />
  <input type="hidden" name="refer" value="$s_page" />
  ASIN:
  <input type="text" name="asin" size="30" value="" />
  <input type="submit" value="��ӥ塼�Խ�" /> (ISBN 10 �� or ASIN 12 ��)
 </div>
</form>
EOD;
		return $ret;
	}

	$aryargs = func_get_args();

	$align = strtolower($aryargs[1]);
	if ($align == 'clear') return '<div style="clear:both"></div>'; // ��������
	if ($align != 'left') $align = 'right'; // ���ַ���

	$asin_all = htmlspecialchars($aryargs[0]);  // for XSS
	if (is_asin() == FALSE && $align != 'clear') return FALSE;

	if ($aryargs[2] != '') {
		// �����ȥ����
		$title = $alt = htmlspecialchars($aryargs[2]); // for XSS
		if ($alt == 'image') {
			$alt = plugin_amazon_get_asin_title();
			if ($alt == '') return FALSE;
			$title = '';
		} else if ($alt == 'delimage') {
			if (unlink(CACHE_DIR . 'ASIN' . $asin . '.jpg')) {
				return 'Image of ' . $asin . ' deleted...';
			} else {
				return 'Image of ' . $asin . ' NOT DELETED...';
			}
		} elseif ($alt == 'deltitle') {
			if (unlink(CACHE_DIR . 'ASIN' . $asin . '.tit')) {
				return 'Title of ' . $asin . ' deleted...';
			} else {
				return 'Title of ' . $asin . ' NOT DELETED...';
			}
		} elseif ($alt == 'delete') {
			if ((unlink(CACHE_DIR . 'ASIN' . $asin . '.jpg') &&
			     unlink(CACHE_DIR . 'ASIN' . $asin . '.tit'))) {
				return 'Title and Image of ' . $asin . ' deleted...';
			} else {
				return 'Title and Image of ' . $asin . ' NOT DELETED...';
			}
		}
	} else {
		// �����ȥ뼫ư����
		$alt = $title = plugin_amazon_get_asin_title();
		if ($alt == '') return FALSE;
	}

	return plugin_amazon_print_object($align, $alt, $title);
}

function plugin_amazon_action()
{
	global $vars, $script, $edit_auth, $edit_auth_users;
	global $amazon_body, $asin, $asin_all;

	$asin_all = htmlspecialchars(rawurlencode(strip_bracket($vars['asin'])));

	if (! is_asin()) {
		$retvars['msg']   = '�֥å���ӥ塼�Խ�';
		$retvars['refer'] = $vars['refer'];
		$retvars['body']  = plugin_amazon_convert();
		return $retvars;

	} else {
		$s_page     = $vars['refer'];
		$r_page     = $s_page . '/' . $asin;
		$r_page_url = rawurlencode($r_page);

		pkwk_headers_sent();
		if ($edit_auth && (! isset($_SERVER['PHP_AUTH_USER']) ||
		    ! array_key_exists($_SERVER['PHP_AUTH_USER'], $edit_auth_users) ||
			$edit_auth_users[$_SERVER['PHP_AUTH_USER']] != $_SERVER['PHP_AUTH_PW'])) {
			header('Location: ' . get_script_uri() . '?cmd=read&page=' . $r_page_url);
		} else {
			$title = plugin_amazon_get_asin_title();
			if ($title == '' || preg_match('/^\//', $s_page)) {
				header('Location: ' . get_script_uri() .
					'?cmd=read&page=' . encode($s_page));
			}
			$body = '#amazon(' . $asin_all . ',,image)' . "\n" .
				'*' . $title . "\n" . $amazon_body;
			plugin_amazon_review_save($r_page, $body);
			header('Location: ' . get_script_uri() . '?cmd=edit&page=' . $r_page_url);
		}
		exit;
	}
}

function plugin_amazon_inline()
{
	global $amazon_aid, $asin, $asin_all;

	list($asin_all) = func_get_args();

	$asin_all = htmlspecialchars($asin_all); // for XSS
	if (! is_asin()) return FALSE;

	$title = plugin_amazon_get_asin_title();
	if ($title == '') {
		return FALSE;
	} else {
		return '<a href="' . PLUGIN_AMAZON_SHOP_URI .
			$asin . '/' . $amazon_aid . 'ref=nosim">' . $title . '</a>' . "\n";
	}
}

function plugin_amazon_print_object($align, $alt, $title)
{
	global $amazon_aid;
	global $asin, $asin_ext, $asin_all;

	$url      = plugin_amazon_cache_image_fetch(CACHE_DIR);
	$url_shop = PLUGIN_AMAZON_SHOP_URI . $asin . '/' . $amazon_aid . 'ref=nosim';
	$center   = 'text-align:center';

	if ($title == '') {
		// Show image only
		$div  = '<div style="float:' . $align . ';margin:16px 16px 16px 16px;' . $center . '">' . "\n";
		$div .= ' <a href="' . $url_shop . '"><img src="' . $url . '" alt="' . $alt . '" /></a>' . "\n";
		$div .= '</div>' . "\n";

	} else {
		// Show image and title
		$div  = '<div style="float:' . $align . ';padding:.5em 1.5em .5em 1.5em;' . $center . '">' . "\n";
		$div .= ' <table style="width:110px;border:0;' . $center . '">' . "\n";
		$div .= '  <tr><td style="' . $center . '">' . "\n";
		$div .= '   <a href="' . $url_shop . '"><img src="' . $url . '" alt="' . $alt  .'" /></a></td></tr>' . "\n";
		$div .= '  <tr><td style="' . $center . '"><a href="' . $url_shop . '">' . $title . '</a></td></tr>' . "\n";
		$div .= ' </table>' . "\n";
		$div .= '</div>' . "\n";
	}
	return $div;
}

function plugin_amazon_get_asin_title()
{
	global $asin, $asin_ext, $asin_all;

	if ($asin_all == '') return '';

	$nocache = $nocachable = 0;

	$url = PLUGIN_AMAZON_XML . $asin;

	if (file_exists(CACHE_DIR) === FALSE || is_writable(CACHE_DIR) === FALSE) $nocachable = 1; // ����å����ԲĤξ��

	if (($title = plugin_amazon_cache_title_fetch(CACHE_DIR)) == FALSE) {
		$nocache = 1; // ����å��師�Ĥ��餺
		$body    = plugin_amazon_get_page($url); // �������ʤ��ΤǼ��ˤ���
		$tmpary  = array();
		$body    = mb_convert_encoding($body, SOURCE_ENCODING, 'UTF-8');
		preg_match('/<ProductName>([^<]*)</', $body, $tmpary);
		$title     = trim($tmpary[1]);
//		$tmpary[1] = '';
//		preg_match('#<ImageUrlMedium>http://images-jp.amazon.com/images/P/[^.]+\.(..)\.#',
//			$body, $tmpary);
//		if ($tmpary[1] != '') {
//			$asin_ext = $tmpary[1];
//			$asin_all = $asin . $asin_ext;
//		}
	}

	if ($title == '') {
		return '';
	} else {
		if ($nocache == 1 && $nocachable != 1)
			plugin_amazon_cache_title_save($title, CACHE_DIR);
		return $title;
	}
}

// �����ȥ륭��å��夬���뤫Ĵ�٤�
function plugin_amazon_cache_title_fetch($dir)
{
	global $asin, $asin_ext, $asin_all;

	$filename = $dir . 'ASIN' . $asin . '.tit';

	$get_tit = 0;
	if (! is_readable($filename)) {
		$get_tit = 1;
	} elseif (PLUGIN_AMAZON_EXPIRE_TITLECACHE * 3600 * 24 < time() - filemtime($filename)) {
		$get_tit = 1;
	}

	if ($get_tit) return FALSE;

	if (($fp = @fopen($filename, 'r')) === FALSE) return FALSE;
	$title = fgets($fp, 4096);
//	$tmp_ext = fgets($fp, 4096);
//	if ($tmp_ext != '') $asin_ext = $tmp_ext;
	fclose($fp);

	if (strlen($title) > 0) {
		return $title;
	} else {
		return FALSE;
	}
}

// ��������å��夬���뤫Ĵ�٤�
function plugin_amazon_cache_image_fetch($dir)
{
	global $asin, $asin_ext, $asin_all;

	$filename = $dir . 'ASIN' . $asin . '.jpg';

	$get_img = 0;
	if (! is_readable($filename)) {
		$get_img = 1;
	} elseif (PLUGIN_AMAZON_EXPIRE_IMAGECACHE * 3600 * 24 < time() - filemtime($filename)) {
		$get_img = 1;
	}

	if ($get_img) {
		$url = 'http://images-jp.amazon.com/images/P/' . $asin . '.' . $asin_ext . '.MZZZZZZZ.jpg';
		if (! is_url($url)) return FALSE;

		$body = plugin_amazon_get_page($url);
		if ($body != '') {
			$tmpfile = $dir . 'ASIN' . $asin . '.jpg.0';
			$fp = fopen($tmpfile, 'wb');
			fwrite($fp, $body);
			fclose($fp);
			$size = getimagesize($tmpfile);
			unlink($tmpfile);
		}
		if ($body == '' || $size[1] <= 1) { // �̾��1���֤뤬ǰ�Τ���0�ξ���(reimy)
			// ����å���� PLUGIN_AMAZON_NO_IMAGE �Υ��ԡ��Ȥ���
			if ($asin_ext == '09') {
				$url = 'http://images-jp.amazon.com/images/P/' . $asin . '.01.MZZZZZZZ.jpg';
				$body = plugin_amazon_get_page($url);
				if ($body != '') {
					$tmpfile = $dir . 'ASIN' . $asin . '.jpg.0';
					$fp = fopen($tmpfile, 'wb');
					fwrite($fp, $body);
					fclose($fp);
					$size = getimagesize($tmpfile);
					unlink($tmpfile);
				}
			}
			if ($body == '' || $size[1] <= 1) {
				$fp = fopen(PLUGIN_AMAZON_NO_IMAGE, 'rb');
				if (! $fp) return FALSE;
				
				$body = '';
				while (! feof($fp)) $body .= fread($fp, 4096);
				fclose ($fp);
			}
		}
		plugin_amazon_cache_image_save($body, CACHE_DIR);
	}
	return $filename;
}

// Save title cache
function plugin_amazon_cache_title_save($data, $dir)
{
	global $asin, $asin_ext, $asin_all;

	$filename = $dir . 'ASIN' . $asin . '.tit';
	$fp = fopen($filename, 'w');
	fwrite($fp, $data);
	fclose($fp);

	return $filename;
}

// Save image cache
function plugin_amazon_cache_image_save($data, $dir)
{
	global $asin, $asin_ext, $asin_all;

	$filename = $dir . 'ASIN' . $asin . '.jpg';
	$fp = fopen($filename, 'wb');
	fwrite($fp, $data);
	fclose($fp);

	return $filename;
}

// Save book data
function plugin_amazon_review_save($page, $data)
{
	global $asin, $asin_ext, $asin_all;

	$filename = DATA_DIR . encode($page) . '.txt';
	if (! is_readable($filename)) {
		$fp = fopen($filename, 'w');
		fwrite($fp, $data);
		fclose($fp);
		return TRUE;
	} else {
		return FALSE;
	}
}

function plugin_amazon_get_page($url)
{
	$data = http_request($url);
	return ($data['rc'] == 200) ? $data['data'] : '';
}

// is ASIN?
function is_asin()
{
	global $asin, $asin_ext, $asin_all;

	$tmpary = array();
	if (preg_match('/^([A-Z0-9]{10}).?([0-9][0-9])?$/', $asin_all, $tmpary) == FALSE) {
		return FALSE;
	} else {
		$asin     = $tmpary[1];
		$asin_ext = $tmpary[2];
		if ($asin_ext == '') $asin_ext = '09';
		$asin_all = $asin . $asin_ext;
		return TRUE;
	}
}
?>
