<?php

$servername = "Enter";

// REPLACE with your Database name
$dbname = "Enter";
// REPLACE with Database user
$username = "Enter";
// REPLACE with Database user password
$password = "Enter";


//error_reporting(0);// With this no error reporting will be there
//////// Do not Edit below /////////

$connection = mysqli_connect($localhost, $username, $password, $dbname);


if (!$connection) {
    echo "Error: Unable to connect to MySQL.<br>";
    echo "<br>Debugging errno: " . mysqli_connect_errno();
    echo "<br>Debugging error: " . mysqli_connect_error();
    exit;
}
?>

