<?php
/*
 * Included with https://github.com/hightman/scws/ which is a program
 * released under BSD license
 */

/* ----------------------------------------------------------------------- *\
   PHP4�����HDB - (HashTreeDB.class.php)
   -----------------------------------------------------------------------
   ����: ������(hightman) (MSN: MingL_Mar@msn.com) (php-QQȺ: 17708754)
   ��վ: http://www.hi-php.com
   ʱ��: 2007/05/01 (update: 2007/05/08)
   �汾: 0.1
   Ŀ��: ȡ�� cdb/gdbm ���ٴ�ȡ�ִʴʵ�, ��󲿷��û�ȱ����Щ���������֪ʶ
   ����:
         ����һ�������� cdb/gdbm �� PHP ���뼶�������, ͨ�� key, value �ķ�
		 ʽ��ȡ����, ʹ�÷ǳ���.

		 �����ڿ��ٸ���Ψһ������������

   Ч��:
         1. Ч�ʸ�(20���¼���ϱ�php�ڽ���cdb��Ҫ��), �����Ż��� 35���¼ʱ
		    ����������Ϊ5, ����Ч�ʸ�,�����ļ�
		 2. �ļ�С(ȱʡ������, ��������Լ 100KB, ֮��ÿ����¼Ϊ key, value��
		    �ܳ���+13bytes
		 3. ��ϵͳ����, �����ϵͳ, ���� little endian �� big endian Ӱ��
		 4. PHP ���뼶, �޸�ά������
		 5. �ṩ�ڽ��������Ż�����, �ṩ��ȡ�ṹͼ���ƽӿ�, �ṩ�����ӿ�
		 6. ���ݿɿ��ٸ���, �� cdb ��ֻ���Ļ�ֻд��

   ȱ��:
         1. ����unique key��˵, һ�����Ӳ������ (���Խ�value��Ϊ��ֵ)
		 2. ������ value ʱ, ����� value �ϳ���ɵļ�¼ֱ������, �����޸�
		    ���ܻᵼ���ļ���һЩ���õ�����, ����������Ե��ñ����ӿ���ȫ��
			���������ݿ�
		 3. ������ php ���뼶������, �����ϱ� gdbm û��ʲô����
		 4. IO����, ���Կ��ǽ������ļ��ŵ� memfs �� (linux/bsd)
		 5. key ��󳤶�Ϊ 240bytes, value ��󳤶�Ϊ 65279 bytes, �����ļ����Ϊ 4G
		 6. ��������������ҳ��ȡ

   �÷�: (��Ҫ�ķ���)

   1. ������������, ���캯��: HashTreeDB([int mask [, int base ]])
	  ��ѡ����(������½�������Ч): mask, base ��Ϊ������, ����
	    mask �� hash ��ģ�Ļ���, ����ѡһ������, ��ԼΪ�ܼ�¼���� 1/10 ����.
		base �� hash ���ݼ���Ļ���, ����ʹ��Ĭ��ֵ. ``h = ((h << 5) + h) ^ c''

      $HDB = new HashTreeDB;

   2. �������ļ�, Bool Open(string fpath [, string mode])
      ��Ҫ���� fpath Ϊ�����ļ���·��, ��ѡ���� mode ��ֵΪ r �� w, �ֱ��ʾֻ
	  �����д��ʽ�����ݿ�. �ɹ����� true, ʧ�ܷ��� false.

      ȱʡ���������ֻ����ʽ��, �� mode ��ȱʡֵΪ 'r'
      $HDB->Open('/path/to/dict.hdb');

	  ���Զ�д��ʽ��(�½�����ʱ����), mode ֵΪ 'w', ��ʱ���ݿ�ɶ���д
	  $HDB->Open('/path/to/dict.hdb', 'w');

   3. ���� key ��ȡ���� mixed Get(string key [, bool verbose])
      �ɹ����ҵ� key ����Ӧ������ʱ������������, ����Ϊ string
	  �� key �����������ݿ���ʱ���������ֱ�ӷ��� false
	  (*ע* �� verbose ����Ϊ true ʱ, �򷵻�һ�������ļ�¼����, �� key&value, �����ڵ���Ŀ��)

      $value = $HDB->Get($key);
	  ��
	  $debug = $HDB->Get($key, true); print_r($debug);

   4. �������� bool Put(string key [, string value])
      �ɹ����� true, ʧ�ܻ������ false , �����Զ�д��ʽ�򿪲ſɵ���
	  ע����������Ŀǰֻ֧�� string ����, ��������Ҫ����ʹ�� php �ڽ��� serialize �� array ת��
	  �� string ȡ��ʱ���� unserialize() ��ԭ

	  $result = $HDB->Put($key, $value);

   5. �ر����ݿ�, void Close()
      $HDB->Close();

   6. ��ѯ�ļ��汾��, string Version()
      �������� HDB/0.1 ֮��ĸ�ʽ, �ǵ�ǰ�ļ��İ汾��

   7. ��¼����, mixed Next()
      ����һ����¼key, value ��ɵ�����, �����ڲ�ָ��������һλ, �ɵ��� Reset() ����ָ��
	  ��û�м�¼ʱ�᷵�� false, ����Ӧ������

	  $HDB->Reset();
	  while ($tmp = $HDB->Next())
	  {
		  echo "$tmp[key] => $tmp[value]\n";
	  }
	  Ҳ�����ڵ������ݿ��ؽ��µ����ݿ�, ������������д���µ��ļ��յ�.

   8. ����ָ�븴λ, void Reset()
      �˺�����Ϊ���� Next() ʹ��
	  $HDB->Reset();

   9. �Ż����ݿ�, �����ݿ��е� btree ת������ȫ������. void Optimize([int index])
      �������ݿ���� key ���� hash ��ɢ�� mask �Ŷ�������, ������� index Ϊ 0~[mask-1]
	  ȱʡ����� index ֵΪ -1 ���Ż��������ݿ�, �����Զ�д��ʽ�򿪵����ݿ�����ø÷���

	  $HDB->Optimize();

  10. ��ӡ������, ��������ṹ����״ͼ, void Draw([int index])
      ���� index ͬ Optimize() �Ĳ���, �������޷���ֵ, ֱ�ӽ���� echo ����, �����ڵ��Ժ͹ۿ�
	  ����

	  $HDB->Draw(0);
	  $HDB->Draw(1);
	  ...

\* ----------------------------------------------------------------------- */
// Constant Define
define ('_HDB_BASE_KEY',	0x238f13af);
define ('_HDB_BASE_MSK',	0x3ffd);	// ���������: 16381
define ('_HDB_VERSION',		0x02);		// 0x01 ~ 0xff
define ('_HDB_TAGNAME',		'HDB');		// First bytes
define ('_HDB_MAXKLEN',		0xf0);		// max key length (<242)
define ('_HDB_MAXVLEN',		0xfeff);	// max value length (0xffff - 0x100);

