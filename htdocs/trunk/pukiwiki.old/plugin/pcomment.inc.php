<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: pcomment.inc.php,v 1.26 2003/10/01 07:00:34 arino Exp $
//

/*

*�ץ饰���� pcomment
���ꤷ���ڡ����˥����Ȥ�����

*Usage
 #pcomment([�ڡ���̾][,ɽ�����륳���ȿ�][,���ץ����])

*�ѥ�᡼��
-�ڡ���̾~
 ��Ƥ��줿�����Ȥ�Ͽ����ڡ�����̾��
-ɽ�����륳���ȿ�~
 ���Υ����Ȥ򲿷�ɽ�����뤫(0������)

*���ץ����
-above~
 �����Ȥ�ե�����ɤ�����ɽ��(��������������)
-below~
 �����Ȥ�ե�����ɤθ��ɽ��(��������������)
-reply~
 2��٥�ޤǤΥ����Ȥ˥�ץ饤��Ĥ���radio�ܥ����ɽ��

*/
// �ڡ���̾�Υǥե����(%s��$vars['page']������)
define('PCMT_PAGE','[[������/%s]]');
//
// ɽ�����륳���ȿ��Υǥե����
define('PCMT_NUM_COMMENTS',10);
//
// �����Ȥ�̾���ƥ����ȥ��ꥢ�Υ�����
define('PCMT_COLS_NAME',15);
//
// �����ȤΥƥ����ȥ��ꥢ�Υ�����
define('PCMT_COLS_COMMENT',70);
//
// ����������� 1:���� 0:��Ƭ
define('PCMT_INSERT_INS',1);
//
//�����Ȥ������ե����ޥå�
define('PCMT_NAME_FORMAT','[[$name]]');
define('PCMT_MSG_FORMAT','$msg');
define('PCMT_NOW_FORMAT','&new{$now};');
// \x08�ϡ���Ƥ��줿ʸ������˸���ʤ�ʸ���Ǥ���Фʤ�Ǥ⤤����
define('PCMT_FORMAT',"\x08MSG\x08 -- \x08NAME\x08 \x08DATE\x08");
//
// ��ư������ 1�ڡ���������η������� 0��̵��
define('PCMT_AUTO_LOG',0);
// �����ȥڡ����Υ����ॹ����פ򹹿����������֥ڡ����Υ����ॹ����פ򹹿�����
define('PCMT_TIMESTAMP',0);

function plugin_pcomment_action()
{
	global $script,$post,$vars;
	
	if ($post['msg'] == '')
	{
		return array();
	}
	
	$retval = pcmt_insert();
	
	if ($retval['collided'])
	{
		$vars['page'] = $post['page'] = $post['refer'];
		return $retval;
	}
	
	header("Location: $script?".rawurlencode($post['refer']));
	exit;
}

