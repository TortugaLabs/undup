<?php
//
// Scan filesystem
//
function fs_scan($root,$stdev,$hdbfile,&$oo) {
  $db = new HashTreeDB;
  if (!$db->Open($hdbfile,'w')) die("Unable to open $hdbfile\n");
  $dirs = array('');

  while (count($dirs)) {
    $d = array_shift($dirs);
    if (false === ($ztdev = stat1($root.$d,'dev'))) {
      trigger_error("Unable to stat directory: $root$d",E_USER_WARNING);
      continue;
    }
    if ($stdev != $ztdev) {
      trigger_error("Crossing device boundarys, skipping: $d",E_USER_NOTICE);
      continue;
    }
    if (false === ($dh = opendir($root.$d))) {
      trigger_error("Unable to open directory: $root$d",E_USER_WARNING);
      continue;
    }
    $q = $d == '' ? '' : '/';
    while (false !== ($f = readdir($dh))) {
      if ($f == '.' || $f == '..') continue;
      $fp = $d.$q.$f;
      if ($oo !== false) fwrite($oo,$fp."\n");
      if (is_link($root.$fp)) continue; // Skip symlinks
      if (is_dir($root.$fp)) {
	$dirs[] = $fp;
	continue;
      }
      if (!is_file($root.$fp)) continue; // Skip special files

      if (false === ($st = stat($root.$fp))) {
	trigger_error("Unable to stat file: $fp",E_USER_WARNING);
	continue;
      }

      list($pmeta,$fpaths) = db_read($db,$st['ino']);
      $cmd5_32k = '';
      $cmd5 = '';

      if ($pmeta !== false) {
	$save = false;
	if ($st['mtime'] == $pmeta[FS_MTIME] &&
	    $st['size'] == $pmeta[FS_SIZE] &&
	    $st['blocks'] == $pmeta[FS_BLOCKS]) {
	  $cmd5_32k = md5_file32k($root.$fp);
	  if ($cmd5_32k == $pmeta[FS_MD5_32K]) {
	    $cmd5 = $pmeta[FS_MD5];
	  }
	}
      } else {
	$save = true;
      }
      if ($cmd5_32k == '') $cmd5_32k = md5_file32k($root.$fp);
      if ($cmd5 == '') {
	global $trout;
	if ($trout) echo "CHKSUM $fp..";
	$cmd5 = md5_file($root.$fp);
	if ($trout) echo "$cmd5\n";
      }

      $cmeta = pack_meta($st,$cmd5_32k,$cmd5);
      if (!isset($fpaths[$fp])) {
	$fpaths[$fp] = $fp;
	$save = true;
      }
      if ($save || $cmeta != $pmeta) db_write($db,$st['ino'],$cmeta,$fpaths);
    }
  }
  closedir($dh);
  $db->Close();
}
