<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// Id: amazon.inc.php,v 1.1 2003/07/24 13:00:00 �׼�
//
// Copyright: 2003 By �׼� <raku@rakunet.org>
//
// Thanks: To reimy and PukiWiki Developers Team.
//
// ChangeLog:
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
// Todo:
// *  ���ʤꥴ���㥴���㤷�Ƥ����Τǡ�������ɬ�פ��ꡣ
//
//       GNU/GPL �ˤ������ä����ۤ��롣
//
// Caution!:
// * �������Ϣ����١�www.amazon.co.jp �Υ����������ȥץ������ǧ�ξ头���Ѳ�������
// * ��ӥ塼�ϡ�amazon �ץ饰���󤬸ƤӽФ��Խ����̤Ϥ⤦����� PukiWiki ����Ͽ����Ƥ���Τǡ�
//   ��ߤ���ʤ���ʸ�������ƥڡ����ι����ܥ���򲡤����ȡ�
// * ���� AMAZON_AID��PROXY �����Ф���ʬ��expire ����ʬ��Ŭ�����Խ����ƻ��Ѥ��Ƥ�������(¾�Ϥ��ΤޤޤǤ� Ok)��
//

/////////////////////////////////////////////////
// $Id: amazon.inc.php,v 1.2 2004/07/31 03:09:19 henoheno Exp $
// - modified by arino <arino@users.sourceforge.jp>
// -- replace plugin_amazon_get_page().
// -- AMAZON_XML 'xml.amazon.com' -> 'xml.amazon.co.jp'
//

/////////////////////////////////////////////////
// amazon �Υ����������� ID(�ʤ��ʤ� ���̥桼��)
define('AMAZON_AID','');
/////////////////////////////////////////////////
// expire ����/�����ȥ륭��å�������Ǻ�����뤫
define('AMAZON_EXPIRE_IMG',1);
define('AMAZON_EXPIRE_TIT',356);
/////////////////////////////////////////////////
// �����ʤ��ξ��β���
define('NO_IMAGE','./image/noimage.jpg');
/////////////////////////////////////////////////
// amazon ����å�
define('AMAZON_SHOP','http://www.amazon.co.jp/exec/obidos/ASIN/');
/////////////////////////////////////////////////
// amazon ���ʾ�����礻 URI(dev-t �ϥޥ˥奢��Υǥ��ե������)
define('AMAZON_XML','http://xml.amazon.co.jp/onca/xml3?t=webservices-20&dev-t=GTYDRES564THU&type=lite&page=1&f=xml&locale=jp&AsinSearch=');