// Class object Declare
class HashTreeDB
{
	// Public var
	var $fd = false;
	var $mode = 'r';
	var $hash_key = _HDB_BASE_KEY;
	var $hash_mask = _HDB_BASE_MSK;
	var $version = _HDB_VERSION;

	// Private
	var $rec_off = 0;
	var $trave_stack = array();
	var $trave_index = -1;

	// Debug test
	var $_io_times = 0;

	// Constructor Function
	function HashTreeDB($mask = 0, $key = 0)
	{
		if (0 != $mask) $this->hash_mask = $mask;
		if (0 != $key) $this->hash_key = $key;
	}

	// Open the database: read | write
	function Open($fpath, $mode = 'r')
	{
		// open the file
		$newdb = false;
		if ($mode == 'w')
		{
			// write & read only
			if (!($fd = @fopen($fpath, 'rb+')))
			{
				if (!($fd = @fopen($fpath, 'wb+')))
				{
					trigger_error("HDB::Open(), failed to write the db `" . basename($fpath) . "`", E_USER_WARNING);
					return false;
				}
				// create the header
				$this->_write_header($fd);
				$newdb = true;
			} else { // HACK to suport temp file name creation
			  if (filesize($fpath) == 0) {
			    $this->_write_header($fd);
			    $newdb = true;
			  }
			}
		}
		else
		{
			// read only
			if (!($fd = @fopen($fpath, 'rb')))
			{
				trigger_error("HDB::Open(), faild to read the db `" . basename($fpath) . "`", E_USER_WARNING);
				return false;
			}
		}

		// check the header
		if (!$newdb && !$this->_check_header($fd))
		{
			trigger_error("HDB::Open(), invalid db file `" . basename($fpath) . "`", E_USER_WARNING);
			fclose($fd);
			return false;
		}

		// set the variable
		if ($this->fd !== false) fclose($this->fd);
		$this->fd = $fd;
		$this->mode = $mode;
		$this->rec_off = ($this->hash_mask + 1) * 6 + 32;
		$this->Reset();
		return true;
	}

