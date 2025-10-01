#!/usr/bin/perl

use strict;
use warnings;
use CGI;
use POSIX qw(strftime);

my $q = CGI->new;
my $name = $q->param('name') || 'inconnu';
my $time = strftime "%H:%M:%S", localtime;

print $q->header('text/html; charset=UTF-8');

print <<'HTML';
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
HTML

print "<h1>Salut, $name ! 👋</h1>\n";
print "<p>Il est actuellement $time</p>\n";
print "<p>Bienvenue sur notre page CGI dynamique ✨</p>\n";

print <<'HTML';
</body>
</html>
HTML
