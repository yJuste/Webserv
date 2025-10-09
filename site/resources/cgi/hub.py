#!/usr/bin/env python3
import os
import subprocess

# Récupérer PATH_INFO
path_info = os.environ.get("PATH_INFO", "")
base_path = os.path.dirname(__file__)
file_path = os.path.join(base_path, path_info.lstrip("/"))
real_file_path = os.path.realpath(file_path)
real_base_path = os.path.realpath(base_path)

# Calcul du chemin relatif pour affichage sécurisé
relative_path = os.path.relpath(real_file_path, real_base_path)

# Sécurité : empêcher les accès en dehors du dossier CGI
if not real_file_path.startswith(real_base_path):
    print("Error: Access denied.")
    exit()

# Exécution du fichier CGI cible
if os.access(real_file_path, os.X_OK):
    try:
        output = subprocess.check_output([real_file_path], stderr=subprocess.STDOUT)
        print(output.decode())
    except Exception:
        print(f"Error executing file: Cannot execute '{relative_path}' (permission denied or runtime error).")
else:
    print(f"Error: '{relative_path}' is not executable or does not exist. (Try chmod +x)")
