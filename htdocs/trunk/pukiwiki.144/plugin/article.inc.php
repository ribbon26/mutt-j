<?php
 /*

 PukiWiki BBS���ץ饰����

 CopyRight 2002 OKAWARA,Satoshi
 http://www.dml.co.jp/~kawara/pukiwiki/pukiwiki.php
 kawara@dml.co.jp

 ��å��������ѹ�����������LANGUAGE�ե�����˲������ͤ��ɲä��Ƥ��餴���Ѥ�������
	$_btn_name = '��̾��';
	$_btn_article = '���������';
	$_btn_subject = '��̾: ';

 ��$_btn_name��comment�ץ饰����Ǵ������ꤵ��Ƥ����礬����ޤ�

 ������Ƥμ�ư�᡼��ž����ǽ�򤴻��Ѥˤʤꤿ������
 -������ƤΥ᡼�뼫ư�ۿ�
 -������ƤΥ᡼�뼫ư�ۿ���
 ������ξ塢�����Ѥ���������

 $Id: article.inc.php,v 1.19 2004/07/31 03:09:20 henoheno Exp $

 */

global $_mailto;

/////////////////////////////////////////////////
// �ƥ����ȥ��ꥢ�Υ�����
define('article_COLS', 70);
/////////////////////////////////////////////////
// �ƥ����ȥ��ꥢ�ιԿ�
define('article_ROWS', 5);
/////////////////////////////////////////////////
// ̾���ƥ����ȥ��ꥢ�Υ�����
define('NAME_COLS', 24);
/////////////////////////////////////////////////
// ��̾�ƥ����ȥ��ꥢ�Υ�����
define('SUBJECT_COLS', 60);
/////////////////////////////////////////////////
// ̾���������ե����ޥå�
define('NAME_FORMAT', '[[$name]]');
/////////////////////////////////////////////////
// ��̾�������ե����ޥå�
define('SUBJECT_FORMAT', '**$subject');
/////////////////////////////////////////////////
// ����������� 1:����� 0:��θ�
define('ARTICLE_INS', 0);
/////////////////////////////////////////////////
// �񤭹��ߤβ��˰�ԥ����Ȥ������ 1:����� 0:����ʤ�
define('ARTICLE_COMMENT', 1);
/////////////////////////////////////////////////
// ���Ԥ�ưŪ�Ѵ� 1:���� 0:���ʤ�
define('ARTICLE_AUTO_BR', 1);

/////////////////////////////////////////////////
// ������ƤΥ᡼�뼫ư�ۿ� 1:���� 0:���ʤ�
define('MAIL_AUTO_SEND', 0);
/////////////////////////////////////////////////
// ������ƤΥ᡼���������������ԥ᡼�륢�ɥ쥹
define('MAIL_FROM', '');
/////////////////////////////////////////////////
// ������ƤΥ᡼������������̾
define('MAIL_SUBJECT_PREFIX', '[someone\'sPukiWiki]');
/////////////////////////////////////////////////
// ������ƤΥ᡼�뼫ư�ۿ���
$_mailto = array (
	''
);

