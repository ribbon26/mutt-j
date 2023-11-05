<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: config.php,v 1.1 2004/08/01 01:54:35 henoheno Exp $
//
/*
 * �ץ饰����������PukiWiki�Υڡ����˵��Ҥ���
 *
 * // ���֥�����������
 * $obj = new Config('plugin/�ץ饰����̾/')
 * // �ɤ߽Ф�
 * $obj->read();
 * // �������
 * $array = &$obj->get($title);
 * // �ɲ� - ľ��
 * $array[] = array(4,5,6);
 * // �ɲ� - Config���֥������ȤΥ᥽�å�
 * $obj->add($title,array(4,5,6));
 * // �ִ� - ľ��
 * $array = array(1=>array(1,2,3));
 * // �ִ� - Config���֥������ȤΥ᥽�å�
 * $obj->put($title,array(1=>array(1,2,3));
 * // �õ�
 * $obj->put_values($title,NULL);
 * // �񤭹���
 * $obj->write();
 *
 */

// �ڡ���̾�Υץ�ե�����
define('CONFIG_BASE',':config/');

// ����ڡ�������
class Config
{
	// �ڡ���̾
	var $name,$page;
	// ����
	var $objs = array();

	function Config($name)
	{
		$this->name = $name;
		$this->page = CONFIG_BASE.$name;
	}
	// �ڡ������ɤ߹���
	function read()
	{
		if (!is_page($this->page))
		{
			return FALSE;
		}
		$this->objs = array();
		$obj = &new ConfigTable('');
		foreach (get_source($this->page) as $line)
		{
			if ($line == '')
			{
				continue;
			}

			$head = $line{0};
			$level = strspn($line,$head);

			if ($level > 3)
			{
				$obj->add_line($line);
				continue;
			}

			if ($head == '*')
			{
				// ���Ф��θ�ͭID������
				$line = preg_replace('/^(\*{1,3}.*)\[#[A-Za-z][\w-]+\](.*)$/','$1$2',$line);

				if ($level == 1)
				{
					$this->objs[$obj->title] = $obj;
					$obj = &new ConfigTable($line);
				}
				else
				{
					if (!is_a($obj,'ConfigTable_Direct'))
					{
						$obj = &new ConfigTable_Direct('',$obj);
					}
					$obj->set_key($line);
				}
			}
			else if ($head == '-' and $level > 1)
			{
				if (!is_a($obj,'ConfigTable_Direct'))
				{
					$obj = &new ConfigTable_Direct('',$obj);
				}
				$obj->add_value($line);
			}
			else if ($head == '|' and preg_match('/^\|(.+)\|\s*$/',$line,$matches))
			{
				if (!is_a($obj,'ConfigTable_Sequential'))
				{
					$obj = &new ConfigTable_Sequential('',$obj);
				}
				$obj->add_value(explode('|',$matches[1]));
			}
			else
			{
				$obj->add_line($line);
			}
		}
		$this->objs[$obj->title] = $obj;

		return TRUE;
	}
	// ������������
	function &get($title)
	{
		$obj = &$this->get_object($title);
		return $obj->values;
	}
	// ��������ꤹ��(���)
	function put($title,$values)
	{
		$obj = &$this->get_object($title);
		$obj->values = $values;
	}
	// �Ԥ��ɲä���
	function add($title,$value)
	{
		$obj = &$this->get_object($title);
		$obj->values[] = $value;
	}
	// ���֥������Ȥ��������(�ʤ��Ȥ��Ϻ��)
	function &get_object($title)
	{
		if (!array_key_exists($title,$this->objs))
		{
			$this->objs[$title] = &new ConfigTable('*'.trim($title)."\n");
		}
		return $this->objs[$title];
	}
	// �ڡ����˽񤭹���
	function write()
	{
		page_write($this->page, $this->toString());
	}
	// �񼰲�
	function toString()
	{
		$retval = '';
		foreach ($this->objs as $title=>$obj)
		{
			$retval .= $obj->toString();
		}
		return $retval;
	}
}
//�����ͤ��ݻ����륯�饹
class ConfigTable
{
	// �ơ��֥��̾��
	var $title = '';
	// �ڡ���������(�ơ��֥�ʳ�����ʬ)
	var $before = array();
	// ���������ͤ�����
	var $values = array();
	// �ڡ���������(�ơ��֥�ʳ�����ʬ)
	var $after = array();

	function ConfigTable($title,$obj=NULL)
	{
		if ($obj !== NULL)
		{
			$this->title = $obj->title;
			$this->before = array_merge($obj->before,$obj->after);
		}
		else
		{
			$this->title = trim(substr($title,strspn($title,'*')));
			$this->before[] = $title;
		}
	}
	// �������ɲ�
	function add_line($line)
	{
		$this->after[] = $line;
	}
	function toString()
	{
		return join('',$this->before).join('',$this->after);
	}
}
class ConfigTable_Sequential extends ConfigTable
{
	// �Ԥ��ɲ�
	function add_value($value)
	{
		$this->values[] = (count($value) == 1) ? $value[0] : $value;
	}
	// �񼰲�
	function toString()
	{
		$retval = join('',$this->before);
		if (is_array($this->values))
		{
			foreach ($this->values as $value)
			{
				$value = is_array($value) ? join('|',$value) : $value;
				$retval .= "|$value|\n";
			}
		}
		$retval .= join('',$this->after);
		return $retval;
	}
}
class ConfigTable_Direct extends ConfigTable
{
	// �����������������󡣽�������˻��Ѥ��롣
	var $_keys = array();

	// ����������
	function set_key($line)
	{
		$level = strspn($line,'*');
		$this->_keys[$level] = trim(substr($line,$level));
	}
	// �Ԥ��ɲ�
	function add_value($line)
	{
		$level = strspn($line,'-');
		$arr = &$this->values;
		for ($n = 2; $n <= $level; $n++)
		{
			$arr = &$arr[$this->_keys[$n]];
		}
		$arr[] = trim(substr($line,$level));
	}
	// �񼰲�
	function toString($values = NULL,$level = 2)
	{
		$retval = '';
		if ($root = ($values == NULL))
		{
			$retval = join('',$this->before);
			$values = &$this->values;
		}
		foreach ($values as $key=>$value)
		{
			if (is_array($value))
			{
				$retval .= str_repeat('*',$level).$key."\n";
				$retval .= $this->toString($value,$level + 1);
			}
			else
			{
				$retval .= str_repeat('-',$level - 1).$value."\n";
			}
		}
		if ($root)
		{
			$retval .= join('',$this->after);
		}
		return $retval;
	}
}
?>