	// Insert Or Update the value
	function Put($key, $value)
	{
		// check the file description
		if (!$this->fd || $this->mode != 'w')
		{
			trigger_error("HDB::Put(), null db handler or readonly.", E_USER_WARNING);
			return false;
		}

		// check the length
		$klen = strlen($key);
		$vlen = strlen($value);
		if ($klen > _HDB_MAXKLEN || $vlen > _HDB_MAXVLEN)
			return false;

		// try to find the old data
		$rec = $this->_get_record($key);
		if (isset($rec['vlen']) && ($vlen <= $rec['vlen']))
		{
			// update the old value & length
			flock($this->fd, LOCK_EX);
			fseek($this->fd, $rec['voff'], SEEK_SET);
			fwrite($this->fd, $value, $vlen);
			if ($vlen < $rec['vlen'])
			{
				$newlen = $rec['len'] + $vlen - $rec['vlen'];
				$newbuf = pack('v', $newlen);
				fseek($this->fd, $rec['poff'] + 4, SEEK_SET);
				fwrite($this->fd, $newbuf, 2);
			}
			fflush($this->fd);
			flock($this->fd, LOCK_UN);
			return true;
		}

		// ��������
		$new = array('loff' => 0, 'llen' => 0, 'roff' => 0, 'rlen' => 0);
		if (isset($rec['vlen']))
		{
			$new['loff'] = $rec['loff'];
			$new['llen'] = $rec['llen'];
			$new['roff'] = $rec['roff'];
			$new['rlen'] = $rec['rlen'];
		}
		$buf  = pack('VvVvC', $new['loff'], $new['llen'], $new['roff'], $new['rlen'], $klen);
		$buf .= $key . $value;

		$len  = $klen + $vlen + 13;
		flock($this->fd, LOCK_EX);
		fseek($this->fd, 0, SEEK_END);
		$off = ftell($this->fd);
		if ($off < $this->rec_off)
		{
			$off = $this->rec_off;
			fseek($this->fd, $off, SEEK_SET);
		}
		fwrite($this->fd, $buf, $len);
		$pbuf = pack('Vv', $off, $len);
		fseek($this->fd, $rec['poff'], SEEK_SET);
		fwrite($this->fd, $pbuf, 6);
		fflush($this->fd);
		flock($this->fd, LOCK_UN);
		return true;
	}

	// Read the value by key
	function Get($key, $verbose = false)
	{
		// check the file description
		if (!$this->fd)
		{
			trigger_error("HDB::Get(), null db handler.", E_USER_WARNING);
			return false;
		}

		// get the data?
		$rec = $this->_get_record($key);
		if ($verbose) return $rec;
		if (!isset($rec['value'])) return false;
		return $rec['value'];
	}

	// Read the each key & value
	// return array(key => xxx, value => xxx)
	function Next()
	{
		// check the file description
		if (!$this->fd)
		{
			trigger_error("HDB::Next(), null db handler.", E_USER_WARNING);
			return false;
		}

		// Traversal the all tree
		if (!($pointer = array_pop($this->trave_stack)))
		{
			do
			{
				$this->trave_index++;
				if ($this->trave_index >= $this->hash_mask) break;

				$poff = $this->trave_index * 6 + 32;
				fseek($this->fd, $poff, SEEK_SET);
				$buf = fread($this->fd, 6);
				if (strlen($buf) != 6) { $pointer = false; break; }
				$pointer = unpack('Voff/vlen', $buf);
			}
			while (!$pointer['len']);
		}

		// end the all records?
		if (!$pointer || $pointer['len'] == 0)
			return false;

		$rec = $this->_tree_get_record($pointer['off'], $pointer['len']);

		// push the left & right
		if ($rec['llen'] != 0)
		{
			$left = array('off' => $rec['loff'], 'len' => $rec['llen']);
			array_push($this->trave_stack, $left);
		}
		if ($rec['rlen'] != 0)
		{
			$right = array('off' => $rec['roff'], 'len' => $rec['rlen']);
			array_push($this->trave_stack, $right);
		}

		// return value
		$ret = array('key' => $rec['key'], 'value' => $rec['value']);
		return $ret;
	}

