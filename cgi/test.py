#!/usr/bin/env python3
import os
import cgi
import cgitb

cgitb.enable()  # Enable error reporting in browser

form = cgi.FieldStorage()

# Get QUERY_STRING and POST parameters
query = os.environ.get("QUERY_STRING", "")
name = form.getvalue("name", "Guest")

print("Content-Type: text/html")
print()
print("<html>")
print("<head><title>Python CGI Test</title></head>")
print("<body>")
print("<h1>Python CGI Test</h1>")
print("<p>Query string: {}</p>".format(query))
print("<p>POST name parameter: {}</p>".format(name))
print("<ul>")
for key in form.keys():
    print("<li>{} = {}</li>".format(key, form.getvalue(key)))
print("</ul>")
print("</body>")
print("</html>")
