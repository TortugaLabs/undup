<?php
function fs_dupmerge($root,$hdbfile,$do) {
  if (!is_readable($hdbfile)) die("You need to scan first\n");

  $cdb = new HashTreeDB;
  if (!$cdb->Open($hdbfile,'r')) die("Unable to open $hdbfile\n");
  $wdb = new HashTreeDB;
  $tmpfile = tmpdb($hdbfile);
  if (!$wdb->Open($tmpfile,'w')) die("Unable to open $tmpfile\n");

  $cand = array();

  $cdb->Reset();
  while ($p = $cdb->Next()) {
    $ino = intval($p['key']);
    list($meta,$cpaths) = db_unpack($p['value']);
    if ($meta[FS_BLOCKS] == 0) continue; // Ignore empty files..
    $meta = implode(':',$meta);
    $q = $wdb->Get($meta);
    if ($q == false) {
      $q = $ino;
    } else {
      $q .= ':'.$ino;
      $cand[$meta] = $q;
    }
    $wdb->Put($meta,$q);
  }

  $wdb->Close();
  unlink($tmpfile);

  $count = 0;
  foreach ($cand as $a=>$b) {
    $count += process_candidate($cdb,$root,$b,$do);
  }

  $cdb->Close();

  if ($count) {
    $blksize = calc_blksize($root);
    echo "blksize=$blksize\n";
    echo human_size(floatval($blksize)*floatval($count))." bytes (";
    echo number_format($count)." blocks)";
    if ($do) {
      echo " freed\n";
    } else {
      echo " can be freed\n";
    }
  }
}

function process_candidate(&$db,$root,$inolst,$do) {
  $c = array();
  if ($do) global $trout;

  foreach (explode(':', $inolst) as $ino) {
    list($meta,$fpaths) = db_read($db,intval($ino));
    if ($meta === false) {
      trigger_error("Inode $ino not found in hdb",E_USER_WARNING);
      return 0;
    }
    $fpaths = array_values($fpaths);
    $st = stat($root.$fpaths[0]);
    if ($st == false) return 0;
    if ($st['ino'] != intval($ino)) {
      trigger_error("Change detected in ".$fpaths[0],E_USER_WARNING);
      return 0;
    }
    $cmeta = pack_meta($st,$meta[FS_MD5_32K],$meta[FS_MD5]);
    if ($cmeta != $meta) {
      trigger_error("Inode $ino (".implode(',',$fpaths).")",
		    E_USER_WARNING);
      return 0;
    }
    // Also check if all paths have the same inode...
    foreach ($fpaths as $fp) {
      $x = stat1($root.$fp,'ino');
      if ($x != intval($ino)) {
	trigger_error("Change detected in $fp",E_USER_WARNING);
	return 0;
      }
    }
    $c[$ino] = $fpaths;
  }
  // So c contains all the inodes and paths...
  $count = 0;

  foreach ($c as $lst) {
    if (!isset($target)) {
      $target = $root.$lst[0];
      $sz = stat1($target,'blocks');
      if ($do) {
	if ($trout) echo('= '.$lst[0]."\n");
      } else {
	echo($lst[0].":\n");
      }
    } else {
      $count += $sz;
      foreach ($lst as $f) {
	if ($do) {
	  if ($trout) echo("    x $f\n");
	  unlink($root.$f);
	  link($target,$root.$f);
	} else {
	  echo("\t$f\n");
	}
      }
    }
  }
  return $count;
}