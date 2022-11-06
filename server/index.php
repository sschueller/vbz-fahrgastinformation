<?php

$file = "/tmp/vbz" . date("YmdHis") . ".json";

exec('/usr/bin/node /var/www/vbz/index.js > ' . $file);

$output = file_get_contents($file);
header('Content-Type: application/json');
echo $output;
