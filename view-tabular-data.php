<html>
<head>
<meta http-equiv = 'refresh' content = '30'>
<title>Enter</title>
<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>
</head>
<div><center><h1>Enter</h1></center><div>
<br>
<div class='container'>
    
<center><a href=https://Enter.php>View latest 24 Hours - Line Graph</a></center>
<center><a href=https://Enter.php>View latest Month - Line Graph</a></center>
<br><br>

<?php
// Connection data (server_address, database, name, password)
$servername = "localhost";
$dbname = "Enter";
$username = "Enter";
$password = "Enter";

try {
  // Connect and create the PDO object
  $conn = new PDO("mysql:host=$servername; dbname=$dbname", $username, $password);
  $conn->exec("SET CHARACTER SET utf8");      // Sets encoding UTF-8
  $conn->query("SET @@session.time_zone = '+01:00'");

  // Define and perform the SQL query
$sql = "SELECT id, sensor, location, Temperature, Humidity, Pressure, WindSpeed, reading_time FROM SensorData ORDER BY id DESC";
  $result = $conn->query($sql);

  // If the SQL query is succesfully performed ($result not false)
  if($result !== false) {
    // Create the beginning of HTML table, and the first row with colums title
    $html_table = '<center><table border="1" cellspacing="0" cellpadding="2"><tr><th>ID</th><th>Sensor</th><th>Location</th><th>Temperature</th><th>Humidity</th><th>Pressure</th><th>WindSpeed</th><th>TimeStamp</th></tr></center>';

    // Parse the result set, and adds each row and colums in HTML table
    foreach($result as $row) {
      $html_table .= '<tr><td>' .$row['id']. '</td><td>' .$row['sensor']. '</td><td>' .$row['location']. '</td><td>' .$row['Temperature']. '</td><td>' .$row['Humidity']. '</td><td>' .$row['Pressure']. '</td><td>' .$row['WindSpeed']. '</td><td>' .$row['reading_time']. '</td></tr>';
    }
  }

  $conn = null;        // Disconnect

  $html_table .= '</table>';           // ends the HTML table

  echo $html_table;        // display the HTML table
}
catch(PDOException $e) {
  echo $e->getMessage();
}
?>
