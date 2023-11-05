<?php
/**
 *
 * PukiWiki - Yet another WikiWikiWeb clone.
 *
 * backup.php
 *
 * �Хå����åפ��������
 *
 * @package org.pukiwiki
 * @access  public
 * @author
 * @create
 * @version $Id: backup.php,v 1.2 2004/08/01 13:39:35 henoheno Exp $
 **/

/**
 * make_backup
 * �Хå����åפ��������
 *
 * @access    public
 * @param     String    $page        �ڡ���̾
 * @param     Boolean   $delete      TRUE:�Хå����åפ�������
 *
 * @return    Void
 */
function make_backup($page,$delete = FALSE)
{
	global $splitter,$cycle,$maxage;
	global $do_backup,$del_backup;

	if (!$do_backup)
	{
		return;
	}

	if ($del_backup and $delete)
	{
		backup_delete($page);
		return;
	}

	if (!is_page($page))
	{
		return;
	}

	$lastmod = backup_get_filetime($page);
	if (($lastmod == 0) or (UTIME - $lastmod) > (60 * 60 * $cycle))
	{
		$backups = get_backup($page);
		$count = count($backups) + 1;
		if ($count > $maxage)
		{
			//ľ���1���ɲä���Τǡ�(������-1)��Ķ�������Ǥ�ΤƤ�
			array_splice($backups,0,$count - $maxage);
		}

		$strout = '';
		foreach($backups as $age=>$data)
		{
			$strout .= "$splitter {$data['time']}\n";
			$strout .= join('',$data['data']);
		}
		$strout = preg_replace("/([^\n])\n*$/","$1\n",$strout);

		// ��ʸ�˴ޤޤ��$splitter�򥨥������פ���(Ⱦ�ѥ��ڡ��������ղ�)
		$body = preg_replace('/^('.preg_quote($splitter)."\s\d+)$/",'$1 ',get_source($page));
		$body = "$splitter ".get_filetime($page)."\n".join('',$body);
		$body = preg_replace("/\n*$/","\n",$body);

		$fp = backup_fopen($page,'wb')
			or die_message('cannot write file '.htmlspecialchars($realfilename).'<br />maybe permission is not writable or filename is too long');
		backup_fputs($fp,$strout);
		backup_fputs($fp,$body);
		backup_fclose($fp);
	}
}

/**
 * get_backup
 * �Хå����åפ��������
 * $age=0�ޤ��Ͼ�ά : ���ƤΥХå����åץǡ���������Ǽ�������
 * $age>0          : ���ꤷ������ΥХå����åץǡ������������
 *
 * @access    public
 * @param     String    $page        �ڡ���̾
 * @param     Integer   $age         �Хå����åפ������ֹ� ��ά��������
 *
 * @return    String    �Хå����å�($age!=0)
 *            Array     �Хå����åפ�����($age==0)
 */
function get_backup($page,$age = 0)
{
	global $splitter;

	$lines = backup_file($page);

	if (!is_array($lines))
	{
		return array();
	}

	$_age = 0;
	$retvars = $match = array();
	foreach($lines as $line)
	{
		if (preg_match("/^$splitter\s(\d+)$/", $line, $match)) {
			++$_age;
			if ($age > 0 and $_age > $age) {
				return $retvars[$age];
			}
			$retvars[$_age] = array('time'=>$match[1], 'data'=>array());
		} else {
			$retvars[$_age]['data'][] = $line;
		}
	}

	return $retvars;
}

/**
 * backup_get_filename
 * �Хå����åץե�����̾���������
 *
 * @access    private
 * @param     String    $page        �ڡ���̾
 *
 * @return    String    �Хå����åפΥե�����̾
 */
function backup_get_filename($page)
{
	return BACKUP_DIR.encode($page).BACKUP_EXT;
}

/**
 * backup_file_exists
 * �Хå����åץե����뤬¸�ߤ��뤫
 *
 * @access    private
 * @param     String    $page        �ڡ���̾
 *
 * @return    Boolean   TRUE:���� FALSE:�ʤ�
 */
