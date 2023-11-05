<?php
/////////////////////////////////////////////////
// PukiWiki - Yet another WikiWikiWeb clone.
//
// $Id: convert_html.php,v 1.1 2004/08/01 01:54:35 henoheno Exp $
//

function convert_html($lines)
{
	global $vars, $digest;
	static $contents_id = 0;

	if (!is_array($lines))
	{
		$lines = explode("\n", $lines);
	}

	$digest = md5(join('', get_source($vars['page'])));

	$body = &new Body(++$contents_id);
	$body->parse($lines);
	$ret = $body->toString();

	return $ret;
}

class Element
{ // �֥�å�����
	var $parent;   // ������
	var $last;     // �������Ǥ�����������
	var $elements; // ���Ǥ�����

	function Element()
	{
		$this->elements = array();
		$this->last = &$this;
	}

	function setParent(&$parent)
	{
		$this->parent = &$parent;
	}

	function &add(&$obj)
	{
		if ($this->canContain($obj))
		{
			return $this->insert($obj);
		}

		return $this->parent->add($obj);
	}

	function &insert(&$obj)
	{
		$obj->setParent($this);
		$this->elements[] = &$obj;

		return $this->last = &$obj->last;
	}
	function canContain($obj)
	{
		return TRUE;
	}

	function wrap($string, $tag, $param = '', $canomit = TRUE)
	{
		return ($canomit and $string == '') ? '' : "<$tag$param>$string</$tag>";
	}

	function toString()
	{
		$ret = array();
		foreach (array_keys($this->elements) as $key)
		{
			$ret[] = $this->elements[$key]->toString();
		}

		return join("\n",$ret);
	}

	function dump($indent = 0)
	{
		$ret = str_repeat(' ', $indent).get_class($this)."\n";

		$indent += 2;

		foreach (array_keys($this->elements) as $key)
		{
			$ret .= is_object($this->elements[$key]) ?
				$this->elements[$key]->dump($indent) : '';
				//str_repeat(' ',$indent).$this->elements[$key];
		}

		return $ret;
	}
}

class Inline extends Element
{ // ����饤������

	function Inline($text)
	{
		parent::Element();

		if (substr($text,0,1) == '~') // ��Ƭ~���ѥ饰��ճ���
		{
			$this = new Paragraph(' '.substr($text,1));
			$this->last = &$this;

			return;
		}
		$this->elements[] = trim((substr($text, 0, 1) == "\n") ? $text : make_link($text));
	}

	function &insert(&$obj)
	{
		$this->elements[] = $obj->elements[0];

		return $this;
	}

	function canContain($obj)
	{
		return is_a($obj,'Inline');
	}

	function toString()
	{
		global $line_break;

		return join($line_break ? "<br />\n" : "\n",$this->elements);
	}

	function &toPara($class = '')
	{
		$obj = &new Paragraph('', $class);
		$obj->insert($this);
		return $obj;
	}
}

class Paragraph extends Element
{ // ����
	var $param;

	function Paragraph($text, $param = '')
	{
		parent::Element();

		$this->param = $param;
		if ($text == '')
		{
			return;
		}
		if (substr($text,0,1) == '~')
		{
			$text = ' '.substr($text, 1);
		}
		$this->insert(new Inline($text));
	}

	function canContain($obj)
	{
		return is_a($obj,'Inline');
	}

	function toString()
	{
		return $this->wrap(parent::toString(), 'p', $this->param);
	}
}

class Heading extends Element
{ // *
	var $level;
	var $id;
	var $msg_top;

	function Heading(&$root, $text)
	{
		parent::Element();

		$this->level = min(3, strspn($text, '*'));
		list($text, $this->msg_top, $this->id) = $root->getAnchor($text, $this->level);
		$this->insert(new Inline($text));
		$this->level++; // h2,h3,h4
	}

	function &insert(&$obj)
	{
		parent::insert($obj);
		return $this->last = &$this;
	}

	function canContain(&$obj)
	{
		return FALSE;
	}

