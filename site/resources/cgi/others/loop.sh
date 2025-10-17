#!/bin/bash

echo "<html><head><title>CGI Loop Test</title></head><body>"
echo "<h1>CGI started...</h1>"

while true; do
    echo "<p>Stop at $(date)</p>"
    sleep 1
done

echo "</body></html>"
