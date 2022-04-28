<meta http-equiv="content-type" content="text/html; charset=utf-8"> 
<?php
$linkid = mysql_connect("localhost","root","1491625");
mysql_select_db('JPML');
mysql_query('SET NAMES UTF8');

if(!$linkid) echo "連線失敗<br><br>";
else echo "連線成功<br><br>";

//queryForJP("1132");
 /*
$JPMeta = array("黃氏家譜","第一冊");
$personMeta = array("p.77","黃","家富","永泉","宋宥淳","XXX",1,"1991-11-07","3000-11-07",1,"總統","彰化","彰化","第一代","第一房","阿富","富富","no comment");
$personMeta2 = array("p.78","黃","毅","家富","XXX","女人",1,"2000-11-07","3100-11-07",1,"宅","彰化","彰化","第一代","第三房","yee","yeeeeeeeeee","no comment");
$personMeta3 = array("p.78","黃","睿","毅","XXX","女人",1,"2000-11-07","3100-11-07",1,"宅","彰化","彰化","第一代","第三房","yee","yeeeeeeeeee","no comment");
$personMeta4 = array("p.78","黃","琳","家富","XXX","女人",1,"2000-11-07","3100-11-07",1,"宅","彰化","彰化","第一代","第三房","yee","yeeeeeeeeee","no comment");


$JP_id = addJP($JPMeta);
echo "家譜id值為".$JP_id."<br>";

$person_id = addPerson($JP_id, $personMeta);
$person_id = addPerson($JP_id, $personMeta2);
$person_id = addPerson($JP_id, $personMeta3);
$person_id = addPerson($JP_id, $personMeta4);

//updateInfo("26","name", "家家富富富");
$name = findPersonId($JP_id,"家富");
deletePerson($name);
*/
//自訂sql指令
function sqlInstruction($sql_instruction){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$result = mysql_query($sql_instruction,$linkid);
	$Array =mysql_fetch_array($result);
	echo "fetch success";
	return $Array;
}
//查詢族譜ID
function findJPId($JPName){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "SELECT id FROM JPinfo WHERE name='".$JPName."'";
	$result = mysql_query($query,$linkid);
	$JPid =mysql_fetch_array($result);
	return $JPid[0];
}
//新增家譜
function addJP($JPMeta){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "INSERT INTO JPinfo (name, volume) VALUES ('".$JPMeta[0]."','".$JPMeta[1]."')";
	$result = mysql_query($query,$linkid);
	if($result) echo "success insert JP information<br>";
	return findJPId($JPMeta[0],$linkid);
}
//新增家譜(By Yi)
function addJPYi($JPMeta){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "INSERT INTO JPinfo (name, volume) VALUES ('".$JPMeta[0]."','".$JPMeta[1]."')";
	$result = mysql_query($query,$linkid);
	$rid = mysql_insert_id($linkid);
	if($result) echo "success insert JP information<br>";
	return $rid;
}
//查詢人ID
function findPersonId($JPid, $name){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "SELECT id FROM info WHERE name='".$name."' AND JPid='".$JPid."'";
	$result = mysql_query($query,$linkid);
	$id =mysql_fetch_array($result);
	return $id[0];
}
//新增一個人
function addPerson($JPid, $personMeta){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "INSERT INTO info (JPid, page, surname, name, father, mother, spouse, gender, birth_time, dead_time, sibling_order, goverment_position, hometown, grave, generation, funpai, zi, hao, note) VALUES ('".$JPid."','".$personMeta[0]."','".$personMeta[1]."','".$personMeta[2]."','".$personMeta[3]."','".$personMeta[4]."','".$personMeta[5]."',".$personMeta[6].",'".$personMeta[7]."','".$personMeta[8]."',".$personMeta[9].",'".$personMeta[10]."','".$personMeta[11]."','".$personMeta[12]."','".$personMeta[13]."','".$personMeta[14]."','".$personMeta[15]."','".$personMeta[16]."','".$personMeta[17]."')";
	echo "<br>" . "QUERY:" .$query . "<br>";
	$result = mysql_query($query,$linkid);
	if($result) echo "success insert person information<br>";
	else echo "add person fail.<br>";
	/*
	#新增父子關係
	$parent_id = findPersonId($JPid, $personMeta[3]);
	$person_id = findPersonId($JPid, $personMeta[2]);
	$stepson_id = "-1";
	addPersonParent($JPid, $parent_id, $person_id, $stepson_id);
	#新增夫妻關係
	$housband_id = findPersonId($JPid, $personMeta[2]);
	$wife_id = findPersonId($JPid, $personMeta[5]);
	addPersonSpouse($JPid, $housband_id, $wife_id);
	*/
	return $person_id;
}
//新增一個人(By Yi)
function addPersonYi($JPid, $personMeta){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	
	$query = "INSERT INTO info (JPid, page, surname, name, father, mother, spouse, gender, birth_time, dead_time, sibling_order, goverment_position, hometown, grave, generation, funpai, zi, hao, note) VALUES ('".$JPid."','".$personMeta[0]."','".$personMeta[1]."','".$personMeta[2]."','".$personMeta[3]."','".$personMeta[4]."','".$personMeta[5]."',".$personMeta[6].",'".$personMeta[7]."','".$personMeta[8]."',".$personMeta[9].",'".$personMeta[10]."','".$personMeta[11]."','".$personMeta[12]."','".$personMeta[13]."','".$personMeta[14]."','".$personMeta[15]."','".$personMeta[16]."','".$personMeta[17]."')";
	$result = mysql_query($query,$linkid);
	$rid = mysql_insert_id($linkid);
	if($result) echo "success insert person information<br>";
	else echo "add person fail.<br>";
	return $rid;
}
//新增父子關係
function addPersonParent($JPid, $parent_id, $person_id, $stepson_id){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "INSERT INTO son (JPid, father_id, son_id, stepson_id) VALUES ('".$JPid."','".$parent_id."','".$person_id."','".$stepson_id."')";
	
	echo "<br>" . "QUERY PARENT:" .$query . "<br>";
	
	
	$result = mysql_query($query,$linkid);
	if($result) echo "success insert parent relation<br>";
}
//新增夫妻關係
function addPersonSpouse($JPid, $housband_id, $wife_id){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "INSERT INTO spouse (JPid, housband_id, wife_id) VALUES ('".$JPid."','".$housband_id."','".$wife_id."')";
	
	echo "<br>" . "QUERY SPOUSE:" .$query . "<br>";
	
	
	$result = mysql_query($query,$linkid);
	if($result) echo "success insert busband & wife relation<br>";
}
//更新資料
function updateInfo($id, $column_name, $update_value){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "UPDATE info SET ".$column_name."='".$update_value."' WHERE id='".$id."'";
	$result = mysql_query($query,$linkid);
	if($result) echo "success update information<br>";
}
//刪除某人
function deletePerson($id){
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$delete_list = array();
	$delete_list[0] = $id;
	$list_counter = 1;
	$list_pointer = 0;
	
	do{
		$j =0;
		$son_id = array();
		$query="SELECT son_id FROM son WHERE father_id ='".$delete_list[$list_pointer]."'";
		$result = mysql_query($query,$linkid);
		while($temp = mysql_fetch_row($result)){
			$son_id[$j] = $temp[0]; 
			$j++;
		}
		echo $delete_list[$list_pointer]."找到".count($son_id)."個兒子".$son_id[0]."以及".$son_id[1]."<br>";
		for($i=0;$i<=count($son_id)-1;$i++){
			$delete_list[$list_counter] = $son_id[$i];
			$list_counter++;
		}
		$list_pointer++;
	}while($delete_list[$list_pointer]);
	
	for($i = 0 ;$i <=count($delete_list)-1;$i++){
		$query = "DELETE FROM info WHERE id='".$delete_list[$i]."'";
		$result = mysql_query($query,$linkid);
}
	echo "delete job done";
}

function queryForJP($JPid){
	$info_table=array();
	
	$linkid = mysql_connect("localhost","root","1491625");
	mysql_select_db('JPML');
	$query = "SELECT * FROM info WHERE JPid='".$JPid."'";
	$result = mysql_query($query,$linkid);
	$i=0;
	while($temp = mysql_fetch_row($result)){
		for($j=0;$j<=20-1;$j++){
			$info_table[$i][$j] = $temp[$j];
			//echo $info_table[$i][$j]."	/	";
		}
		$i++;
		//echo "<br>";
	}
	return $info_table;
}


?>