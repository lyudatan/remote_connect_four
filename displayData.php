<html>
<head>
   <title>Data</title>
</head>
<body>
<?php
$db_host = "localhost";
$db_username = "root";
$db_pass = "";
$db_name = "arduino_db";

$con = @mysql_connect("$db_host" , "$db_username" , "$db_pass") or die ("could not connect to mysql");
@mysql_select_db("$db_name") or die ("no database");

$sql = "SELECT * FROM ard_db ORDER BY move DESC";
$myData = mysql_query($sql, $con);
echo "<table border=1>
<tr>
<th>Column</th>
<th>Move</th>
</tr>";

while ($record = mysql_fetch_array($myData)){
echo"<tr>";
echo "<td>" . $record['columns'] . "</td>";
echo "<td>" . $record['move'] . "</td>";
echo "</tr>";
}

echo "</table>";
mysql_close($con);
?>
</body>
</html>