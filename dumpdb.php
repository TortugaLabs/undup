<?php

function dumpdb($fn) {
  $db = new HashTreeDB;
  if (!$db->Open($fn,'r')) die("Unable to open $fn\n");

  $db->Reset();
  while ($tmp = $db->Next()) {
    echo $tmp['key'].' => '.$tmp['value']."\n";
  }
  $db->Close();
}