	function toString()
	{
		return $this->msg_top.$this->wrap(parent::toString(), 'h'.$this->level, " id=\"{$this->id}\"");
	}
}

class HRule extends Element
{ // ----

	function HRule(&$root, $text)
	{
		parent::Element();
	}

	function canContain(&$obj)
	{
		return FALSE;
	}

	function toString()
	{
		global $hr;

		return $hr;
	}
}

class ListContainer extends Element
{
	var $tag;
	var $tag2;
	var $level;
	var $style;
	var $margin;
	var $left_margin;

	function ListContainer($tag, $tag2, $head, $text)
	{
		parent::Element();

		//�ޡ���������
		$var_margin = "_{$tag}_margin";
		$var_left_margin = "_{$tag}_left_margin";
		global $$var_margin, $$var_left_margin;
		$this->margin = $$var_margin;
		$this->left_margin = $$var_left_margin;

		//�����
		$this->tag = $tag;
		$this->tag2 = $tag2;
		$this->level = min(3, strspn($text, $head));
		$text = ltrim(substr($text, $this->level));

		parent::insert(new ListElement($this->level, $tag2));
		if ($text != '')
		{
			$this->last = &$this->last->insert(new Inline($text));
		}
	}

	function canContain(&$obj)
	{
		return (!is_a($obj, 'ListContainer')
			or ($this->tag == $obj->tag and $this->level == $obj->level));
	}

	function setParent(&$parent)
	{
		global $_list_pad_str;

		parent::setParent($parent);

		$step = $this->level;
		if (isset($parent->parent) and is_a($parent->parent, 'ListContainer'))
		{
			$step -= $parent->parent->level;
		}
		$margin = $this->margin * $step;
		if ($step == $this->level)
		{
			$margin += $this->left_margin;
		}
		$this->style = sprintf($_list_pad_str, $this->level, $margin, $margin);
	}

	function &insert(&$obj)
	{
		if (!is_a($obj, get_class($this)))
		{
			return $this->last = &$this->last->insert($obj);
		}
        // ��Ƭʸ���Τߤλ������UL/OL�֥�å���æ��
        // BugTrack/524
		if (count($obj->elements) == 1 && count($obj->elements[0]->elements) == 0)
		{
			return $this->last->parent; // up to ListElement.
		}
		// move elements.
		foreach(array_keys($obj->elements) as $key)
		{
			parent::insert($obj->elements[$key]);
		}

		return $this->last;
	}

	function toString()
	{
		return $this->wrap(parent::toString(), $this->tag, $this->style);
	}
}

class ListElement extends Element
{
	function ListElement($level, $head)
	{
		parent::Element();
		$this->level = $level;
		$this->head = $head;
	}

	function canContain(&$obj)
	{
		return (!is_a($obj, 'ListContainer') or ($obj->level > $this->level));
	}

	function toString()
	{
		return $this->wrap(parent::toString(), $this->head);
	}
}

class UList extends ListContainer
{ // -
	function UList(&$root, $text)
	{
		parent::ListContainer('ul', 'li', '-', $text);
	}
}

class OList extends ListContainer
{ // +
	function OList(&$root, $text)
	{
		parent::ListContainer('ol', 'li', '+', $text);
	}
}

class DList extends ListContainer
{ // :
	function DList(&$root, $text)
	{
		$out = explode('|', $text, 2);
		if (count($out) < 2)
		{
			$this = new Inline($text);
			$this->last = &$this;

			return;
		}
		parent::ListContainer('dl', 'dt', ':', $out[0]);

		$this->last = &Element::insert(new ListElement($this->level, 'dd'));
		if ($out[1] != '')
		{
			$this->last = &$this->last->insert(new Inline($out[1]));
		}
	}
}

class BQuote extends Element
{ // >
	var $level;

