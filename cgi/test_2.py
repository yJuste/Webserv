#!/usr/bin/env python3
import os
import cgi
import cgitb
import sys
import urllib.parse

cgitb.enable()  # Enable error reporting in the browser

# --- Parse QUERY_STRING (GET parameters) ---
query = os.environ.get("QUERY_STRING", "")
get_params = urllib.parse.parse_qs(query)          # parse query string into a dictionary
get_name = get_params.get("name", ["Guest"])[0]    # get 'name' parameter or default "Guest"

# --- Parse POST parameters ---
form = cgi.FieldStorage()                          # parse POST form data
post_name = form.getvalue("name", "Guest")         # get 'name' from POST or default "Guest"

# --- Output HTTP header ---
print("Content-Type: text/html")
print()  # empty line separates headers from body

# --- HTML content ---
print("<html>")
print("<head><title>Python CGI Test</title></head>")
print("<body>")
print("<h1>Python CGI Test</h1>")

# Display GET parameters
print("<p>Query string: {}</p>".format(query))
print("<p>GET name parameter: {}</p>".format(get_name))

# Display POST parameters
print("<p>POST name parameter: {}</p>".format(post_name))

# List all POST parameters
print("<ul>")
for key in form.keys():
    print("<li>{} = {}</li>".format(key, form.getvalue(key)))
print("</ul>")

print("</body>")
print("</html>")