function plugin_amazon_init() {
  global $amazon_aid, $amazon_body;

  if (AMAZON_AID == '') {
    $amazon_aid = '';
  } else {
    $amazon_aid = AMAZON_AID . '/';
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

function plugin_amazon_convert() {
  global $script, $vars;
  global $asin, $asin_ext, $asin_all;

  if (func_num_args() == 0) { // ��ӥ塼����
    $s_page = htmlspecialchars($vars['page']);
    if ($s_page == '') {
      $s_page = $vars['refer'];
    }
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
  } elseif (func_num_args() < 1 or func_num_args() > 3) {
    return false;
  }
  $aryargs = func_get_args();

  $align = strtolower($aryargs[1]);
  if ($align == 'clear') return '<div style="clear:both"></div>'; // ��������
  if ($align != 'left') $align = 'right'; // ���ַ���

  $asin_all = htmlspecialchars($aryargs[0]);  // for XSS
  if (is_asin() == false and $align != 'clear') return false;

  if ($aryargs[2] != '') { // �����ȥ���꤫��ư������
    $title = $alt = htmlspecialchars($aryargs[2]); // for XSS
    if ($alt == 'image') {
      $alt = plugin_amazon_get_asin_title();
      if ($alt == '') return false;
      $title = "";
    } elseif ($alt == 'delimage') {
      if (unlink(CACHE_DIR . "ASIN" . $asin . ".jpg")) {
        return "Image of $asin deleted...";
      } else {
        return "Image of $asin NOT DELETED...";
      }
    } elseif ($alt == 'deltitle') {
      if (unlink(CACHE_DIR . "ASIN" . $asin . ".tit")) {
        return "Title of $asin deleted...";
      } else {
        return "Title of $asin NOT DELETED...";
      }
    } elseif ($alt == 'delete') {
      if ((unlink(CACHE_DIR . "ASIN" . $asin . ".jpg") and unlink(CACHE_DIR . "ASIN" . $asin . ".tit"))) {
        return "Title and Image of $asin deleted...";
      } else {
        return "Title and Image of $asin NOT DELETED...";
      }
    }
  } else {
    $alt = $title = plugin_amazon_get_asin_title(); // �����ȥ뼫ư����
    if ($alt == '') return false;
  }

  return plugin_amazon_print_object($align, $alt, $title);
}

function plugin_amazon_action() {
  global $vars,$script, $_btn_edit, $edit_auth, $edit_auth_users, $_msg_auth;
  global $amazon_body;
  global $asin, $asin_ext, $asin_all;

  $asin_all = htmlspecialchars(rawurlencode(strip_bracket($vars['asin'])));
  if (! is_asin()) {
    $retvars['msg'] = "�֥å���ӥ塼�Խ�";
    $retvars['refer'] = $vars['refer'];
    $s_page = $vars['refer'];
    $r_page = $s_page . '/' . $asin;
    $retvars['body'] = plugin_amazon_convert();
    return $retvars;
  }

  $s_page = $vars['refer'];
  $r_page = $s_page . '/' . $asin;
  $r_page_url = rawurlencode($r_page);

  if ($edit_auth and (!isset($_SERVER['PHP_AUTH_USER']) or !array_key_exists($_SERVER['PHP_AUTH_USER'],$edit_auth_users) or
      $edit_auth_users[$_SERVER['PHP_AUTH_USER']] != $_SERVER['PHP_AUTH_PW'])) {
    header("Location: $script?cmd=read&page=$r_page_url");
  } else {
    $title = plugin_amazon_get_asin_title();
    if ($title == '' or preg_match('/^\//', $s_page)) {
      header("Location: $script?cmd=read&page=" . encode($s_page));
    }
    $body = "#amazon($asin_all,,image)\n*$title\n" . $amazon_body;
    plugin_amazon_review_save($r_page, $body);
    header("Location: $script?cmd=edit&page=$r_page_url");
  }
  die();
}

function plugin_amazon_inline() {
  global $amazon_aid;
  global $asin, $asin_ext, $asin_all;

  list($asin_all) = func_get_args();

  $asin_all = htmlspecialchars($asin_all); // for XSS
  if (! is_asin()) return false;

  $title = plugin_amazon_get_asin_title();
  if ($title == '')
    return false;
  else
    return '<a href="' . AMAZON_SHOP . "$asin/$amazon_aid" . 'ref=nosim">' . "$title</a>\n";
}

function plugin_amazon_print_object($align, $alt, $title) {
  global $amazon_aid;
  global $asin, $asin_ext, $asin_all;

  $url = plugin_amazon_cache_image_fetch(CACHE_DIR);

  if ($title == '') { // �����ȥ뤬�ʤ���С������Τ�ɽ��
    $div = "<div style=\"float:$align;margin:16px 16px 16px 16px;text-align:center\">\n";
    $div .= ' <a href="' . AMAZON_SHOP . "$asin/" . $amazon_aid . 'ref=nosim">' . "<img src=\"$url\" alt=\"$alt\" /></a>\n";
    $div .= "</div>\n";
  } else {	      // �̾�ɽ��
    $div = "<div style=\"float:$align;padding:.5em 1.5em .5em 1.5em;text-align:center\">\n";
    $div .= " <table style=\"width:110px;border:0;text-align:center\"><tr><td style=\"text-align:center\">\n";
    $div .= '  <a href="' . AMAZON_SHOP . "$asin/$amazon_aid" . 'ref=nosim">' . "<img src=\"$url\" alt=\"$alt\" /></a></td></tr>\n";
    $div .= '  <tr><td style="text-align:center"><a href="' . AMAZON_SHOP . "$asin/$amazon_aid" . 'ref=nosim">' . "$title</a></td>\n";
    $div .= " </tr></table>\n</div>\n";
  }
  return $div;
}

function plugin_amazon_get_asin_title() {
  global $asin, $asin_ext, $asin_all;

  if ($asin_all == '') return "";

  $nocache = $nocachable = 0;
  $title = '';

  $url = AMAZON_XML . $asin;

  if (file_exists(CACHE_DIR) === false or is_writable(CACHE_DIR) === false) $nocachable = 1; // ����å����ԲĤξ��

  if (($title = plugin_amazon_cache_title_fetch(CACHE_DIR)) == false) {
    $nocache = 1; // ����å��師�Ĥ��餺
    $body = plugin_amazon_get_page($url); // �������ʤ��ΤǼ��ˤ���
    $tmpary = array();
    $body = mb_convert_encoding($body,SOURCE_ENCODING,"UTF-8");
    preg_match('/<ProductName>([^<]*)</', $body, $tmpary);
    $title = trim($tmpary[1]);
//    $tmpary[1] = '';
//    preg_match("/<ImageUrlMedium>http:\/\/images-jp.amazon.com\/images\/P\/[^.]+\.(..)\./", $body, $tmpary);
//    if ($tmpary[1] != '') {
//      $asin_ext = $tmpary[1];
//      $asin_all = "$asin.$asin_ext";
//    }
  }

  if ($title == '') return '';

  if ($nocache == 1 and $nocachable != 1) plugin_amazon_cache_title_save($title, CACHE_DIR); // �����ȥ뤬����Х���å������¸
  return $title;
}

// �����ȥ륭��å��夬���뤫Ĵ�٤�
function plugin_amazon_cache_title_fetch($dir) {
  global $asin, $asin_ext, $asin_all;

  $filename = $dir . "ASIN" . $asin . ".tit";

  $get_tit = 0;
  if (!is_readable($filename)) {
    $get_tit = 1;
  } elseif (AMAZON_EXPIRE_TIT * 3600 * 24 < time() - filemtime($filename)) {
    $get_tit = 1;
  }

  if ($get_tit) return false;

  if (!($fp = @fopen($filename, "r"))) return false;
  $title = fgets($fp, 4096);
//  $tmp_ext = fgets($fp, 4096);
//  if ($tmp_ext != '') {
//    $asin_ext = $tmp_ext;
//  }
  fclose($fp);

  if (strlen($title) > 0)
    return $title;
  else
    return false;
}

// ��������å��夬���뤫Ĵ�٤�
function plugin_amazon_cache_image_fetch($dir) {
  global $asin, $asin_ext, $asin_all;

  $filename = $dir . "ASIN" . $asin . ".jpg";

  $get_img = 0;
  if (!is_readable($filename)) {
    $get_img = 1;
  } elseif (AMAZON_EXPIRE_IMG * 3600 * 24 < time() - filemtime($filename)) {
    $get_img = 1;
  }

  if ($get_img) {
    $url = "http://images-jp.amazon.com/images/P/$asin.$asin_ext.MZZZZZZZ.jpg";

    if (!is_url($url)) return false; // URL ���������å�
    $body = plugin_amazon_get_page($url);
    if ($body != "") {
      $tmpfile = $dir . "ASIN$asin.jpg.0";
      $fp = fopen($tmpfile, "wb");
      fwrite($fp, $body);
      fclose($fp);
      $size = getimagesize($tmpfile);
      unlink($tmpfile);
    }
    if ($body == "" or $size[1] <= 1) { // �̾��1���֤뤬ǰ�Τ���0�ξ���(reimy)
      // ����å���� NO_IMAGE �Υ��ԡ��Ȥ���
      if ($asin_ext == "09") {
        $url = "http://images-jp.amazon.com/images/P/$asin.01.MZZZZZZZ.jpg";
        $body = plugin_amazon_get_page($url);
	if ($body != "") {
	  $tmpfile = $dir . "ASIN$asin.jpg.0";
	  $fp = fopen($tmpfile, "wb");
	  fwrite($fp, $body);
	  fclose($fp);
	  $size = getimagesize($tmpfile);
	  unlink($tmpfile);
	}
      }
      if ($body == "" or $size[1] <= 1) {
        $fp = fopen(NO_IMAGE, "rb");
        if (! $fp) return false;
        $body = "";
        while (!feof($fp)) {
          $body .= fread($fp, 4096);
        }
        fclose ($fp);
      }
    }
    plugin_amazon_cache_image_save($body, CACHE_DIR);
  }
  return $filename;
}

// �����ȥ륭��å������¸
function plugin_amazon_cache_title_save($data, $dir) {
  global $asin, $asin_ext, $asin_all;

  $filename = $dir . "ASIN" . $asin . ".tit";

  $fp = fopen($filename, "w");
  fwrite($fp, "$data");
  fclose($fp);

  return $filename;
}

// ��������å������¸
function plugin_amazon_cache_image_save($data, $dir) {
  global $asin, $asin_ext, $asin_all;

  $filename = $dir . "ASIN" . $asin . ".jpg";

  $fp = fopen($filename, "wb");
  fwrite($fp, $data);
  fclose($fp);

  return $filename;
}

// ���ҥǡ�������¸
function plugin_amazon_review_save($page, $data) {
  global $asin, $asin_ext, $asin_all;

  $filename = DATA_DIR . encode($page) . ".txt";

  if (!is_readable($filename)) {
    $fp = fopen($filename, "w");
    fwrite($fp, $data);
    fclose($fp);
    return true;
  }
  return false;
}

// �ͥåȾ�� URL �Υǡ������äƤ����֤�(�ʤ���ж��ǡ���)
function plugin_amazon_get_page($url)
{
	$data = http_request($url);

	return ($data['rc'] == 200) ? $data['data'] : '';
}

// ASIN����
function is_asin() {
  global $asin, $asin_ext, $asin_all;

  $tmpary = array();
  if (preg_match("/^([A-Z0-9]{10}).?([0-9][0-9])?$/", $asin_all, $tmpary) == false) {
    return false;
  } else {
    $asin = $tmpary[1];
    $asin_ext = $tmpary[2];
    if ($asin_ext == '') {
      $asin_ext = "09";
    }
    $asin_all = "$asin.$asin_ext";
    return true;
  }
}

?>