	function BQuote(&$root, $text)
	{
		parent::Element();

		$head = substr($text, 0, 1);
		$this->level = min(3, strspn($text, $head));
		$text = ltrim(substr($text, $this->level));

		if ($head == '<') //blockquote close
		{
			$level = $this->level;
			$this->level = 0;
			$this->last = &$this->end($root, $level);
			if ($text != '')
			{
				$this->last = &$this->last->insert(new Inline($text));
			}
		}
		else
		{
			$this->insert(new Inline($text));
		}
	}

	function canContain(&$obj)
	{
		return (!is_a($obj, get_class($this)) or $obj->level >= $this->level);
	}

	function &insert(&$obj)
	{
        // BugTrack/521, BugTrack/545
		if (is_a($obj, 'inline')) {
        	return parent::insert($obj->toPara(' class="quotation"'));
        }
		if (is_a($obj, 'BQuote') and $obj->level == $this->level and count($obj->elements))
		{
			$obj = &$obj->elements[0];
			if (is_a($this->last,'Paragraph') and count($obj->elements))
			{
				$obj = &$obj->elements[0];
			}
		}
		return parent::insert($obj);
	}

	function toString()
	{
		return $this->wrap(parent::toString(), 'blockquote');
	}

	function &end(&$root, $level)
	{
		$parent = &$root->last;

		while (is_object($parent))
		{
			if (is_a($parent,'BQuote') and $parent->level == $level)
			{
				return $parent->parent;
			}
			$parent = &$parent->parent;
		}
		return $this;
	}
}

class TableCell extends Element
{
	var $tag = 'td'; // {td|th}
	var $colspan = 1;
	var $rowspan = 1;
	var $style; // is array('width'=>, 'align'=>...);

	function TableCell($text, $is_template = FALSE)
	{
		parent::Element();
		$this->style = $matches = array();

		while (preg_match('/^(?:(LEFT|CENTER|RIGHT)|(BG)?COLOR\(([#\w]+)\)|SIZE\((\d+)\)):(.*)$/',$text,$matches))
		{
			if ($matches[1])
			{
				$this->style['align'] = 'text-align:'.strtolower($matches[1]).';';
				$text = $matches[5];
			}
			else if ($matches[3])
			{
				$name = $matches[2] ? 'background-color' : 'color';
				$this->style[$name] = $name.':'.htmlspecialchars($matches[3]).';';
				$text = $matches[5];
			}
			else if ($matches[4])
			{
				$this->style['size'] = 'font-size:'.htmlspecialchars($matches[4]).'px;';
				$text = $matches[5];
			}
		}
		if ($is_template and is_numeric($text))
		{
			$this->style['width'] = "width:{$text}px;";
		}
		if ($text == '>')
		{
			$this->colspan = 0;
		}
		else if ($text == '~')
		{
			$this->rowspan = 0;
		}
		else if (substr($text, 0, 1) == '~')
		{
			$this->tag = 'th';
			$text = substr($text, 1);
		}
		if ($text != '' and $text{0} == '#')
		{
			// �������Ƥ�'#'�ǻϤޤ�Ȥ���Div���饹���̤��Ƥߤ�
			$obj = &new Div($this, $text);
			if (is_a($obj, 'Paragraph'))
			{
				$obj = &$obj->elements[0];
			}
		}
		else
		{
			$obj = &new Inline($text);
		}
		$this->insert($obj);
	}

	function setStyle(&$style)
	{
		foreach ($style as $key=>$value)
		{
			if (!array_key_exists($key, $this->style))
			{
				$this->style[$key] = $value;
			}
		}
	}

	function toString()
	{
		if ($this->rowspan == 0 or $this->colspan == 0)
		{
			return '';
		}
		$param = " class=\"style_{$this->tag}\"";
		if ($this->rowspan > 1)
		{
			$param .= " rowspan=\"{$this->rowspan}\"";
		}
		if ($this->colspan > 1)
		{
			$param .= " colspan=\"{$this->colspan}\"";
			unset($this->style['width']);
		}
		if (count($this->style))
		{
			$param .= ' style="'.join(' ', $this->style).'"';
		}

		return $this->wrap(parent::toString(), $this->tag, $param, FALSE);
	}
}

