#!/bin/bash

# Début du contenu HTML
echo "<html><head><title>CGI Loop Test</title></head><body>"
echo "<h1>CGI started...</h1>"

# Boucle infinie
while true; do
    echo "<p>Stop at $(date)</p>"
    sleep 1
done

# Fin du contenu HTML (jamais atteinte)
echo "</body></html>"
