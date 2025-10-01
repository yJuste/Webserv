#!/bin/bash

# Obligatoire : l'en-tête HTTP
echo "Content-Type: text/html"
echo ""

# Le contenu HTML
cat <<EOF
<html>
<head><title>CGI Test: SHELL</title></head>
<body>
<h1>Hello from CGI!</h1>
</body>
</html>
EOF
