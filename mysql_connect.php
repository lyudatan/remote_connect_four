<?php
$db_host = "localhost";
$db_username = "root";
$db_pass = "";
$db_name = "arduino_db";

$con = @mysql_connect("$db_host" , "$db_username" , "$db_pass") or die ("could not connect to mysql");
@mysql_select_db("$db_name") or die ("no database");

$move = $_GET['countMove'];
$columns = $_GET['column'];


$sql = "INSERT INTO ard_DB (move, columns) VALUES ('$move', '$columns')";
mysql_query($sql, $con);
mysql_close($con);
?>