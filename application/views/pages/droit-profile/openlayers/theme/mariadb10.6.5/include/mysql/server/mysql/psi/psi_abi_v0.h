able value (e.g.
       C:\php)
     * Press OK and restart your computer

   On Windows 98/Me you need to edit the autoexec.bat file:

     * Open the Notepad (Start -> Run and enter notepad)
     * Open the C:\autoexec.bat file
     * Add a new line to the end of the file: set PHPRC=C:\php (replace
       C:\php with the directory where php.ini is located). Please note
       that the path cannot contain spaces. For instance, if you have
       installed PHP in C:\Program Files\PHP, you would enter
       C:\PROGRA~1\PHP instead.
     * Save the file and restart your computer

   16. Is it possible to use Apache content negotiation (MultiViews
   option) with PHP?

   If links to PHP files include extension, everything works perfect. This
   FAQ is only for the case when links to PHP files don't include
   extension and you want to use content negotiation to choose PHP files
   from URL with no extension. In this case, replace the line AddType
   application/x-httpd-php .php with:
# PHP 4
AddHandler php-script php
AddType text/html php

# PHP 5
AddHandler php5-script php
AddType text/html php

   This solution doesn't work for Apache 1 as PHP module doesn't catch
   php-script.

   17. Is PHP limited to process GET and POST request methods only?

   No, it is possible to handle any request method, e.g. CONNECT. Proper
   response status can be sent with header(). If only GET and POST methods