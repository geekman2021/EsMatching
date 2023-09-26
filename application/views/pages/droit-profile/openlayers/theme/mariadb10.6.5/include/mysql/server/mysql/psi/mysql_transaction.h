fig-file-path=/usr/lib \
--enable-debug=no \
--enable-safe-mode \
--with-exec-dir=/usr/bin \
--with-mysql=/usr \
--with-system-regex

     * Once this modification is made then build the binary rpm as
       follows:

rpm -bb /usr/src/redhat/SPECS/mod_php3.spec

     * Then install the rpm

rpm -ivh /usr/src/redhat/RPMS/i386/mod_php3-3.0.5-2.i386.rpm

   Make sure you restart Apache, and you now have PHP 3 with MySQL support
   using RPM's. Note that it is probably much easier to just build from
   the distribution tarball of PHP 3 and follow the instructions in
   INSTALL.REDHAT found in that distribution.

   6. Unix: I patched Apache with the FrontPage extensions patch, and
   suddenly PHP stopped working. Is PHP incompatible with the Apache
   FrontPage extensions?

   No, PHP works fine with the FrontPage extensions. The problem is that
   the FrontPage patch modifies several Apache structures, that PHP relies
   on. Recompiling PHP (using 'make clean ; make') after the FP patch is
   applied would solve the problem.

   7. Unix/Windows: I have installed PHP, but when I try to access a PHP
   script file via my browser, I get a blank screen.

   Do a 'view source' in the web browser and you will probably find that
   you can see the source code of your PHP script. This means that the web
   server did not send the script to PHP for interpretation. Something is
   wrong with the server configuration - double check the server
   configuration against the PHP installation instructions.

   8. Unix/Windows: I have installed PHP, but when try to access a PHP
   script file via my browser, I get a server 500 error.

   Something went wrong when the server tried to run PHP. To get to see a
   sensible error message, from the command line, change to the directory
   containing the PHP executable (php.exe on Windows) and run php -i. If
   PHP has any problems running, then a suitable error message will be
   displayed which will give you a clue as to what needs to be done next.
   If you get a screen full of HTML codes (the output of the phpinfo()
   function) then PHP is working, and your problem may be related to your
   server configuration which you should double check.

   9. Some operating systems: I have installed PHP without errors, but
   when I try to start apache I get undefined symbol errors:
[mybox:user /src/php5] root# apachectl configtest
 apachectl: /usr/local/apache/bin/httpd Undefined symbols:
  _compress
  _uncompress

   This has actually nothing to do with PHP, but with the MySQL client
   libraries. Some need --with-zlib, others do not. This is also covered
   in the MySQL FAQ.

   10. Windows: I have installed PHP, but when I to access a PHP script
   file via my browser, I get the error:
cgi error:
 The specified CGI application misbehaved by not
 returning a complete set of HTTP headers.
 The headers it did return are:

   This error message means that PHP failed to output anything at all. To
   get to see a sensible error message, from the command line, change to
   the directory containing the PHP executable (php.exe on Windows) and
   run php -i. If PHP has any problems running, then a suitable error
   message will be displayed which will give you a clue as to what needs
   to be done next. If you get a screen full of HTML codes (the output of
   the phpinfo() function) then PHP is working.

   Once PHP is working at the command line, try accessing the script via
   the browser again. If it still fails then it could be one of the
   following:

     * File permissions on your PHP script, php.exe, php5ts.dll, php.ini
       or any PHP extensions you are trying to load are such that the
       anonymous internet user ISUR_<machinename> cannot access them.
     * The script file does not exist (or possibly isn't where you think
       it is relative to your web root directory). Note that for IIS you
       can trap this error by ticking the 'check file exists' box when
       setting up the script mappings in the Internet Services Manager. If
       a script file does not exist then the server will return a 404
       error instead. There is also the additional benefit that IIS will
       do any authentication required for you based on the NTLanMan
       permissions on your script file.

   11. Windows: I've followed all the instructions, but still can't get
   PHP and IIS to work together!

   Make sure any user who needs to run a PHP script has the rights to run
   php.exe! IIS uses an anonymous user which is added at the time IIS is
   installed. This user needs rights to php.exe. Also, any authenticated
   user will also need rights to execute php.exe. And for IIS4 you need to
   tell it that PHP is a script engine. Also, you will want to read this
   faq.

   12. When running PHP as CGI with IIS, OmniHTTPD or Xitami, I get
   the following error: Security Alert! PHP CGI cannot be accessed
   directly..

   You must set the cgi.force_redirect directive to 0. It defaults to 1 so
   be sure the directive isn't commented out (with a ;). Like all
   directives, this is set in php.ini

   Because the default is 1, it's critical that you're 100% sure that the
   correct php.ini file is being read. Read this faq for details.

   13. How do I know if my php.ini is being found and read? It seems like
   it isn't as my changes aren't being implemented.

   To be sure your php.ini is being read by PHP, make a call to phpinfo()
   and near the top will be a listing called Configuration File (php.ini).
   This will tell you where PHP is looking for php.ini and whether or not
   it's being read. If just a directory PATH exists than it's not being
   read and you should put your php.ini in that directory. If php.ini is
   included within the PATH than it is being read.

   If php.ini is being read and you're running PHP as a module, then be
   sure to restart your web server after making changes to php.ini

   14. How do I add my PHP directory to the PATH on Windows?

   On Windows NT, 2000, XP and 2003:

     * Go to Control Panel and open the System icon (Start -> Settings ->
       Control Panel -> System, or just Start -> Control Panel -> System
       for Windows XP/2003)
     * Go to the Advanced tab
     * Click on the 'Environment Variables' button
     * Look into the 'System Variables' pane
     * Find the Path entry (you may need to scroll to find it)
     * Double click on the Path entry
     * Enter your PHP directory at the end, including ';' before (e.g.
       ;C:\php)
     * Press OK and restart your computer

   On Windows 98/Me you need to edit the autoexec.bat file:

     * Open the Notepad (Start -> Run and enter notepad)
     * Open the C:\autoexec.bat file
     * Locate the line with PATH=C:\WINDOWS;C:\WINDOWS\COMMAND;..... and
       add: ;C:\php to the end of the line
     * Save the file and restart your computer

     Note: Be sure to reboot after following the steps above to ensure
     that the PATH changes are applied.

   The PHP manual used to promote the copying of files into the Windows
   system directory, this