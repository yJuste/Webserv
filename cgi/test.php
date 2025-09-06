#!/usr/bin/php
<?php
// Enable error reporting
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Get GET/POST parameters
$name = isset($_REQUEST['name']) ? $_REQUEST['name'] : 'Guest';
$query = isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : '';

header("Content-Type: text/html");

echo "<html>";
echo "<head><title>PHP CGI Test</title></head>";
echo "<body>";
echo "<h1>PHP CGI Test</h1>";
echo "<p>Query string: $query</p>";
echo "<p>Name parameter: $name</p>";
echo "<ul>";
foreach ($_REQUEST as $key => $value) {
    echo "<li>$key = $value</li>";
}
echo "</ul>";
echo "</body>";
echo "</html>";
?>