function plugin_pcomment_convert()
{
	global $script,$vars;
	global $_pcmt_messages;
	
	//�����
	$ret = '';
	
	//�ѥ�᡼���Ѵ�
	$params = array(
		'noname'=>FALSE,
		'nodate'=>FALSE,
		'below' =>FALSE,
		'above' =>FALSE,
		'reply' =>FALSE,
		'_args' =>array()
	);
	array_walk(func_get_args(), 'pcmt_check_arg', &$params);
	
	//ʸ��������
	$page = array_key_exists(0,$params['_args']) ? $params['_args'][0] : '';
	$count = array_key_exists(1,$params['_args']) ? $params['_args'][1] : 0;
	
	if ($page == '')
	{
		$page = sprintf(PCMT_PAGE,strip_bracket($vars['page']));
	}
	
	$_page = get_fullname(strip_bracket($page),$vars['page']);
	if (!is_pagename($_page))
	{
		return sprintf($_pcmt_messages['err_pagename'],htmlspecialchars($_page));
	}
	if ($count == 0 and $count !== '0')
	{
		$count = PCMT_NUM_COMMENTS;
	}
	
	//���������
	$dir = PCMT_INSERT_INS;
	if ($params['above'])
	{
		$dir = 1;
	}
	if ($params['below']) //ξ�����ꤵ�줿�鲼�� (^^;
	{
		$dir = 0;
	}
	
	//�����Ȥ����
	list($comments, $digest) = pcmt_get_comments($_page,$count,$dir,$params['reply']);
	
	//�ե������ɽ��
	if ($params['noname'])
	{
		$title = $_pcmt_messages['msg_comment'];
		$name = '';
	}
	else
	{
		$title = $_pcmt_messages['btn_name'];
		$name = '<input type="text" name="name" size="'.PCMT_COLS_NAME.'" />';
	}
	
	$radio = $params['reply'] ? '<input type="radio" name="reply" value="0" tabindex="0" checked="checked" />' : '';
	$comment = '<input type="text" name="msg" size="'.PCMT_COLS_COMMENT.'" />';
	
	//XSS�ȼ������� - ���������褿�ѿ��򥨥�������
	$s_page = htmlspecialchars($page);
	$s_refer = htmlspecialchars($vars['page']);
	$s_nodate = htmlspecialchars($params['nodate']);
	$s_count = htmlspecialchars($count);
	
	$form = <<<EOD
  <div>
  <input type="hidden" name="digest" value="$digest" />
  <input type="hidden" name="plugin" value="pcomment" />
  <input type="hidden" name="refer" value="$s_refer" />
  <input type="hidden" name="page" value="$s_page" />
  <input type="hidden" name="nodate" value="$s_nodate" />
  <input type="hidden" name="dir" value="$dir" />
  <input type="hidden" name="count" value="$count" />
  $radio $title $name $comment
  <input type="submit" value="{$_pcmt_messages['btn_comment']}" />
  </div>
EOD;
	if (!is_page($_page))
	{
		$link = make_pagelink($_page);
		$recent = $_pcmt_messages['msg_none'];
	}
	else
	{
		$msg = ($_pcmt_messages['msg_all'] != '') ? $_pcmt_messages['msg_all'] : $_page;
		$link = make_pagelink($_page,$msg);
		$recent = ($count > 0) ? sprintf($_pcmt_messages['msg_recent'],$count) : '';
	}
	
	return $dir ?
		"<div><p>$recent $link</p>\n<form action=\"$script\" method=\"post\">$comments$form</form></div>" :
		"<div><form action=\"$script\" method=\"post\">$form$comments</form>\n<p>$recent $link</p></div>";
}

