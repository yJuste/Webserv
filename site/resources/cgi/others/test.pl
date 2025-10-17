#!/usr/bin/perl

use strict;
use warnings;

print <<HTML;
Content-Type: text/html;

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Bienvenue</title>
    <style>
        body {
            background: linear-gradient(to right, #00c6ff, #0072ff);
            color: white;
            font-family: 'Segoe UI', sans-serif;
            text-align: center;
            padding-top: 100px;
            animation: fadeIn 2s ease-in;
        }
        h1 {
            font-size: 3em;
            margin-bottom: 0.5em;
        }
        p {
            font-size: 1.5em;
        }
        \@keyframes fadeIn {
            from { opacity: 0; transform: translateY(-20px); }
            to { opacity: 1; transform: translateY(0); }
        }
    </style>
</head>
<body>
<h1>Salut, Mister ! 👋</h1>
<p>How are you doing ?</p>
</body>
</html>
HTML