function plugin_article_action()
{
	global $script, $post, $vars, $cols, $rows, $now;
	global $_title_collided, $_msg_collided, $_title_updated;
	global $_mailto, $_no_subject, $_no_name;

	if ($post['msg'] == '') {
		return array('msg'=>'','body'=>'');
	}

	$name = ($post['name'] == '') ? $_no_name : $post['name'];
	$name = ($name == '') ? '' : str_replace('$name', $name, NAME_FORMAT);
	$subject = ($post['subject'] == '') ? $_no_subject : $post['subject'];
	$subject = ($subject == '') ? '' : str_replace('$subject', $subject, SUBJECT_FORMAT);
	$article  = "$subject\n>$name ($now)~\n~\n";

	$msg = rtrim($post['msg']);
	if (ARTICLE_AUTO_BR) {
		//���Ԥμ�갷���Ϥ��ä�������ä�URL�������Ȥ��ϡ�
		//�����ȹԡ������Ѥ߹Ԥˤ�~��Ĥ��ʤ��褦�� arino
		$msg = join("\n", preg_replace('/^(?!\/\/)(?!\s)(.*)$/', '$1~', explode("\n", $msg)));
	}
	$article .= "$msg\n\n//";

	if (ARTICLE_COMMENT) {
		$article .= "\n\n#comment\n";
	}

	$postdata = '';
	$postdata_old  = get_source($post['refer']);
	$article_no = 0;

	foreach($postdata_old as $line) {
		if (! ARTICLE_INS) {
			$postdata .= $line;
		}
		if (preg_match('/^#article/', $line)) {
			if ($article_no == $post['article_no'] && $post['msg'] != '') {
				$postdata .= "$article\n";
			}
			$article_no++;
		}
		if (ARTICLE_INS) {
			$postdata .= $line;
		}
	}

	$postdata_input = "$article\n";
	$body = '';

	if (md5(@join('', get_source($post['refer']))) != $post['digest']) {
		$title = $_title_collided;

		$body = "$_msg_collided\n";

		$s_refer = htmlspecialchars($post['refer']);
		$s_digest = htmlspecialchars($post['digest']);
		$s_postdata = htmlspecialchars($postdata_input);
		$body .= <<<EOD
<form action="$script?cmd=preview" method="post">
 <div>
  <input type="hidden" name="refer" value="$s_refer" />
  <input type="hidden" name="digest" value="$s_digest" />
  <textarea name="msg" rows="$rows" cols="$cols" id="textarea">$s_postdata</textarea><br />
 </div>
</form>
EOD;
	}
	else {
		page_write($post['refer'], trim($postdata));

		// ������ƤΥ᡼�뼫ư����
		if (MAIL_AUTO_SEND) {
			$mailaddress = implode(',', $_mailto);
			$mailsubject = MAIL_SUBJECT_PREFIX . ' ' . str_replace('**', '', $subject);
			if ($post['name']) {
				$mailsubject .= '/' . $post['name'];
			}
			$mailsubject = mb_encode_mimeheader($mailsubject);

			$mailbody = $post['msg'];
			$mailbody .= "\n\n---\n";
			$mailbody .= $_msg_article_mail_sender . $post['name'] . " ($now)\n";
			$mailbody .= $_msg_article_mail_page . $post['refer'] . "\n";
			$mailbody .= '�� URL: ' . $script . '?' . rawurlencode($post['refer']) . "\n";
			$mailbody = mb_convert_encoding( $mailbody, "JIS" );

			$mailaddheader = "From: " . MAIL_FROM;

			mail($mailaddress, $mailsubject, $mailbody, $mailaddheader);
		}

		$title = $_title_updated;
	}
	$retvars['msg'] = $title;
	$retvars['body'] = $body;

	$post['page'] = $post['refer'];
	$vars['page'] = $post['refer'];

	return $retvars;
}

function plugin_article_convert()
{
	global $script, $vars, $digest;
	global $_btn_article, $_btn_name, $_btn_subject;
	static $numbers = array();

	if (! array_key_exists($vars['page'], $numbers))
	{
		$numbers[$vars['page']] = 0;
	}
	$article_no = $numbers[$vars['page']]++;

	$s_page = htmlspecialchars($vars['page']);
	$s_digest = htmlspecialchars($digest);
	$name_cols = NAME_COLS;
	$subject_cols = SUBJECT_COLS;
	$article_rows = article_ROWS;
	$article_cols = article_COLS;
	$string = <<<EOD
<form action="$script" method="post">
 <div>
  <input type="hidden" name="article_no" value="$article_no" />
  <input type="hidden" name="plugin" value="article" />
  <input type="hidden" name="digest" value="$s_digest" />
  <input type="hidden" name="refer" value="$s_page" />
  $_btn_name <input type="text" name="name" size="$name_cols" /><br />
  $_btn_subject <input type="text" name="subject" size="$subject_cols" /><br />
  <textarea name="msg" rows="$article_rows" cols="$article_cols">\n</textarea><br />
  <input type="submit" name="article" value="$_btn_article" />
 </div>
</form>
EOD;

	return $string;
}
?>