function pcmt_insert()
{
	global $script,$vars,$post,$now;
	global $_title_updated,$_no_name,$_pcmt_messages;
	
	$page = $post['page'];
	if (!is_pagename($page))
	{
		return array('msg'=>'invalid page name.','body'=>'cannot add comment.','collided'=>TRUE);
	}
	
	check_editable($page, true, true);
	
	$ret = array(
		'msg' => $_title_updated,
		'collided' => FALSE
	);
	
	//�����ȥե����ޥåȤ�Ŭ��
	$msg = str_replace('$msg',rtrim($post['msg']),PCMT_MSG_FORMAT);
	$name = $post['name'] == '' ? $_no_name : $post['name'];
	$name = ($name == '') ? '' : str_replace('$name',$name,PCMT_NAME_FORMAT);
	$date = ($post['nodate'] == '1') ? '' : str_replace('$now',$now,PCMT_NOW_FORMAT);
	if ($date != '' or $name != '')
	{
		$msg = str_replace("\x08MSG\x08", $msg,  PCMT_FORMAT);
		$msg = str_replace("\x08NAME\x08",$name, $msg);
		$msg = str_replace("\x08DATE\x08",$date, $msg);
	}
	$reply_hash = array_key_exists('reply',$post) ? $post['reply'] : '';
	if ($reply_hash or !is_page($page))
	{
		$msg = preg_replace('/^\-+/','',$msg);
	}
	$msg = rtrim($msg);
	
	if (!is_page($page))
	{
		$postdata = '[['.htmlspecialchars(strip_bracket($post['refer']))."]]\n\n-$msg\n";
	}
	else
	{
		//�ڡ������ɤ߽Ф�
		$postdata = get_source($page);
		
		// �����ξ��ͤ򸡽�
		if (md5(join('',$postdata)) != $post['digest'])
		{
			$ret['msg'] = $_pcmt_messages['title_collided'];
			$ret['body'] = $_pcmt_messages['msg_collided'];
		}
		
		// �����
		$level = 1;
		$pos = 0;
		
		// �����Ȥγ��ϰ��֤򸡺�
		while ($pos < count($postdata))
		{
			if (preg_match('/^\-/',$postdata[$pos]))
			{
				break;
			}
			$pos++;
		}
		$start_pos = $pos;
		//��ץ饤��Υ����Ȥ򸡺�
		if ($reply_hash != '')
		{
			while ($pos < count($postdata))
			{
				if (preg_match('/^(\-{1,2})(?!\-)(.*)$/',$postdata[$pos++],$matches)
					and md5($matches[2]) == $reply_hash)
				{
					$level = strlen($matches[1]) + 1; //���������٥�
					
					// �����Ȥ������򸡺�
					while ($pos < count($postdata))
					{
						if (preg_match('/^(\-{1,3})(?!\-)/',$postdata[$pos],$matches)
							and strlen($matches[1]) < $level)
						{
							break;
						}
						$pos++;
					}
					break;
				}
			}
		}
		else
		{
			$pos = ($post['dir'] == 0) ? $start_pos : count($postdata);
		}
		
		if ($post['dir'] == '0')
		{
			if ($pos == count($postdata))
			{
				$pos = $start_pos; //��Ƭ
			}
		}
		else
		{
			if ($pos == 0)
			{
				$pos = count($postdata); //����
			}
		}
		
		//�����Ȥ�����
		array_splice($postdata,$pos,0,str_repeat('-',$level)."$msg\n");
		
		// ��������
		pcmt_auto_log($page,$post['dir'],$post['count'],$postdata);
		
		$postdata = join('',$postdata);
	}
	page_write($page,$postdata,PCMT_TIMESTAMP);
	if (PCMT_TIMESTAMP)
	{
		// �ƥڡ����Υ����ॹ����פ򹹿�����
		touch(get_filename($post['refer']));
		put_lastmodified();
	}
	return $ret;
}
// ��������
function pcmt_auto_log($page,$dir,$count,&$postdata)
{
	if (!PCMT_AUTO_LOG)
	{
		return;
	}
	$keys = array_keys(preg_grep('/(?:^-(?!-).*$)/m',$postdata));
	if (count($keys) < (PCMT_AUTO_LOG + $count))
	{
		return;
	}
	if ($dir) //������PCMT_AUTO_LOG��
	{
		$old = array_splice($postdata,$keys[0],$keys[PCMT_AUTO_LOG] - $keys[0]);
	}
	else //�����PCMT_AUTO_LOG��
	{
		$old = array_splice($postdata,$keys[count($keys) - PCMT_AUTO_LOG]);
	}
	// �ڡ���̾�����
	$i = 0;
	do {
		$i++;
		$_page = "$page/$i";
	} while (is_page($_page));
	
	page_write($_page,"[[$page]]\n\n".join('',$old));
	
	// �����֤� :)
	pcmt_auto_log($page,$dir,$count,$postdata);
}
//���ץ�������Ϥ���
function pcmt_check_arg($val, $key, &$params)
{
	if ($val != '')
	{
		$l_val = strtolower($val);
		foreach (array_keys($params) as $key)
		{
			if (strpos($key,$l_val) === 0)
			{
				$params[$key] = TRUE;
				return;
			}
		}
	}
	$params['_args'][] = $val;
}

function pcmt_get_comments($page,$count,$dir,$reply)
{
	global $_msg_pcomment_restrict;
	
	if (!check_readable($page, false, false))
	{
		return array(str_replace('$1',$page,$_msg_pcomment_restrict));
	}
	
	$data = get_source($page);
	
	if (!is_array($data))
	{
		return array('',0);
	}
	
	$digest = md5(join('',$data));
	
	//�����Ȥ���ꤵ�줿��������ڤ���
	if ($dir)
	{
		$data = array_reverse($data);
	}
	$num = $cnt = 0;
	$cmts = array();
	foreach ($data as $line)
	{
		if ($count > 0 and $dir and $cnt == $count)
		{
			break;
		}
		if (preg_match('/^(\-{1,2})(?!\-)(.+)$/', $line, $matches))
		{
			if ($count > 0 and strlen($matches[1]) == 1 and ++$cnt > $count)
			{
				break;
			}
			if ($reply)
			{
				++$num;
				$cmts[] = "$matches[1]\x01$num\x02".md5($matches[2])."\x03$matches[2]\n";
				continue;
			}
		}
		$cmts[] = $line;
	}
	$data = $cmts;
	if ($dir)
	{
		$data = array_reverse($data);
	}
	unset($cmts);
	
	//�����Ȥ�����Υǡ������������
	while (count($data) > 0 and substr($data[0],0,1) != '-')
	{
		array_shift($data);
	}
	
	//html�Ѵ�
	$comments = convert_html($data);
	unset($data);
	
	//�����Ȥ˥饸���ܥ���ΰ���Ĥ���
	if ($reply)
	{
		$comments = preg_replace("/<li>\x01(\d+)\x02(.*)\x03/",'<li class="pcmt"><input class="pcmt" type="radio" name="reply" value="$2" tabindex="$1" />', $comments);
	}
	return array($comments,$digest);
}
?>
