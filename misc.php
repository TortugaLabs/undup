<?php
function stat1($fn,$key) {
    $st = stat($fn);
    if ($st === false) return false;
    return $st[$key];
}

$i=0;
foreach (['MODE','MTIME','UID','GID','SIZE','BLOCKS','MD5','MD5_32K','COUNT'] as $enum) {
    define('FS_'.$enum,$i++);
}

function pack_meta($st,$md5_32k,$md5) {
    return array(sprintf('%04o',$st['mode'] & 07777),
                 $st['mtime'],$st['uid'],$st['gid'],
                 $st['size'],$st['blocks'],$md5,$md5_32k);
}

function db_unpack($v) {
    $v = explode(":",$v);

    $fpaths = array();
    $meta = array_slice($v,0,FS_COUNT);
    foreach ([FS_UID,FS_GID,FS_MTIME,FS_SIZE,FS_BLOCKS] as $i) {
        $meta[$i] = intval($meta[$i]);
    }
    foreach(array_slice($v,FS_COUNT) as $f) {
        $f = db_decode($f);
        $fpaths[$f] = $f;
    }
    return [$meta,$fpaths];
}

function db_read(&$db,$ino) {
    $v = $db->Get($ino);
    if ($v === false) return [false,array()];
    return db_unpack($v);
}

function db_write(&$db,$ino,$meta,$fpaths) {
    $v = $meta;
    foreach ($fpaths as $f) {
        $v[] = db_encode($f);
    }
    $db->Put($ino,implode(':',$v));
}

function db_encode($f) {
    $f = str_replace('%','%25',$f);
    $f = str_replace(':','%3A',$f);
    return $f;
}
function db_decode($f) {
    return rawurldecode($f);
}

function md5_file32k($fn) {
    return '_x_';
    $max = 32768;
    //$max = 8192;
    if (false === ($fp = fopen($fn,'r'))) return '-';
    if (filesize($fn) < $max) $max = filesize($fn);
    if ($max == 0) {
        $md5 = md5('');
    } else {
        $md5 = md5(fread($fp,$max));
    }
    fclose($fp);
    return $md5;
}

function tmpdb($hdbfile) {
    $tmpfile = tempnam(dirname($hdbfile),'tmp.'.basename($hdbfile).'.');
    //unlink($tmpfile);
    return $tmpfile;
}

function calc_blksize($fs) {
    //return stat1($fs,'blksize');
    if (!is_dir($fs)) $fs = dirname($fs);

    $tmpfile = tempnam($fs,'tmp.stat.');
    $fh = fopen($tmpfile,'w');
    fwrite($fh,'1');
    fclose($fh);
    $st = stat($tmpfile);
    unlink($tmpfile);
    return $st ? intval($st['blksize']/$st['blocks']) : 512;
}
