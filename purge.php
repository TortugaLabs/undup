<?php
function fs_purge($root,$hdbfile) {
  if (!is_readable($hdbfile)) return;

  $cdb = new HashTreeDB;
  if (!$cdb->Open($hdbfile,'r')) die("Unable to open $hdbfile\n");
  $wdb = new HashTreeDB;
  $tmpfile = tmpdb($hdbfile);
  if (!$wdb->Open($tmpfile,'w')) die("Unable to open $tmpfile\n");

  $count = 0;
  $cdb->Reset();
  while ($p = $cdb->Next()) {
    $ino = intval($p['key']);
    list($meta,$cpaths) = db_unpack($p['value']);
    $npaths = array();
    foreach ($cpaths as $f) {
      if (!is_file($root.$f)) continue;
      $nino = stat1($root.$f,'ino');
      if ($nino != $ino) continue;
      $npaths[$f] = $f;
    }
    if (count($cpaths) != count($npaths)) ++$count;
    if (count($npaths) == 0) continue;
    if (count($cpaths) != count($npaths)) {
      db_write($wdb,$ino,$meta,$npaths);
    } else {
      $wdb->Put($ino,$p['value']);
    }
  }
  $wdb->Close();
  $cdb->Close();

  if ($count) {
    unlink($hdbfile);
    rename($tmpfile,$hdbfile);
  } else {
    unlink($tmpfile);
  }
}
