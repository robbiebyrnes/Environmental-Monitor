<?php

$servername = "localhost";

$dbname = "Enter";

$username = "Enter";

$password = "Enter";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "Enter";

$api_key = $sensor = $location = $Temperature = $Humidity = $Pressure = $WindSpeed = "";
// Create connection - uncomment for debug
        $conn = new mysqli($servername, $username, $password, $dbname);
        echo "Successful Connection...";
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    echo "post received...";
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $sensor = test_input($_POST["sensor"]);
        $location = test_input($_POST["location"]);
        $Temperature = test_input($_POST["Temperature"]);
        $Humidity = test_input($_POST["Humidity"]);
        $Pressure = test_input($_POST["Pressure"]);
        $WindSpeed = test_input($_POST["WindSpeed"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        echo "Successful Connection...";
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO SensorData (sensor, location, Temperature, Humidity, Pressure, WindSpeed)
        VALUES ('" . $sensor . "', '" . $location . "', '" . $Temperature . "', '" . $Humidity . "', '" . $Pressure . "', '" . $WindSpeed . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
