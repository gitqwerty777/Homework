<?php

$file = fopen("jp.csv","r");
while(!feof($file)){
	print_r(fgetcsv($file));
	echo "<br>";
}
fclose($file);

?> 