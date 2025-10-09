#!/usr/bin/env python3
import os
import subprocess

# Récupérer PATH_INFO
path_info = os.environ.get("PATH_INFO", "")
base_path = os.path.dirname(__file__)
file_path = os.path.join(base_path, path_info.lstrip("/"))
real_file_path = os.path.realpath(file_path)
real_base_path = os.path.realpath(base_path)

# Sécurité
if not real_file_path.startswith(real_base_path):
    print("Error: Access denied.")
    exit()

# Exécution du fichier CGI cible
if os.access(real_file_path, os.X_OK):
    try:
        output = subprocess.check_output([real_file_path], stderr=subprocess.STDOUT)
        print(output.decode())
    except Exception as e:
        print(f"Error executing file: {e}")
else:
    print("Error: Target file is not executable ( maybe chmod +x [file] or not exist. ).")