class Table extends Element
{ // |
	var $type;
	var $types;
	var $col; // number of column

	function Table(&$root, $text)
	{
		parent::Element();

		$out = array();
		if (!preg_match("/^\|(.+)\|([hHfFcC]?)$/", $text, $out))
		{
			$this = new Inline($text);
			$this->last = &$this;

			return;
		}
		$cells = explode('|', $out[1]);
		$this->col = count($cells);
		$this->type = strtolower($out[2]);
		$this->types = array($this->type);
		$is_template = ($this->type == 'c');
		$row = array();
		foreach ($cells as $cell)
		{
			$row[] = &new TableCell($cell, $is_template);
		}
		$this->elements[] = $row;
	}

	function canContain(&$obj)
	{
		return is_a($obj, 'Table') and ($obj->col == $this->col);
	}

	function &insert(&$obj)
	{
		$this->elements[] = $obj->elements[0];
		$this->types[] = $obj->type;

		return $this;
	}

	function toString()
	{
		static $parts = array('h'=>'thead', 'f'=>'tfoot', ''=>'tbody');

		// rowspan������(��������)
		for ($ncol = 0; $ncol < $this->col; $ncol++)
		{
			$rowspan = 1;
			foreach (array_reverse(array_keys($this->elements)) as $nrow)
			{
				$row = &$this->elements[$nrow];
				if ($row[$ncol]->rowspan == 0)
				{
					$rowspan++;
					continue;
				}
				$row[$ncol]->rowspan = $rowspan;
				while (--$rowspan) // �Լ��̤�Ѿ�����
				{
					$this->types[$nrow + $rowspan] = $this->types[$nrow];
				}
				$rowspan = 1;
			}
		}
		// colspan,style������
		$stylerow = NULL;
		foreach (array_keys($this->elements) as $nrow)
		{
			$row = &$this->elements[$nrow];
			if ($this->types[$nrow] == 'c')
			{
				$stylerow = &$row;
			}
			$colspan = 1;
			foreach (array_keys($row) as $ncol)
			{
				if ($row[$ncol]->colspan == 0)
				{
					$colspan++;
					continue;
				}
				$row[$ncol]->colspan = $colspan;
				if ($stylerow !== NULL)
				{
					$row[$ncol]->setStyle($stylerow[$ncol]->style);
					while (--$colspan) // �󥹥������Ѿ�����
					{
						$row[$ncol - $colspan]->setStyle($stylerow[$ncol]->style);
					}
				}
				$colspan = 1;
			}
		}
		// �ƥ����Ȳ�
		$string = '';
		foreach ($parts as $type => $part)
		{
			$part_string = '';
			foreach (array_keys($this->elements) as $nrow)
			{
				if ($this->types[$nrow] != $type)
				{
					continue;
				}
				$row = &$this->elements[$nrow];
				$row_string = '';
				foreach (array_keys($row) as $ncol)
				{
					$row_string .= $row[$ncol]->toString();
				}
				$part_string .= $this->wrap($row_string, 'tr');
			}
			$string .= $this->wrap($part_string, $part);
		}
		$string = $this->wrap($string, 'table', ' class="style_table" cellspacing="1" border="0"');
		return $this->wrap($string, 'div', ' class="ie5"');
	}
}

class YTable extends Element
{ // ,
	var $col;

