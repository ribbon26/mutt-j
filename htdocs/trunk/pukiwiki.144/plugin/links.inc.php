<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: links.inc.php,v 1.20 2004/07/31 03:09:20 henoheno Exp $
//

// ��å���������
function plugin_links_init()
{
	$messages = array(
		'_links_messages'=>array(
			'title_update'  => '����å��幹��',
			'msg_adminpass' => '�����ԥѥ����',
			'btn_submit'    => '�¹�',
			'msg_done'      => '����å���ι�������λ���ޤ�����',
			'msg_usage'     => "
* ��������

:����å���򹹿�|
���ƤΥڡ����򥹥���󤷡�����ڡ������ɤΥڡ��������󥯤���Ƥ��뤫��Ĵ�����ơ�����å���˵�Ͽ���ޤ���

* ���
�¹ԤˤϿ�ʬ��������⤢��ޤ����¹ԥܥ���򲡤������ȡ����Ф餯���Ԥ�����������

* �¹�
�����ԥѥ���ɤ����Ϥ��ơ�[�¹�]�ܥ���򥯥�å����Ƥ���������
"
		)
	);
	set_plugin_messages($messages);
}

function plugin_links_action()
{
	global $script, $post, $vars, $foot_explain;
	global $_links_messages;

	if (empty($vars['action']) or empty($post['adminpass']) or ! pkwk_login($post['adminpass']))
	{
		$body = convert_html($_links_messages['msg_usage']);
		$body .= <<<EOD
<form method="POST" action="$script">
 <div>
  <input type="hidden" name="plugin" value="links" />
  <input type="hidden" name="action" value="update" />
  {$_links_messages['msg_adminpass']}
  <input type="password" name="adminpass" size="20" value="" />
  <input type="submit" value="{$_links_messages['btn_submit']}" />
 </div>
</form>
EOD;
		return array(
			'msg'=>$_links_messages['title_update'],
			'body'=>$body
		);
	}
	else if ($vars['action'] == 'update')
	{
		links_init();

		// ������ˤ���
		$foot_explain = array();
		return array(
			'msg'=>$_links_messages['title_update'],
			'body'=>$_links_messages['msg_done']
		);
	}

	return array(
		'msg'=>$_links_messages['title_update'],
		'body'=>$_links_messages['err_invalid']
	);
}
?>