	// Traversal every tree... & debug to test
	function Draw($i = -1)
	{
		if ($i < 0 || $i >= $this->hash_mask)
		{
			$i = 0;
			$j = $this->hash_mask;
		}
		else
		{
			$j = $i + 1;
		}
		while ($i < $j)
		{
			$poff = $i * 6 + 32;
			fseek($this->fd, $poff, SEEK_SET);
			$buf = fread($this->fd, 6);
			if (strlen($buf) != 6) break;
			$pot = unpack('Voff/vlen', $buf);
			if ($pot['len'] == 0)
				echo "EMPTY tree [$i]\n";
			else
			{
				$this->_cur_depth = 0;
				$this->_cur_lkey = '';
				$this->_node_num = 0;
				$this->_draw_node($pot['off'], $pot['len']);
				echo "-------------------------------------------\n";
				echo "Tree[$i] max_depth: {$this->_cur_depth} ";
				echo "nodes_num: {$this->_node_num} bottom_key: {$this->_cur_lkey}\n";
			}
			$i++;
		}
	}

	// Reset the inner pointer
	function Reset()
	{
		$this->trave_stack = array();
		$this->trave_index = -1;
	}

	// Show the version
	function Version()
	{
		$ver = (is_null($this) ? _HDB_VERSION : $this->version);
		$str = sprintf("%s/%d.%d\n", _HDB_TAGNAME, ($ver >> 4), ($ver & 0x0f));
		return $str;
	}

	// Close the DB
	function Close()
	{
		if ($this->fd)
		{
			fclose($this->fd);
			$this->fd = false;
		}
	}

	// Optimize the tree
	function Optimize($i = -1)
	{
		// check the file description
		if (!$this->fd || $this->mode != 'w')
		{
			trigger_error("HDB::Optimize(), null db handler or readonly.", E_USER_WARNING);
			return false;
		}

		// get the index zone:
		if ($i < 0 || $i >= $this->hash_mask)
		{
			$i = 0;
			$j = $this->hash_mask;
		}
		else
		{
			$j = $i + 1;
		}
		while ($i < $j)
		{
			$this->_optimize_index($i);
			$i++;
		}
	}

	// optimize a node
	function _optimize_index($index)
	{
		static $cmp = false;
		$poff = $index * 6 + 32;

		// save all nodes into array()
		$this->_sync_nodes = array();
		$this->_load_tree_nodes($poff);

		$count = count($this->_sync_nodes);
		if ($count < 3) return;

		// sync the nodes, sort by key first
		if ($cmp == false) $cmp = create_function('$a,$b', 'return strcmp($a[key],$b[key]);');
		usort($this->_sync_nodes, $cmp);
		$this->_reset_tree_nodes($poff, 0, $count - 1);
		unset($this->_sync_nodes);
	}

	// load tree nodes
	function _load_tree_nodes($poff)
	{
		fseek($this->fd, $poff, SEEK_SET);
		$buf = fread($this->fd, 6);
		if (strlen($buf) != 6) return;

		$tmp = unpack('Voff/vlen', $buf);
		if ($tmp['len'] == 0) return;
		fseek($this->fd, $tmp['off'], SEEK_SET);
		$buf = fread($this->fd, $tmp['len']);
		$rec = unpack('Vloff/vllen/Vroff/vrlen/Cklen', substr($buf, 0, 13));
		$rec['off'] = $tmp['off'];
		$rec['len'] = $tmp['len'];
		$rec['key'] = substr($buf, 13, $rec['klen']);
		$this->_sync_nodes[] = $rec;
		unset($buf);
		// left
		if ($rec['llen'] != 0) $this->_load_tree_nodes($tmp['off']);
		// right
		if ($rec['rlen'] != 0) $this->_load_tree_nodes($tmp['off'] + 6);
	}

	// sync the tree
	function _reset_tree_nodes($poff, $low, $high)
	{
		if ($low <= $high)
		{
			$mid = ($low+$high)>>1;
			$node = $this->_sync_nodes[$mid];
			$buf = pack('Vv', $node['off'], $node['len']);
			// left
			$this->_reset_tree_nodes($node['off'], $low, $mid - 1);
			// right
			$this->_reset_tree_nodes($node['off'] + 6, $mid + 1, $high);
		}
		else
		{
			$buf = pack('Vv', 0, 0);
		}
		fseek($this->fd, $poff, SEEK_SET);
		fwrite($this->fd, $buf, 6);
	}

