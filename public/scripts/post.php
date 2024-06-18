<?php
$method = $_SERVER['REQUEST_METHOD'] ?? 'CLI';
$name = $_GET['name'] ?? 'Guest';
$data = file_get_contents('php://stdin');

parse_str($data, $parsedData);

echo "<html>
<style>
.navbar {
	width: 750px;
	margin: auto;
	padding: 50px;
	display: flex;
	justify-content: space-around;
	font-size: 32px;
	font-weight: 500;
	letter-spacing: 1px;
}
.nav-link {
	text-decoration: none;
	color: black;
}
.active {
	text-decoration: underline;
}
</style>
<body>
<nav class=\"navbar\">
<a id=\"home-nav\" href=\"/\" class=\"nav-link\">Home</a>
<a id=\"upload-nav\" href=\"/upload\" class=\"nav-link\">Upload</a>
<a id=\"post-nav\" href=\"/post\" class=\"nav-link\">Post CGI</a>
</nav>
<h1>
Request Method: $method<br>
Hello, $name!<br>";
echo "Received data from stdin: <br>";
foreach ($parsedData as $key => $value) {
	echo htmlspecialchars($key) . ": " . htmlspecialchars($value) . "<br>";
}
echo "</h1>
</body>
</html>";
?>