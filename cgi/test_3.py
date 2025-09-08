#!/usr/bin/env python3
import os
import cgi
import cgitb

cgitb.enable()  # Enable error reporting in browser

form = cgi.FieldStorage()

# Get QUERY_STRING and POST parameters
query = os.environ.get("QUERY_STRING", "")
post_name = form.getvalue("name", "Guest")  # 从 POST 表单获取 name

# Check if the name exists in user.db
# Check if the name exists in users.db
found_in_db = False
db_file = "./users.db"
try:
    with open(db_file, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            # 每行格式为 username:password
            username_password = line.split(":")
            if len(username_password) == 2:
                username, _ = username_password
                if username.strip() == post_name:
                    found_in_db = True
                    break
except Exception as e:
    db_error = str(e)
else:
    db_error = None

# Output HTTP header
print("Content-Type: text/html; charset=utf-8")
print()  # 空行分隔 header 和 body

# HTML content
print("<html>")
print("<head><title>Python CGI Test</title></head>")
print("<body>")
print("<h1>Python CGI Test</h1>")

print("<p>Query string: {}</p>".format(query))
print("<p>POST name parameter: {}</p>".format(post_name))

if db_error:
    print("<p>Error reading database: {}</p>".format(db_error))
else:
    if found_in_db:
        print("<p style='color:green;'>Name '{}' found in user.db ✅</p>".format(post_name))
    else:
        print("<p style='color:red;'>Name '{}' NOT found in user.db ❌</p>".format(post_name))

# List all POST parameters
print("<ul>")
for key in form.keys():
    print("<li>{} = {}</li>".format(key, form.getvalue(key)))
print("</ul>")

print("</body>")
print("</html>")
