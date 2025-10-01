#!/usr/bin/env python3
import os
import cgi
import cgitb
import urllib.parse
import html

cgitb.enable()
form = cgi.FieldStorage()

# valeur passée (FieldStorage décode %XX automatiquement)
post_name = form.getvalue("name", "")

found_in_db = False
db_file = "./.users.db"
db_lines = []

db_error = None
try:
    with open(db_file, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            # stocker les deux premiers éléments (username et 'next part' si existante)
            parts = line.split(":", 2)  # on garde au plus 3 éléments pour ne pas perdre le reste
            # normalize length: username = parts[0], next_field = parts[1] if present else ""
            username = parts[0].strip() if len(parts) >= 1 else ""
            next_field = parts[1].strip() if len(parts) >= 2 else ""
            # décoder percent-encoding éventuel présent dans le fichier
            username_decoded = urllib.parse.unquote(username)
            next_field_decoded = urllib.parse.unquote(next_field)
            db_lines.append((username_decoded, next_field_decoded, line))
            # comparaison avec la valeur réelle passée (post_name est déjà décodée par FieldStorage)
            if username_decoded == post_name:
                found_in_db = True
                # on garde la ligne trouvée dans next_field_decoded si besoin
                found_entry = (username_decoded, next_field_decoded)
                break
except Exception as e:
    db_error = str(e)

# Output HTTP header
print("Content-Type: text/html; charset=utf-8")
print()

# HTML content
print("<html>")
print("<head><title>Python CGI Test</title></head>")
print("<body>")
print("<h1>Python CGI Test</h1>")

print("<p>Query string: {}</p>".format(html.escape(os.environ.get("QUERY_STRING", ""))))
print("<p>POST name parameter: {}</p>".format(html.escape(post_name)))

if db_error:
    print("<p style='color:red;'>Error reading database: {}</p>".format(html.escape(db_error)))
else:
    if found_in_db:
        uname = html.escape(found_entry[0])
        extra = html.escape(found_entry[1])
        print("<p style='color:green;'>Name '{}' found in .users.db ✅</p>".format(uname))
        # optionnel: afficher le second champs si présent
        if extra:
            print("<p>Second field: {}</p>".format(extra))
    else:
        print("<p style='color:red;'>Name '{}' NOT found in .users.db ❌</p>".format(html.escape(post_name)))

# List all POST parameters
print("<h2>POST parameters</h2>")
print("<ul>")
for key in form.keys():
    print("<li>{} = {}</li>".format(html.escape(key), html.escape(form.getvalue(key))))
print("</ul>")

print("</body>")
print("</html>")
