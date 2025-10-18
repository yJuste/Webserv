#!/usr/bin/env python3
import os
import subprocess

path_info = os.environ.get("PATH_INFO", "")
base_path = os.path.dirname(__file__)
file_path = os.path.join(base_path, path_info.lstrip("/"))
real_file_path = os.path.realpath(file_path)
real_base_path = os.path.realpath(base_path)

relative_path = os.path.relpath(real_file_path, real_base_path)

if not real_file_path.startswith(real_base_path):
    print("cgi: Error: Access denied.")
    exit()

if os.access(real_file_path, os.X_OK):
    try:
        if os.path.samefile(real_file_path, os.path.realpath(__file__)):
            print(f"cgi: Error: Refusing to execute itself ('{relative_path}').")
            exit()
        output = subprocess.check_output([real_file_path], stderr=subprocess.STDOUT)
        print(output.decode())
    except Exception:
        print(f"cgi: Error executing file: Cannot execute '{relative_path}' (permission denied or runtime error or wrong pattern).")
else:
    print(f"cgi: Error: '{relative_path}' is not executable or does not exist. (Try chmod +x)")
