____

Note about nsapi_virtual() and subrequests (PHP >= 4.3.3)

   The NSAPI module now supports the nsapi_virtual() function (alias:
   virtual()) to make subrequests on the web server and insert the result
   in the web page. The problem is, that this function uses some
   undocumented features from the NSAPI library.

   Under Unix this is not a problem, because the module automatically
   looks for the needed functions and uses them if available. If not,
   nsapi_virtual() is disabled.

   Under Windows limitations in the DLL handling need the use of a
   automatic detection of the most recent ns-httpdXX.dll file. This is
   tested for servers till version 6.1. If a newer version of the Sun
   server is used, the detection fails and nsapi_virtual() is disabled.

   If this is the case, try the following: Add the following parameter to
   php5_init in magnus.conf/obj.conf:
   Init fn=php5_init ... server_lib="ns-httpdXX.dll"

   where XX is the correct DLL version number. To get it, look in the
   server-root for the correct DLL name. The DLL with the biggest filesize
   is the right one.

   You can check the status by using the phpinfo() function.

     Note: But be warned: Support for nsapi_virtual() is EXPERIMENTAL!!!
     __________________________________________________________________

OmniHTTPd Server

   This section contains notes and hints specific to OmniHTTPd on Windows.

     Note: You should read the manual installation steps first!

   Warning

   By using the CGI setup, your server is open to several possible
   attacks. Please read our CGI security section to learn how to defend
   yourself from those attacks.

   You need to complete the following steps to make PHP work with
   OmniHTTPd. This is a CGI executable setup. SAPI is supported by
   OmniHTTPd, but some tests have shown that it is not so stable to use
   PHP as an ISAPI module.

     Important for CGI users: Read the faq on cgi.force_redirect for
     important details. This directive needs to be set to 0.

    1. Install OmniHTTPd server.
    2. Right click on the blue OmniHTTPd icon in the system tray and
       select Properties
    3. Click on Web Server Global Settings
    4. On the 'External' tab, enter: virtual = .php | actual =
       c:\php\php.exe (use php-cgi.exe if installing PHP 5), and use the
       Add button.
    5. On the Mime tab, enter: virtual = wwwserver/stdcgi | actual = .php,
       and use the Add button.
    6. Click OK

   Repeat steps 2 - 6 for each extension you want to associate with PHP.

     __________________________________________________________________

