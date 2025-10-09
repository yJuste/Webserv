#!/usr/bin/env python3

import cgi
import cgitb
cgitb.enable()  # Enable debugging

form = cgi.FieldStorage()
name = form.getvalue("name")
email = form.getvalue("email")

print(f"<html><body>")
print(f"<h2>Received POST Data</h2>")
print(f"<p>Name: {name}</p>")
print(f"<p>Email: {email}</p>")
print(f"</body></html>")