	function YTable(&$root, $text)
	{
		parent::Element();

		$_value = csv_explode(',', substr($text,1));
		if (count($_value) == 0)
		{
			$this = new Inline($text);
			$this->last = &$this;

			return;
		}
		$align = $value = $matches = array();
		foreach($_value as $val)
		{
			if (preg_match('/^(\s+)?(.+?)(\s+)?$/', $val, $matches))
			{
				$align[] =($matches[1] != '') ?
					((array_key_exists(3,$matches) and $matches[3] != '') ?
						' style="text-align:center"' : ' style="text-align:right"'
					) : '';
				$value[] = $matches[2];
			}
			else
			{
				$align[] = '';
				$value[] = $val;
			}
		}
		$this->col = count($value);
		$colspan = array();
		foreach ($value as $val)
		{
			$colspan[] = ($val == '==') ? 0 : 1;
		}
		$str = '';
		for ($i = 0; $i < count($value); $i++)
		{
			if ($colspan[$i])
			{
				while ($i + $colspan[$i] < count($value) and $value[$i + $colspan[$i]] == '==')
				{
					$colspan[$i]++;
				}
				$colspan[$i] = ($colspan[$i] > 1) ? " colspan=\"{$colspan[$i]}\"" : '';
				$str .= "<td class=\"style_td\"{$align[$i]}{$colspan[$i]}>".make_link($value[$i]).'</td>';
			}
		}
		$this->elements[] = $str;
	}

	function canContain(&$obj)
	{
		return is_a($obj, 'YTable') and ($obj->col == $this->col);
	}

	function &insert(&$obj)
	{
		$this->elements[] = $obj->elements[0];

		return $this;
	}

	function toString()
	{
		$rows = '';
		foreach ($this->elements as $str)
		{
			$rows .= "\n<tr class=\"style_tr\">$str</tr>\n";
		}
		$rows = $this->wrap($rows, 'table', ' class="style_table" cellspacing="1" border="0"');
		return $this->wrap($rows, 'div', ' class="ie5"');
	}
}

class Pre extends Element
{ // ' '
	function Pre(&$root,$text)
	{
		global $preformat_ltrim;

		parent::Element();
		$this->elements[] = htmlspecialchars(
			(!$preformat_ltrim or $text == '' or $text{0} != ' ') ? $text : substr($text, 1)
		);
	}

	function canContain(&$obj)
	{
		return is_a($obj, 'Pre');
	}

	function &insert(&$obj)
	{
		$this->elements[] = $obj->elements[0];

		return $this;
	}

	function toString()
	{
		return $this->wrap(join("\n", $this->elements), 'pre');
	}
}

class Div extends Element
{ // #
	var $name;
	var $param;

	function Div(&$root, $text)
	{
		parent::Element();

		if (!preg_match("/^\#([^\(]+)(?:\((.*)\))?/", $text, $out) or !exist_plugin_convert($out[1]))
		{
			$this = new Paragraph($text);
			$this->last = &$this;

			return;
		}
		list(, $this->name, $this->param) = array_pad($out,3,'');
	}

	function canContain(&$obj)
	{
		return FALSE;
	}

	function toString()
	{
		return do_plugin_convert($this->name,$this->param);
	}
}

class Align extends Element
{ // LEFT:/CENTER:/RIGHT:
	var $align;

	function Align($align)
	{
		parent::Element();

		$this->align = $align;
	}

	function canContain(&$obj)
	{
		return is_a($obj, 'Inline');
	}

	function toString()
	{
		return $this->wrap(parent::toString(), 'div', ' style="text-align:'.$this->align.'"');
	}
}

class Body extends Element
{ // Body
	var $id;
	var $count = 0;
	var $contents;
	var $contents_last;
	var $classes = array(
		'-' => 'UList',
		'+' => 'OList',
		':' => 'DList',
		'|' => 'Table',
		',' => 'YTable',
		'>' => 'BQuote',
		'<' => 'BQuote',
		'#' => 'Div'
	);

	function Body($id)
	{
		$this->id = $id;
		$this->contents = &new Element();
		$this->contents_last = &$this->contents;
		parent::Element();
	}

