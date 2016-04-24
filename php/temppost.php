<?php

$servername = "localhost";
$username = "11620_27871";
$password = "hovnokleslo";
$dbname = "11620_esp";

$temp = htmlspecialchars($_POST["temperature"]);
$humi = htmlspecialchars($_POST["humidity"]);

// Create connection
$conn = mysqli_connect($servername, $username, $password, $dbname);
// Check connection
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}

$sql = "INSERT INTO teplomer (id, teplota, vlhkost, datum)
VALUES (null, '".$temp."', '".$humi."', NOW())";

if (mysqli_query($conn, $sql)) {
    echo "New record created successfully";
} else {
    echo "Error: " . $sql . "<br>" . mysqli_error($conn);
}

mysqli_close($conn);




?>
