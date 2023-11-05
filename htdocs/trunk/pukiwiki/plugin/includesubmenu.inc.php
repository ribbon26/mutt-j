<?php
// $Id: includesubmenu.inc.php,v 1.5 2004/07/31 03:09:20 henoheno Exp $

function plugin_includesubmenu_convert()
{
  global $script,$vars;

  $ShowPageName = FALSE;

  if (func_num_args()) {
    $aryargs = func_get_args();
    if ($aryargs[0] == 'showpagename') {
      $ShowPageName = TRUE;
    }
  }

  $SubMenuPageName = '';

  $tmppage = strip_bracket($vars['page']);
  //�����ؤ�SubMenu�ڡ���̾
  $SubMenuPageName1 = $tmppage . '/SubMenu';

  //Ʊ���ؤ�SubMenu�ڡ���̾
  $LastSlash= strrpos($tmppage,'/');
  if ($LastSlash === FALSE) {
    $SubMenuPageName2 = 'SubMenu';
  } else {
    $SubMenuPageName2 = substr($tmppage,0,$LastSlash) . '/SubMenu';
  }
  //echo "$SubMenuPageName1 <br>";
  //echo "$SubMenuPageName2 <br>";
  //�����ؤ�SubMenu�����뤫�����å�
  //����С���������
  if (is_page($SubMenuPageName1)) {
    //�����ؤ�SubMenuͭ��
    $SubMenuPageName = $SubMenuPageName1;
  }
  else if (is_page($SubMenuPageName2)) {
    //Ʊ���ؤ�SubMenuͭ��
    $SubMenuPageName = $SubMenuPageName2;
  }
  else {
    //SubMenu̵��
    return "";
  }

  $body = convert_html(get_source($SubMenuPageName));

  if ($ShowPageName) {
    $r_page = rawurlencode($SubMenuPageName);
    $s_page = htmlspecialchars($SubMenuPageName);
    $link = "<a href=\"$script?cmd=edit&amp;page=$r_page\">$s_page</a>";
    $body = "<h1>$link</h1>\n$body";
  }
  return $body;
}
?>