	// Privated Function
	function _get_index($key)
	{
		$l = strlen($key);
		$h = $this->hash_key;
		while ($l--)
		{
			$h += ($h << 5);
			$h ^= ord($key[$l]);
			$h &= 0x7fffffff;
		}
		return ($h % $this->hash_mask);
	}

	// draw the tree nodes by off & len
	//
	function _draw_node($off, $len, $rl = 'T', $icon = '', $depth = 0)
	{
		if ($rl == 'T')	echo '(��) ';
		else
		{
			echo $icon;
			if ($rl == 'L')
			{
				$icon .= ' ��';
				echo ' ��(��) ';
			}
			else
			{
				$icon .= ' ��';
				echo ' ��(��) ';
			}
		}
		if ($len == 0)
		{
			echo "<NULL>\n";
			return;
		}
		$rec = $this->_tree_get_record($off, $len);
		//echo "$rec[key] => $rec[value]\n";
		echo "$rec[key] (vlen: $rec[vlen])\n";

		// debug used
		$this->_node_num++;
		if ($depth >= $this->_cur_depth)
		{
			$this->_cur_depth = $depth;
			$this->_cur_lkey = $rec['key'];
		}

		// Left node & Right Node
		$this->_draw_node($rec['loff'], $rec['llen'], 'L', $icon, $depth + 1);
		$this->_draw_node($rec['roff'], $rec['rlen'], 'R', $icon, $depth + 1);
	}

	// Check HDB Header
	function _check_header($fd)
	{
		fseek($fd, 0, SEEK_SET);
		$buf = fread($fd, 32);
		if (strlen($buf) !== 32) return false;
		$hdr = unpack('a3tag/Cver/Vkey/Vmask/a20reversed', $buf);
		if ($hdr['tag'] != _HDB_TAGNAME) return false;
		$this->hash_key = $hdr['key'];
		$this->hash_mask = $hdr['mask'];
		$this->version = $hdr['ver'];
		return true;
	}

	// Write HDB Header
	function _write_header($fd)
	{
		$buf = pack('a3CVVa20', _HDB_TAGNAME, $this->version, $this->hash_key, $this->hash_mask, '');
		fseek($fd, 0, SEEK_SET);
		fwrite($fd, $buf, 32);
	}

	// get the record by first key
	function _get_record($key)
	{
		$this->_io_times = 1;
		$index = $this->_get_index($key);
		$poff = $index * 6 + 32;
		fseek($this->fd, $poff, SEEK_SET);
		$buf = fread($this->fd, 6);

		if (strlen($buf) == 6) $tmp = unpack('Voff/vlen', $buf);
		else $tmp = array('off' => 0, 'len' => 0);

		return $this->_tree_get_record($tmp['off'], $tmp['len'], $poff, $key);
	}

	// get the record by tree
	function _tree_get_record($off, $len, $poff = 0, $key = '')
	{
		$ret = array('poff' => $poff);
		if ($len == 0) return $ret;

		$this->_io_times++;
		// get the data & compare the key data
		fseek($this->fd, $off, SEEK_SET);
		$buf = fread($this->fd, $len <= 256 ? $len : 256);
		$rec = unpack('Vloff/vllen/Vroff/vrlen/Cklen', substr($buf, 0, 13));
		$fkey = substr($buf, 13, $rec['klen']);
		$cmp = ($key ? strcmp($key, $fkey) : 0);
		if ($cmp > 0)
		{
			// --> right
			return $this->_tree_get_record($rec['roff'], $rec['rlen'], $off + 6, $key);
		}
		else if ($cmp < 0)
		{
			// <-- left
			return $this->_tree_get_record($rec['loff'], $rec['llen'], $off, $key);
		}
		else {
			// found!!
			$rec['poff'] = $poff;
			$rec['off'] = $off;
			$rec['len'] = $len;
			$rec['voff'] = $off + 13 + $rec['klen'];
			$rec['vlen'] = $len - 13 - $rec['klen'];
			$rec['key'] = $fkey;

			// get the value
			if ($len <= 256)
				$rec['value'] = substr($buf, $rec['klen'] + 13, $rec['vlen']);
			else
			{
				fseek($this->fd, $rec['voff'], SEEK_SET);
				$rec['value'] = fread($this->fd, $rec['vlen']);
			}
			return $rec;
		}
	}
}
?>