Xitami on Microsoft Windows

   This section contains notes and hints specific to Xitami on Windows.

     Note: You should read the manual installation steps first!

   This list describes how to set up the PHP CGI binary to work with
   Xitami on Windows.

     Important for CGI users: Read the faq on cgi.force_redirect for
     important details. This directive needs to be set to 0. If you want
     to use $_SERVER['PHP_SELF'] you have to enable the cgi.fix_pathinfo
     directive.

   Warning

   By using the CGI setup, your server is open to several possible
   attacks. Please read our CGI security section to learn how to defend
   yourself from those attacks.

     * Make sure the web server is running, and point your browser to
       xitamis admin console (usually http://127.0.0.1/admin), and click
       on Configuration.
     * Navigate to the Filters, and put the extension which PHP should
       parse (i.e. .php) into the field File extensions (.xxx).
     * In Filter command or script put the path and name of your PHP CGI
       executable i.e. C:\php\php-cgi.exe.
     * Press the 'Save' icon.
     * Restart the server to reflect changes.
     __________________________________________________________________

Installation of extensions on Windows

   After installing PHP and a web server on Windows, you will probably
   want to install some extensions for added functionality. You can choose
   which extensions you would like to load when PHP starts by modifying
   your php.ini. You can also load a module dynamically in your script
   using dl().

   The DLLs for PHP extensions are prefixed with php_.

   Many extensions are built into the Windows version of PHP. This means
   additional DLL files, and the extension directive, are not used to load
   these extensions. The Windows PHP Extensions table lists extensions
   that require, or used to require, additional PHP DLL files. Here's a
   list of built in extensions:

   In PHP 5 (updated PHP 5.0.4), the following changes exist. Built in:
   DOM, LibXML, Iconv, SimpleXML, SPL and SQLite. And the following are no
   longer built in: MySQL and Overload.

   The default location PHP searches for extensions is C:\php5 in PHP 5. 
   To change this setting to reflect your setup of PHP edit your php.ini
   file:

     * You will need to change the extension_dir setting to point to the
       directory where your extensions lives, or where you have placed
       your php_*.dll files. For example:

extension_dir = C:\php\extensions

     * Enable the extension(s) in php.ini you want to use by uncommenting
       the extension=php_*.dll lines in php.ini. This is done by deleting
       the leading ; from the extension you want to load.

   Example 2-8. Enable Bzip2 extension for PHP-Windows
// change the following line from ...
;extension=php_bz2.dll

// ... to
extension=php_bz2.dll

     * Some of the extensions need extra DLLs to work. Couple of them can
       be found in the distribution package, in in the main folder in PHP 5, 
       but some, for example Oracle (php_oci8.dll) require DLLs which are 
       not bundled with the distribution package.
     * Some of these DLLs are not bundled with the PHP distribution. See
       each extensions documentation page for details. Also, read the
       manual section titled Installation of PECL extensions for details
       on PECL. An increasingly large number of PHP extensions are found
       in PECL, and these extensions require a separate download.

     Note: If you are running a server module version of PHP remember to
     restart your web server to reflect your changes to php.ini.

   The following table describes some of the extensions available and
   required additional dlls.

   Table 2-1. PHP Extensions
   Extension Description Notes
   php_bz2.dll bzip2 compression functions None
   php_calendar.dll Calendar conversion functions
   php_cpdf.dll ClibPDF functions None
   php_crack.dll Crack functions None
   php_ctype.dll ctype family functions
   php_curl.dll CURL, Client URL library functions Requires: libeay32.dll,
   ssleay32.dll (bundled)
   php_db.dll DBM functions Deprecated. Use DBA instead (php_dba.dll)
   php_dba.dll DBA: DataBase (dbm-style) Abstraction layer functions None
   php_dbase.dll dBase functions None
   php_dbx.dll dbx functions
   php_exif.dll EXIF functions php_mbstring.dll. And, php_exif.dll must be
   loaded after php_mbstring.dll in php.ini.
   php_fdf.dll FDF: Forms Data Format functions. Requires: fdftk.dll
   (bundled)
   php_filepro.dll filePro functions Read-only access
   php_ftp.dll FTP functions
   php_gd2.dll GD library image functions GD2
   php_gettext.dll Gettext functions, requires libintl-1.dll, 
   iconv.dll (bundled).
   php_iconv.dll ICONV characterset conversion Requires: iconv.dll
   php_imap.dll IMAP POP3 and NNTP functions None
   php_interbase.dll InterBase functions Requires: gds32.dll (bundled)
   php_ldap.dll LDAP functions requires libeay32.dll, ssleay32.dll (bundled)
   php_mbstring.dll Multi-Byte String functions None
   php_mcrypt.dll Mcrypt Encryption functions Requires: libmcrypt.dll
   php_mime_magic.dll Mimetype functions Requires: magic.mime (bundled)
   php_ming.dll Ming functions for Flash None
   php_msql.dll mSQL functions Requires: msql.dll (bundled)
   php_mssql.dll MSSQL functions Requires: ntwdblib.dll (bundled)
   php_mysql.dll MySQL functions PHP >= 5.0.0, requires libmysql.dll
   (bundled)
   php_mysqli.dll MySQLi functions PHP >= 5.0.0, requires libmysql.dll
   (libmysqli.dll in PHP <= 5.0.2) (bundled)
   php_oci8.dll Oracle 8 functions Requires: Oracle 8.1+ client libraries
   php_openssl.dll OpenSSL functions Requires: libeay32.dll (bundled)
   php_oracle.dll Oracle functions Requires: Oracle 7 client libraries
   php_pgsql.dll PostgreSQL functions None
   php_printer.dll Printer functions None
   php_shmop.dll Shared Memory functions None
   php_snmp.dll SNMP get and walk functions NT only!
   php_soap.dll SOAP functions PHP >= 5.0.0
   php_sockets.dll Socket functions None
   php_sybase_ct.dll Sybase functions Requires: Sybase client libraries
   php_tidy.dll Tidy functions PHP >= 5.0.0
   php_tokenizer.dll Tokenizer functions Built in since PHP 4.3.0
   php_xmlrpc.dll XML-RPC functions PHP >= 4.2.1 requires: iconv.dll
   (bundled)
   php_xslt.dll XSLT requires libxslt.dll, iconv.dll (bundled).
   php_zip.dll Zip File functions
   php_zlib.dll ZLib compression functions
     __________________________________________________________________

Chapter 3. Installation of PECL extensions

Introduction to PECL Installations

   PECL is a repository of PHP extensions that are made available to you
   via the PEAR packaging system. This section of the manual is intended
   to demonstrate how to obtain and install PECL extensions.

   These instructions assume /your/phpsrcdir/ is the path to the PHP
   source distribution, and that extname is the name of the PECL
   extension. Adjust accordingly. These instructions also assume a
   familiarity with the pear command. The information in the PEAR manual
   for the pear command also applies to the pecl command.

   To be useful, a shared extension must be built, installed, and loaded.
   The methods described below provide you with various instructions on
   how to build and install the extensions, but they do not automatically
   load them. Extensions can be loaded by adding an extension directive.
   To this php.ini file, or through the use of the dl() function.

   When building PHP modules, it's important to have known-good versions
   of the required tools (autoconf, automake, libtool, etc.) See the
   SVN Instructions for details on the required tools, and required 
   versions.
     __________________________________________________________________

Downloading PECL extensions

   There are several options for downloading PECL extensions, such as:

     * http://pecl.php.net
       The PECL web site contains information about the different
       extensions that are offered by the PHP Development Team. The
       information available here includes: ChangeLog, release notes,
       requirements and other similar details.
     * pecl download extname
       PECL extensions that have releases listed on the PECL web site are
       available for download and installation using the pecl command.
       Specific revisions may also be specified.
     * SVN 
       Most PECL extensions also reside in SVN. A web-based view may be
       seen at http://svn.php.net/pecl/. To download straight from SVN,
       the following sequence of commands may be used. 

$ svn  co http://svn.php.net/repository/pecl/<extname>/trunk

     * Windows downloads
       Windows users may find compiled PECL binaries by downloading the
       Collection of PECL modules from the PHP Downloads page, or by
       retrieving a PECL Snapshot or an extension DLL on PECL4WIN. To
       compile PHP under Windows, read the appropriate chapter.
     __________________________________________________________________

PECL for Windows users

   As with any other PHP extension DLL, installation is as simple as
   copying the PECL extension DLLs into the extension_dir folder and
   loading them from php.ini. For example, add the following line to your
   php.ini:

   extension=php_extname.dll

   After doing this, restart the web server.
     __________________________________________________________________

Compiling shared PECL extensions with the pecl command

   PECL makes it easy to create shared PHP extensions. Using the pecl
   command, do the following:

   $ pecl install extname

   This will download the source for extname, compile, and install
   extname.so into your extension_dir. extname.so may then be loaded via
   php.ini

   By default, the pecl command will not install packages that are marked
   with the alpha or beta state. If no stable packages are available, you
   may install a beta package using the following command:

   $ pecl install extname-beta

   You may also install a specific version using this variant:

   $ pecl install extname-0.1
     __________________________________________________________________

Compiling shared PECL extensions with phpize

   Sometimes, using the pecl installer is not an option. This could be
   because you're behind a firewall, or it could be because the extension
   you want to install is not available as a PECL compatible package, such
   as unreleased extensions from SVN. If you need to build such an
   extension, you can use the lower-level build tools to perform the build
   manually.

   The phpize command is used to prepare the build environment for a PHP
   extension. In the following sample, the sources for an extension are in
   a directory named extname:

$ cd extname
$ phpize
$ ./configure
$ make
# make install

   A successful install will have created extname.so and put it into the
   PHP extensions directory. You'll need to and adjust php.ini and add an
   extension=extname.so line before you can use the extension.

   If the system is missing the phpize command, and precompiled packages
   (like RPM's) are used, be sure to also install the appropriate devel
   version of the PHP package as they often include the phpize command
   along with the appropriate header files to build PHP and its
   extensions.

   Execute phpize --help to display additional usage information.
     __________________________________________________________________

Compiling PECL extensions statically into PHP

   You might find that you need to build a PECL extension statically into
   your PHP binary. To do this, you'll need to place the extension source
   under the php-src/ext/ directory and tell the PHP build system to
   regenerate its configure script.

$ cd /your/phpsrcdir/ext
$ pecl download extname
$ gzip -d < extname.tgz | tar -xvf -
$ mv extname-x.x.x extname

   This will result in the following directory:

   /your/phpsrcdir/ext/extname

   From here, force PHP to rebuild the configure script, and then build
   PHP as normal:

$ cd /your/phpsrcdir
$ rm configure
$ ./buildconf --force
$ ./configure --help
$ ./configure --with-extname --enable-someotherext --with-foobar
$ make
$ make install

     Note: To run the 'buildconf' script you need autoconf 2.13 and
     automake 1.4+ (newer versions of autoconf may work, but are not
     supported).

   Whether --enable-extname or --with-extname is used depends on the
   extension. Typically an extension that does not require external
   libraries uses --enable. To be sure, run the following after buildconf:

   $ ./configure --help | grep extname
     __________________________________________________________________

Chapter 4. Problems?

Read the FAQ

   Some problems are more common than others. The most common ones are
   listed in the PHP FAQ, part of this manual.
     __________________________________________________________________

Other problems

   If you are still stuck, someone on the PHP installation mailing list
   may be able to help you. You should check out the archive first, in
   case someone already answered someone else who had the same problem as
   you. The archives are available from the support page on
   http://www.php.net/support.php. To subscribe to the PHP installation
   mailing list, send an empty mail to
   php-install-subscribe@lists.php.net. The mailing list address is
   php-install@lists.php.net.

   If you want to get help on the mailing list, please try to be precise
   and give the necessary details about your environment (which operating
   system, what PHP version, what web server, if you are running PHP as
   CGI or a server module, safe mode, etc...), and preferably enough code
   to make others able to reproduce and test your problem.
     __________________________________________________________________

Bug reports

   If you think you have found a bug in PHP, please report it. The PHP
   developers probably don't know about it, and unless you report it,
   chances are it won't be fixed. You can report bugs using the
   bug-tracking system at http://bugs.php.net/. Please do not send bug
   reports in mailing list or personal letters. The bug system is also
   suitable to submit feature requests.

   Read the How to report a bug document before submitting any bug
   reports!
     __________________________________________________________________

Chapter 5. Runtime Configuration

The configuration file

   The configuration file (called php3.ini in PHP 3, and simply php.ini as
   of PHP 4) is read when PHP starts up. For the server module versions of
   PHP, this happens only once when the web server is started. For the CGI
   and CLI version, it happens on every invocation.

   php.ini is searched in these locations (in order):

     * SAPI module specific location (PHPIniDir directive in Apache 2, -c
       command line option in CGI and CLI, php_ini parameter in NSAPI,
       PHP_INI_PATH environment variable in THTTPD)
     * The PHPRC environment variable. Before PHP 5.2.0 this was checked
       after the registry key mentioned below.
     * As of PHP 5.2.0, the following registry locations are searched in
       order: HKEY_LOCAL_MACHINE\SOFTWARE\PHP\x.y.z\IniFilePath,
       HKEY_LOCAL_MACHINE\SOFTWARE\PHP\x.y\IniFilePath and
       HKEY_LOCAL_MACHINE\SOFTWARE\PHP\x\IniFilePath, where x, y and z
       mean the PHP major, minor and release versions.
     * HKEY_LOCAL_MACHINE\SOFTWARE\PHP\IniFilePath (Windows Registry
       location)
     * Current working directory (except CLI)
     * The web server's directory (for SAPI modules), or directory of PHP
       (otherwise in Windows)
     * Windows directory (C:\windows or C:\winnt) (for Windows), or
       --with-config-file-path compile time option

   If php-SAPI.ini exists (where SAPI is used SAPI, so the filename is
   e.g. php-cli.ini or php-apache.ini), it's used instead of php.ini. SAPI
   name can be determined by php_sapi_name().

     Note: The Apache web server changes the directory to root at startup
     causing PHP to attempt to read php.ini from the root filesystem if
     it exists.

   The php.ini directives handled by extensions are documented
   respectively on the pages of the extensions themselves. The list of the
   core directives is available in the appendix. Probably not all PHP
   directives are documented in the manual though. For a complete list of
   directives available in your PHP version, please read your well
   commented php.ini file. Alternatively, you may find the latest
   php.ini from SVN helpful too.

   Example 5-1. php.ini example
; any text on a line after an unquoted semicolon (;) is ignored
[php] ; section markers (text within square brackets) are also ignored
; Boolean values can be set to either:
;    true, on, yes
; or false, off, no, none
html_errors = off
track_errors = yes

; you can enclose strings in double-quotes
include_path = ".:/usr/local/lib/php"

; backslashes are treated the same as any other character
include_path = ".;c:\php\lib"

   Since PHP 5.1.0, it is possible to refer to existing .ini variables
   from within .ini files. Example: open_basedir = ${open_basedir}
   ":/new/dir".
     __________________________________________________________________

How to change configuration settings

Running PHP as an Apache module

   When using PHP as an Apache module, you can also change the
   configuration settings using directives in Apache configuration files
   (e.g. httpd.conf) and .htaccess files. You will need "AllowOverride
   Options" or "AllowOverride All" privileges to do so.

   With PHP 4 and PHP 5, there are several Apache directives that allow
   you to change the PHP configuration from within the Apache
   configuration files. For a listing of which directives are PHP_INI_ALL,
   PHP_INI_PERDIR, or PHP_INI_SYSTEM, have a look at the List of php.ini
   directives appendix.

     Note: With PHP 3, there are Apache directives that correspond to
     each configuration setting in the php3.ini name, except the name is
     prefixed by "php3_".

   php_value name value
          Sets the value of the specified directive. Can be used only with
          PHP_INI_ALL and PHP_INI_PERDIR type directives. To clear a
          previously set value use none as the value.

     Note: Don't use php_value to set boolean values. php_flag (see
     below) should be used instead.

   php_flag name on|off
          Used to set a boolean configuration directive. Can be used only
          with PHP_INI_ALL and PHP_INI_PERDIR type directives.

   php_admin_value name value
          Sets the value of the specified directive. This can not be used
          in .htaccess files. Any directive type set with php_admin_value
          can not be overridden by .htaccess or virtualhost directives. To
          clear a previously set value use none as the value.

   php_admin_flag name on|off
          Used to set a boolean configuration directive. This can not be
          used in .htaccess files. Any directive type set with
          php_admin_flag can not be overridden by .htaccess or virtualhost
          directives.

   Example 5-2. Apache configuration example
<IfModule mod_php5.c>
  php_value include_path ".:/usr/local/lib/php"
  php_admin_flag engine on
</IfModule>

   Caution

   PHP constants do not exist outside of PHP. For example, in httpd.conf
   you can not use PHP constants such as E_ALL or E_NOTICE to set the
   error_reporting directive as they will have no meaning and will
   evaluate to 0. Use the associated bitmask values instead. These
   constants can be used in php.ini
     __________________________________________________________________

Changing PHP configuration via the Windows registry

   When running PHP on Windows, the configuration values can be modified
   on a per-directory basis using the Windows registry. The configuration
   values are stored in the registry key HKLM\SOFTWARE\PHP\Per Directory
   Values, in the sub-keys corresponding to the path names. For example,
   configuration values for the directory c:\inetpub\wwwroot would be
   stored in the key HKLM\SOFTWARE\PHP\Per Directory
   Values\c\inetpub\wwwroot. The settings for the directory would be
   active for any script running from this directory or any subdirectory
   of it. The values under the key should have the name of the PHP
   configuration directive and the string value. PHP constants in the
   values are not parsed. However, only configuration values changeable in
   PHP_INI_USER can be set this way, PHP_INI_PERDIR values can not.
     __________________________________________________________________

Other interfaces to PHP

   Regardless of how you run PHP, you can change certain values at runtime
   of your scripts through ini_set(). See the documentation on the
   ini_set() page for more information.

   If you are interested in a complete list of configuration settings on
   your system with their current values, you can execute the phpinfo()
   function, and review the resulting page. You can also access the values
   of individual configuration directives at runtime using ini_get() or
   get_cfg_var().
     __________________________________________________________________

Chapter 6. Installation FAQ

   This section holds common questions about the way to install PHP. PHP
   is available for almost any OS (except maybe for MacOS before OSX), and
   almost any web server.

   To install PHP, follow the instructions in Installing PHP.

   1. Why shouldn't I use Apache2 with a threaded MPM in a production
          environment?

   2. Unix/Windows: Where should my php.ini file be located?
   3. Unix: I installed PHP, but every time I load a document, I get the
          message 'Document Contains No Data'! What's going on here?

   4. Unix: I installed PHP using RPMS, but Apache isn't processing the
          PHP pages! What's going on here?

   5. Unix: I installed PHP 3 using RPMS, but it doesn't compile with the
          database support I need! What's going on here?

   6. Unix: I patched Apache with the FrontPage extensions patch, and
          suddenly PHP stopped working. Is PHP incompatible with the
          Apache FrontPage extensions?

   7. Unix/Windows: I have installed PHP, but when I try to access a PHP
          script file via my browser, I get a blank screen.

   8. Unix/Windows: I have installed PHP, but when try to access a PHP
          script file via my browser, I get a server 500 error.

   9. Some operating systems: I have installed PHP without errors, but
          when I try to start apache I get undefined symbol errors:

[mybox:user /src/php5] root# apachectl configtest
 apachectl: /usr/local/apache/bin/httpd Undefined symbols:
  _compress
  _uncompress

   10. Windows: I have installed PHP, but when I to access a PHP script
          file via my browser, I get the error:

cgi error:
 The specified CGI application misbehaved by not
 returning a complete set of HTTP headers.
 The headers it did return are:

   11. Windows: I've followed all the instructions, but still can't get
          PHP and IIS to work together!

   12. When running PHP as CGI with IIS, OmniHTTPD or Xitami, I get
          the following error: Security Alert! PHP CGI cannot be accessed
          directly..

   13. How do I know if my php.ini is being found and read? It seems like
          it isn't as my changes aren't being implemented.

   14. How do I add my PHP directory to the PATH on Windows?
   15. How do I make the php.ini file available to PHP on windows?
   16. Is it possible to use Apache content negotiation (MultiViews
          option) with PHP?

   17. Is PHP limited to process GET and POST request methods only?

   1. Why shouldn't I use Apache2 with a threaded MPM in a production
   environment?

   PHP is glue. It is the glue used to build cool web applications by
   sticking dozens of 3rd-party libraries together and making it all
   appear as one coherent entity through an intuitive and easy to learn
   language interface. The flexibility and power of PHP relies on the
   stability and robustness of the underlying platform. It needs a working
   OS, a working web server and working 3rd-party libraries to glue
   together. When any of these stop working PHP needs ways to identify the
   problems and fix them quickly. When you make the underlying framework
   more complex by not having completely separate execution threads,
   completely separate memory segments and a strong sandbox for each
   request to play in, feet of clay are introduced into PHP's system.

   If you feel you have to use a threaded MPM, look at a FastCGI
   configuration where PHP is running in its own memory space.

   And finally, this warning against using a threaded MPM is not as strong
   for Windows systems because most libraries on that platform tend to be
   threadsafe.

   2. Unix/Windows: Where should my php.ini file be located?

   By default on Unix it should be in /usr/local/lib which is
   <install-path>/lib. Most people will want to change this at
   compile-time with the --with-config-file-path flag. You would, for
   example, set it with something like:
   --with-config-file-path=/etc

   And then you would copy php.ini-production from the distribution to
   /etc/php.ini and edit it to make any local changes you want.
   --with-config-file-scan-dir=PATH

   On Windows the default path for the php.ini file is the Windows
   directory. If you're using the Apache webserver, php.ini is first
   searched in the Apaches install directory, e.g. c:\program files\apache
   group\apache. This way you can have different php.ini files for
   different versions of Apache on the same machine.

   See also the chapter about the configuration file.

   3. Unix: I installed PHP, but every time I load a document, I get the
   message 'Document Contains No Data'! What's going on here?

   This probably means that PHP is having some sort of problem and is
   core-dumping. Look in your server error log to see if this is the case,
   and then try to reproduce the problem with a small test case. If you
   know how to use 'gdb', it is very helpful when you can provide a
   backtrace with your bug report to help the developers pinpoint the
   problem. If you are using PHP as an Apache module try something like:

     * Stop your httpd processes
     * gdb httpd
     * Stop your httpd processes
     * > run -X -f /path/to/httpd.conf
     * Then fetch the URL causing the problem with your browser
     * > run -X -f /path/to/httpd.conf
     * If you are getting a core dump, gdb should inform you of this now
     * type: bt
     * You should include your backtrace in your bug report. This should
       be submitted to http://bugs.php.net/

   If your script uses the regular expression functions (ereg() and
   friends), you should make sure that you compiled PHP and Apache with
   the same regular expression package. This should happen automatically
   with PHP and Apache 1.3.x

   4. Unix: I installed PHP using RPMS, but Apache isn't processing the
   PHP pages! What's going on here?

   Assuming you installed both Apache and PHP from RPM packages, you need
   to uncomment or add some or all of the following lines in your
   httpd.conf file:
# Extra Modules
AddModule mod_php.c
AddModule mod_php3.c
AddModule mod_perl.c

# Extra Modules
LoadModule php_module         modules/mod_php.so
LoadModule perl_module        modules/libperl.so

   And add:
AddType application/x-httpd-php3 .php3    # for PHP 3
AddType application/x-httpd-php .php      # for PHP 4

   ... to the global properties, or to the properties of the VirtualDomain
   you want to have PHP support added to.

   5. Unix: I installed PHP 3 using RPMS, but it doesn't compile with the
   database support I need! What's going on here?

   Due to the way PHP 3 built, it is not easy to build a complete flexible
   PHP RPM. This issue is addressed in PHP 4. For PHP 3, we currently
   suggest you use the mechanism described in the INSTALL.REDHAT file in
   the PHP distribution. If you insist on using an RPM version of PHP 3,
   read on...

   The RPM packagers are setting up the RPMS to install without database
   support to simplify installations and because RPMS use /usr/ instead of
   the standard /usr/local/ directory for files. You need to tell the RPM
   spec file which databases to support and the location of the top-level
   of your database server.

   This example will explain the process of adding support for the popular
   MySQL database server, using the mod installation for Apache.

   Of course all of this information can be adjusted for any database
   server that PHP supports. We will assume you installed MySQL and Apache
   completely with RPMS for this example a