function backup_file_exists($page)
{
	return file_exists(backup_get_filename($page));
}

/**
 * backup_get_filetime
 * �Хå����åץե�����ι������������
 *
 * @access    private
 * @param     String    $page        �ڡ���̾
 *
 * @return    Integer   �ե�����ι�������(GMT)
 */

function backup_get_filetime($page)
{
	return backup_file_exists($page) ?
		filemtime(backup_get_filename($page)) - LOCALZONE :
		0;
}

/**
 * backup_delete
 * �Хå����åץե������������
 *
 * @access    private
 * @param     String    $page        �ڡ���̾
 *
 * @return    Boolean   FALSE:����
 */
function backup_delete($page)
{
	return unlink(backup_get_filename($page));
}

/////////////////////////////////////////////////

if (function_exists('gzfile'))
{
	// �ե����륷���ƥ�ؿ�
	// zlib�ؿ������
	define('BACKUP_EXT','.gz');

/**
 * backup_fopen
 * �Хå����åץե�����򳫤�
 *
 * @access    private
 * @param     String    $page        �ڡ���̾
 * @param     String    $mode        �⡼��
 *
 * @return    Boolean   FALSE:����
 */
	function backup_fopen($page,$mode)
	{
		return gzopen(backup_get_filename($page),$mode);
	}

/**
 * backup_fputs
 * �Хå����åץե�����˽񤭹���
 *
 * @access    private
 * @param     Integer   $zp          �ե�����ݥ���
 * @param     String    $str         ʸ����
 *
 * @return    Boolean   FALSE:���� ����¾:�񤭹�����Х��ȿ�
 */
	function backup_fputs($zp,$str)
	{
		return gzputs($zp,$str);
	}

/**
 * backup_fclose
 * �Хå����åץե�������Ĥ���
 *
 * @access    private
 * @param     Integer   $zp          �ե�����ݥ���
 *
 * @return    Boolean   FALSE:����
 */
	function backup_fclose($zp)
	{
		return gzclose($zp);
	}

/**
 * backup_file
 * �Хå����åץե���������Ƥ��������
 *
 * @access    private
 * @param     String    $page        �ڡ���̾
 *
 * @return    Array     �ե����������
 */
	function backup_file($page)
	{
		return backup_file_exists($page) ?
			gzfile(backup_get_filename($page)) :
			array();
	}
}
/////////////////////////////////////////////////
else
{
	// �ե����륷���ƥ�ؿ�
	define('BACKUP_EXT','.txt');

/**
 * backup_fopen
 * �Хå����åץե�����򳫤�
 *
 * @access    private
 * @param     String    $page        �ڡ���̾
 * @param     String    $mode        �⡼��
 *
 * @return    Boolean   FALSE:����
 */
	function backup_fopen($page,$mode)
	{
		return fopen(backup_get_filename($page),$mode);
	}

/**
 * backup_fputs
 * �Хå����åץե�����˽񤭹���
 *
 * @access    private
 * @param     Integer   $zp          �ե�����ݥ���
 * @param     String    $str         ʸ����
 *
 * @return    Boolean   FALSE:���� ����¾:�񤭹�����Х��ȿ�
 */
	function backup_fputs($zp,$str)
	{
		return fputs($zp,$str);
	}

/**
 * backup_fclose
 * �Хå����åץե�������Ĥ���
 *
 * @access    private
 * @param     Integer   $zp          �ե�����ݥ���
 *
 * @return    Boolean   FALSE:����
 */
	function backup_fclose($zp)
	{
		return fclose($zp);
	}

/**
 * backup_file
 * �Хå����åץե���������Ƥ��������
 *
 * @access    private
 * @param     String    $page        �ڡ���̾
 *
 * @return    Array     �ե����������
 */
	function backup_file($page)
	{
		return backup_file_exists($page) ?
			file(backup_get_filename($page)) :
			array();
	}
}
?>