	function parse(&$lines)
	{
		$this->last = &$this;

		while (count($lines))
		{
			$line = array_shift($lines);

			if (substr($line,0,2) == '//') //�����ȤϽ������ʤ�
			{
				continue;
			}

			if (preg_match('/^(LEFT|CENTER|RIGHT):(.*)$/',$line,$matches))
			{
				$this->last = &$this->last->add(new Align(strtolower($matches[1]))); // <div style="text-align:...">
				if ($matches[2] == '')
				{
					continue;
				}
				$line = $matches[2];
			}

			$line = preg_replace("/[\r\n]*$/",'',$line);

			// Empty
			if ($line == '')
			{
				$this->last = &$this;
				continue;
			}
			// Horizontal Rule
			if (substr($line,0,4) == '----')
			{
				$this->insert(new HRule($this,$line));
				continue;
			}
			// ��Ƭʸ��
			$head = $line{0};

			// Heading
			if ($head == '*')
			{
				$this->insert(new Heading($this,$line));
				continue;
			}
			// Pre
			if ($head == ' ' or $head == "\t")
			{
				$this->last = &$this->last->add(new Pre($this,$line));
				continue;
			}
			// Line Break
			if (substr($line,-1) == '~')
			{
				$line = substr($line,0,-1)."\r";
			}
			// Other Character
			if (array_key_exists($head, $this->classes))
			{
				$classname = $this->classes[$head];
				$this->last = &$this->last->add(new $classname($this,$line));
				continue;
			}

			// Default
			$this->last = &$this->last->add(new Inline($line));
		}
	}

	function getAnchor($text,$level)
	{
		global $top,$_symbol_anchor;

		$anchor = (($id = make_heading($text,FALSE)) == '') ?
			'' : " &aname($id,super,full)\{$_symbol_anchor};";
		$text = ' '.$text;
		$id = "content_{$this->id}_{$this->count}";
		$this->count++;
		$this->contents_last = &$this->contents_last->add(new Contents_UList($text,$level,$id));

		return array($text. $anchor, $this->count > 1 ? "\n$top" : '', $id);
	}

	function &insert(&$obj)
	{
		if (is_a($obj, 'Inline'))
		{
			$obj = &$obj->toPara();
		}
		return parent::insert($obj);
	}

	function toString()
	{
		global $vars;

		$text = parent::toString();

		// #contents
		$text = preg_replace_callback('/(<p[^>]*>)<del>#contents<\/del>(\s*)(<\/p>)/', array(&$this,'replace_contents'),$text);

		// ��Ϣ����ڡ���
		// <p>�ΤȤ��Ϲ�Ƭ���顢<del>�ΤȤ���¾�����Ǥλ����ǤȤ���¸��
		$text = preg_replace_callback('/(<p[^>]*>)<del>#related<\/del>(\s*)(<\/p>)/', array(&$this, 'replace_related'), $text);
		$text = preg_replace('/<del>#related<\/del>/',make_related($vars['page'],'del'),$text);
		return "$text\n";
	}

	function replace_contents($arr)
	{
		$contents  = "<div class=\"contents\">\n";
		$contents .= "<a id=\"contents_{$this->id}\"></a>";
		$contents .= $this->contents->toString();
		$contents .= "</div>\n";
		array_shift($arr);

		return ($arr[1] != '') ? $contents.join('',$arr) : $contents;
	}

	function replace_related($arr)
	{
		global $vars;
		static $related = NULL;

		if (is_null($related))
		{
			$related = make_related($vars['page'],'p');
		}
		array_shift($arr);

		return ($arr[1] != '') ? $related.join('',$arr) : $related;
	}
}

class Contents_UList extends ListContainer
{
	function Contents_UList($text,$level,$id)
	{
		// �ƥ����ȤΥ�ե�����
		// ��Ƭ\n�������Ѥߤ�ɽ�� ... X(
		make_heading($text);
		$text = "\n<a href=\"#$id\">$text</a>\n";
		parent::ListContainer('ul', 'li', '-', str_repeat('-',$level));
		$this->insert(new Inline($text));
	}

	function setParent(&$parent)
	{
		global $_list_pad_str;

		parent::setParent($parent);
		$step = $this->level;
		$margin = $this->left_margin;
		if (isset($parent->parent) and is_a($parent->parent,'ListContainer'))
		{
			$step -= $parent->parent->level;
			$margin = 0;
		}
		$margin += $this->margin * ($step == $this->level ? 1 : $step);
		$this->style = sprintf($_list_pad_str,$this->level,$margin,$margin);
	}
}
?>
