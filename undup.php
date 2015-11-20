<?php
$script_dir=dirname(realpath(__FILE__)).'/';
require_once($script_dir.'hdb.class.php');
require_once($script_dir.'human_size.php');
require_once($script_dir.'misc.php');
require_once($script_dir.'scanfs.php');
require_once($script_dir.'purge.php');
require_once($script_dir.'dupmerge.php');
require_once($script_dir.'dumpdb.php');

$argv0 = array_shift($argv);
$trout = true;
if (count($argv) && $argv[0] == '-n') {
  $trout = false;
  array_shift($argv);
}

if (count($argv) < 3) usage($argv0);

$cmd = array_shift($argv);
$hdbfile = array_shift($argv);
$root = array_shift($argv);

if (!is_dir($root)) die("$root: is not a directory\n");
$root = realpath($root).'/';
if ($root == '/') die("$root: can not refer to the top root\n");
if (false === ($stdev = stat1($root,'dev'))) die("Unable to stat $root\n");

$start = time();
function usage($cmd,$txt = '') {
  if ($txt != "") $txt .="\n";
  die($txt."Usage: $cmd sub-cmd HDBfile dirroot [opts]\n\n".
      "Sub commands:\n".
      "* scan : scan filesystem and update hdb file\n\tOptions:\n".
      "\t-oFILE : Generate a filelist.txt\n".
      "* dups|dupmerge : find duplicates\n\tOptions:\n".
      "\t--run : Actually change the filesystem\n"
      );
}

switch($cmd) {
case 'scan':
  $oo = false;
  if (count($argv)) {
    if (substr($argv[0],0,2) == '-o') {
      // Create an output filelist.txt
      $oo = fopen(substr($argv[0],2),'w');
      if ($oo === false) die("Unable to open output file\n");
      array_shift($argv);
    }
  }
  if (count($argv)) usage($cmd,"Invalid options passed");
  fs_purge($root,$hdbfile);
  fs_scan($root,$stdev,$hdbfile,$oo);
  break;
case 'dupmerge':
case 'dups':
  if (count($argv)) {
    if ($argv == ['--run']) {
      $do = true;
    } else {
      usage($cmd,"Invalid options");
    }
  } else {
    $do = false;
  }
  fs_dupmerge($root,$hdbfile,$do);
  break;
case 'dump':
case 'dumpdb':
  if (count($argv)) usage($cmd,"Invalid options");
  dumpdb($hdbfile);
  break;
default:
  usage($argv0,"Invalid command \"$cmd\"");
}

if ($trout) {
  echo 'Mem consuption: '.human_size(memory_get_peak_usage(true))."\n";
  echo time() - $start . " seconds\n";
}
