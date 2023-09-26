Installing PHP
     __________________________________________________________________

   Table of Contents
   Preface
   1. General Installation Considerations
   2. Installation on Windows systems

        Manual Installation Steps
        ActiveScript
        Microsoft IIS
        Apache 1.3.x on Microsoft Windows
        Apache 2.0.x on Microsoft Windows
        Sun, iPlanet and Netscape servers on Microsoft Windows
        OmniHTTPd Server
        Sambar Server on Microsoft Windows
        Xitami on Microsoft Windows
        Installation of extensions on Windows

   3. Installation of PECL extensions

        Introduction to PECL Installations
        Downloading PECL extensions
        PECL for Windows users
        Compiling shared PECL extensions with the pecl command
        Compiling shared PECL extensions with phpize
        Compiling PECL extensions statically into PHP

   4. Problems?

        Read the FAQ
        Other problems
        Bug reports

   5. Runtime Configuration

        The configuration file
        How to change configuration settings

   6. Installation FAQ
     __________________________________________________________________

Preface

   These installation instructions were generated from the HTML version of
   the PHP Manual so formatting and linking have been altered. See the
   online and updated version at: http://php.net/install.windows
     __________________________________________________________________

Chapter 1. General Installation Considerations

   Before starting the installation, first you need to know what do you
   want to use PHP for. There are three main fields you can use PHP, as
   described in the What can PHP do? section:

     * Websites and web applications (server-side scripting)
     * Command line scripting
     * Desktop (GUI) applications

   For the first and most common form, you need three things: PHP itself,
   a web server and a web browser. You probably already have a web
   browser, and depending on your operating system setup, you may also
   have a web server (e.g. Apache on Linux and MacOS X; IIS on Windows).
   You may also rent webspace at a company. This way, you don't need to
   set up anything on your own, only write your PHP scripts, upload it to
   the server you rent, and see the results in your browser.

   In case of setting up the server and PHP on your own, you have two
   choices for the method of connecting PHP to the server. For many
   servers PHP has a direct module interface (also called SAPI). These
   servers include Apache, Microsoft Internet Information Server, Netscape
   and iPlanet servers. Many other servers have support for ISAPI, the
   Microsoft module interface (OmniHTTPd for example). If PHP has no
   module support for your web server, you can always use it as a CGI or
   FastCGI processor. This means you set up your server to use the CGI
   executable of PHP to process all PHP file requests on the server.

   If you are also interested to use PHP for command line scripting (e.g.
   write scripts autogenerating some images for you offline, or processing
   text files depending on some arguments you pass to them), you always
   need the command line executable. For more information, read the
   section about writing command line PHP applications. In this case, you
   need no server and no browser.

   With PHP you can also write desktop GUI applications using the PHP-GTK
   extension. This is a completely different approach than writing web
   pages, as you do not output any HTML, but manage Windows and objects
   within them. For more information about PHP-GTK, please visit the site
   dedicated to this extension. PHP-GTK is not included in the official
   PHP distribution.

   From now on, this section deals with setting up PHP for web servers on
   Unix and Windows with server module interfaces and CGI executables. You
   will also find information on the command line executable in the
   following sections.

   PHP source code and binary distributions for Windows can be found at
   http://www.php.net/downloads.php. We recommend you to choose a mirror
   nearest to you for downloading the distributions.
     __________________________________________________________________

Chapter 2. Installation on Windows systems

   This section applies to Windows 98/Me and Windows NT/2000/XP/2003. PHP
   will not work on 16 bit platforms such as Windows 3.1 and sometimes we
   refer to the supported Windows platforms as Win32. Windows 95 is no
   longer supported as of PHP 4.3.0.

   If you have Microsoft Visual Studio, you can also build PHP from the
   original source code.

   Once you have PHP installed on your Windows system, you may also want
   to load various extensions for added functionality.

   Warning

   There are several all-in-one installers over the Internet, but none of
   those are endorsed by PHP.net, as we believe that the manual
   installation is the best choice to have your system secure and
   optimised.
     __________________________________________________________________

Manual Installation Steps

   This install guide will help you manually install and configure PHP
   with a web server on Microsoft Windows. To get started you'll need to
   download the zip binary distribution from the downloads page at
   http://www.php.net/downloads.php.

   Although there are many all-in-one installation kits, we recommend you
   take the time to setup PHP yourself as this will provide you with a
   better understanding of the system, and enables you to install PHP
   extensions easily when needed.

     Upgrading from a previous PHP version: Previous editions of the
     manual suggest moving various ini and DLL files into your SYSTEM
     (i.e. C:\WINDOWS) folder and while this simplifies the installation
     procedure it makes upgrading difficult. We advise you remove all of
     these files (like php.ini and PHP related DLLs from the Windows
     SYSTEM folder) before moving on with a new PHP installation. Be sure
     to backup these files as you might break the entire system. The old
     php.ini might be useful in setting up the new PHP as well. And as
     you'll soon learn, the preferred method for installing PHP is to
     keep all PHP related files in one directory and have this directory
     available to your systems PATH.

     MDAC requirements: If you use Microsoft Windows 98/NT4 download the
     latest version of the Microsoft Data Access Components (MDAC) for
     your platform. MDAC is available at http://msdn.microsoft.com/data/.
     This requirement exists because ODBC is built into the distributed
     Windows binaries.

   The following steps should be completed on all installations before any
   server specific instructions are performed:

   Extract the distribution file into a directory of your choice. If you
   are installing PHP 4, extract to C:\, as the zip file expands to a
   foldername like php-4.3.7-Win32. If you are installing PHP 5, extract
   to C:\php as the zip file doesn't expand as in PHP 4. You may choose a
   different location but do not have spaces in the path (like C:\Program
   Files\PHP) as some web servers will crash if you do.

   The directory structure extracted from the zip is different for PHP
   versions 4 and 5 and look like as follows:

   Example 2-2. PHP 5 package structure
c:\php
   |
   +--dev
   |  |
   |  |-php5ts.lib
   |
   +--ext                 -- extension DLLs for PHP
   |  |
   |  |-php_bz2.dll
   |  |
   |  |-php_cpdf.dll
   |  |
   |  |-..
   |
   +--extras
   |  |
   |  +--mibs             -- support files for SNMP
   |  |
   |  +--openssl          -- support files for Openssl
   |  |
   |  +--pdf-related      -- support files for PDF
   |  |
   |  |-mime.magic
   |
   +--pear                -- initial copy of PEAR
   |
   |
   |-go-pear.bat          -- PEAR setup script
   |
   |-fdftk.dll
   |
   |-..
   |
   |-php-cgi.exe          -- CGI executable
   |
   |-php-win.exe          -- executes scripts without an opened command prompt
   |
   |-php.exe              -- CLI executable - ONLY for command line scripting
   |
   |-..
   |
   |-php.ini-development  -- development php.ini settings
   |
   |-php.ini-production   -- recommended php.ini settings for production
   |
   |-php5activescript.dll
   |
   |-php5apache2_4.dll
   |
   |-..
   |
   |-php5ts.dll           -- core PHP DLL
   |
   |-...

   Notice the differences and similarities. Both PHP 4 and PHP 5 have a
   CGI executable, a CLI executable, and server modules, but they are
   located in different folders and/or have different names. While PHP 4
   packages have the server modules in the sapi folder, PHP 5
   distributions have no such directory and instead they're in the PHP
   folder root. The supporting DLLs for the PHP 5 extensions are also not
   in a separate directory.

     Note: In PHP 4, you should move all files located in the dll and
     sapi folders to the main folder (e.g. C:\php).

   Here is a list of server modules shipped with PHP 5:

     * php5apache2_4.dll - Apache 2.4.x module.

   Server modules provide significantly better performance and additional
   functionality compared to the CGI binary. The FastCGI is significantly
   more stable and can be faster than the ISAPI module with IIS.
   The CLI version is designed to let you use PHP for command line 
   scripting. More information about CLI is available in the chapter 
   about using PHP from the command line.

   Warning

   The SAPI modules have been significantly improved as of the 4.1
   release, however, in older systems you may encounter server errors or
   other server modules failing, such as ASP.

   The CGI and CLI binaries, and the web server modules all require the
   php5ts.dll file to be available to them. You have to make
   sure that this file can be found by your PHP installation. The search
   order for this DLL is as follows:

     * The same directory from where php.exe is called, or in case you use
       a SAPI module, the web server's directory (e.g. C:\Program
       Files\Apache Group\Apache2\bin).
     * Any directory in your Windows PATH environment variable.

   To make php5ts.dll available you have three options: copy
   the file to the Windows system directory, copy the file to the web
   server's directory, or add your PHP directory, C:\php to the PATH. For
   better maintenance, we advise you to follow the last option, add C:\php
   to the PATH, because it will be simpler to upgrade PHP in the future.
   Read more about how to add your PHP directory to PATH in the
   corresponding FAQ entry (and then don't forget to restart the computer
   - logoff isn't enough).

   The next step is to set up a valid configuration file for PHP, php.ini.
   There are two ini files distributed in the zip file, php.ini-development
   and php.ini-production. We advise you to use php.ini-production, 
   because we optimized the default settings in this file for performance, 
   and security. Read this well documented file carefully because it has
   changes from php.ini-production that will drastically affect your setup.
   Some examples are display_errors being off and magic_quotes_gpc being off.
   In addition to reading these, study the ini settings and set every
   element manually yourself. If you would like to achieve the best
   security, then this is the way for you, although PHP works fine with
   these default ini files. Copy your chosen ini-file to a directory that
   PHP is able to find and rename it to php.ini. PHP searches for php.ini
   in the locations described in the Section called The configuration file
   in Chapter 5 section.

   If you are running Apache 2, the simpler option is to use the PHPIniDir
   directive (read the installation on Apache 2 page), otherwise your best
   option is to set the PHPRC environment variable. This process is
   explained in the following FAQ entry.

     Note: If you're using NTFS on Windows NT, 2000, XP or 2003, make
     sure that the user running the web server has read permissions to
     your php.ini (e.g. make it readable by Everyone).

   The following steps are optional:

     * Edit your new php.ini file. If you plan to use OmniHTTPd, do not
       follow the next step. Set the doc_root to point to your web servers
       document_root. For example:

doc_root = c:\inetpub\wwwroot // for IIS

doc_root = c:\apache\htdocs // for Apache

     * Choose the extensions you would like to load when PHP starts. See
       the section about Windows extensions, about how to set up one, and
       what is already built in. Note that on a new installation it is
       advisable to first get PHP working and tested without any
       extensions before enabling them in php.ini.

   PHP is now setup on your system. The next step is to choose a web
   server, and enable it to run PHP. Choose a web server from the table of
   contents.
     __________________________________________________________________

ActiveScript

   This section contains notes specific to the ActiveScript installation.

   ActiveScript is a Windows only SAPI that enables you to use PHP script
   in any ActiveScript compliant host, like Windows Script Host,
   ASP/ASP.NET, Windows Script Components or Microsoft Scriptlet control.

   As of PHP 5.0.1, ActiveScript has been moved to the PECL repository.
   The DLL for this PECL extension may be downloaded from either the PHP
   Downloads page or from http://pecl4win.php.net/

     Note: You should read the manual installation steps first!

   After installing PHP, you should download the ActiveScript DLL
   (php5activescript.dll) and place it in the main PHP folder (e.g.
   C:\php).

   After having all the files needed, you must register the DLL on your
   system. To achieve this, open a Command Prompt window (located in the
   Start Menu). Then go to your PHP directory by typing something like cd
   C:\php. To register the DLL just type regsvr32 php5activescript.dll.

   To test if ActiveScript is working, create a new file, named test.wsf
   (the extension is very important) and type:
<job id="test">

 <script language="PHPScript">
  $WScript->Echo("Hello World!");
 </script>

</job>

   Save and double-click on the file. If you receive a little window
   saying "Hello World!" you're done.

     Note: In PHP 4, the engine was named 'ActivePHP', so if you are
     using PHP 4, you should replace 'PHPScript' with 'ActivePHP' in the
     above example.

     Note: ActiveScript doesn't use the default php.ini file. Instead, it
     will look only in the same directory as the .exe that caused it to
     load. You should create php-activescript.ini and place it in that
     folder, if you wish to load extensions, etc.
     __________________________________________________________________

Microsoft IIS

   This section contains notes and hints specific to IIS (Microsoft
   Internet Information Server).

   Warning

   By using the CGI setup, your server is open to several possible
   attacks. Please read our CGI security section to learn how to defend
   yourself from those attacks.
     __________________________________________________________________

General considerations for all installations of PHP with IIS

     * First, read the Manual Installation Instructions. Do not skip this
       step as it provides crucial information for installing PHP on
       Windows.
     * CGI users must set the cgi.force_redirect PHP directive to 0 inside
       php.ini. Read the faq on cgi.force_redirect for important details.
       Also, CGI users may want to set the cgi.redirect_status_env
       directive. When using directives, be sure these directives aren't
       commented out inside php.ini.
     * The PHP 4 CGI is named php.exe while in PHP 5 it's php-cgi.exe. In
       PHP 5, php.exe is the CLI, and not the CGI.
     * Modify the Windows PATH environment variable to include the PHP
       directory. This way the PHP DLL files and PHP executables can all
       remain in the PHP directory without cluttering up the Windows
       system directory. For more details, see the FAQ on Setting the
       PATH.
     * The IIS user (usually IUSR_MACHINENAME) needs permission to read
       various files and directories, such as php.ini, docroot, and the
       session tmp directory.
     * Be sure the extension_dir and doc_root PHP directives are
       appropriately set in php.ini. These directives depend on the system
       that PHP is being installed on. In PHP 4, the extension_dir is
       extensions while with PHP 5 it's ext. So, an example PHP 5
       extensions_dir value is "c:\php\ext" and an example IIS doc_root
       value is "c:\Inetpub\wwwroot".
     * PHP extension DLL files, such as php_mysql.dll and php_curl.dll,
       are found in the zip package of the PHP download. In PHP 5, many
       extensions are part of PECL and can be downloaded in the
       "Collection of PECL modules" package. Files such as php_zip.dll and
       php_ssh2.dll. Download PHP files here.
     * When defining the executable, the 'check that file exists' box may
       also be checked. For a small performance penalty, the IIS
       will check that the script file exists and sort out authentication
       before firing up PHP. This means that the web server will provide
       sensible 404 style error messages instead of CGI errors complaining
       that PHP did not output any data.
     __________________________________________________________________

Windows NT/200x/XP and IIS 4 or newer

   PHP may be installed as a CGI binary, or with the ISAPI module. In
   either case, you need to start the Microsoft Management Console (may
   appear as 'Internet Services Manager', either in your Windows NT 4.0
   Option Pack branch or the Control Panel=>Administrative Tools under
   Windows 2000/XP). Then right click on your Web server node (this will
   most probably appear as 'Default Web Server'), and select 'Properties'.

   If you want to use the CGI binary, do the following:

     * Under 'Home Directory', 'Virtual Directory', or 'Directory', do the
       following:
     * Change the Execute Permissions to 'Scripts only'
     * Click on the 'Configuration' button, and choose the Application
       Mappings tab. Click Add and set the Executable path to the
       appropriate CGI file. An example PHP 5 value is: C:\php\php-cgi.exe
       Supply .php as the extension. Leave 'Method exclusions' blank, and
       check the 'Script engine' checkbox. Now, click OK a few times.
     * Set up the appropriate security. (This is done in Internet Service
       Manager), and if your NT Server uses NTFS file system, add execute
       rights for I_USR_ to the directory that contains php.exe /
       php-cgi.exe.

   To use the ISAPI module, do the following:

     * If you don't want to perform HTTP Authentication using PHP, you can
       (and should) skip this step. Under ISAPI Filters, add a new ISAPI
       filter. Use PHP as the filter name, and supply a path to the
       php5isapi.dll.
     * Under 'Home Directory', 'Virtual Directory', or 'Directory', do the
       following:
     * Change the Execute Permissions to 'Scripts only'
     * Click on the 'Configuration' button, and choose the Application
       Mappings tab. Click Add and set the Executable path to the
       appropriate ISAPI DLL. An example PHP 5 value is:
       C:\php\php5isapi.dll Supply .php as the extension. Leave 'Method
       exclusions' blank, and check the 'Script engine' checkbox. Now,
       click OK a few times.
     * Stop IIS completely (NET STOP iisadmin)
     * Start IIS again (NET START w3svc)

   With IIS 6 (2003 Server), open up the IIS Manager, go to Web Service
   Extensions, choose "Add a new Web service extension", enter in a name
   such as PHP, choose the Add button and for the value browse to either
   the ISAPI file (php5isapi.dll) or CGI (php.exe or
   php-cgi.exe) then check "Set extension status to Allowed" and click OK.

   In order to use index.php as a default content page, do the following:
   From within the Documents tab, choose Add. Type in index.php and click
   OK. Adjust the order by choosing Move Up or Move Down. This is similar
   to setting DirectoryIndex with Apache.

   The steps above must be repeated for each extension that is to be
   associated with PHP scripts. .php is the most common although .php3 may
   be required for legacy applications.

   If you experience 100% CPU usage after some time, turn off the IIS
   setting Cache ISAPI Application.
     __________________________________________________________________

Windows and IIS

See http://www.php.net/install.windows
     __________________________________________________________________

Apache 1.3.x on Microsoft Windows

   This section contains notes and hints specific to Apache 1.3.x installs
   of PHP on Microsoft Windows systems. There are also instructions and
   notes for Apache 2 on a separate page.

     Note: Please read the manual installation steps first!

   There are two ways to set up PHP to work with Apache 1.3.x on Windows.
   One is to use the CGI binary (php.exe for PHP 4 and php-cgi.exe for PHP
   5), the other is to use the Apache Module DLL. In either case you need
   to edit your httpd.conf to configure Apache to work with PHP, and then
   restart the server.

   It is worth noting here that now the SAPI module has been made more
   stable under Windows, we recommend it's use above the CGI binary, since
   it is more transparent and secure.

   Although there can be a few variations of configuring PHP under Apache,
   these are simple enough to be used by the newcomer. Please consult the
   Apache Documentation for further configuration directives.

   After changing the configuration file, remember to restart the server,
   for example, NET STOP APACHE followed by NET START APACHE, if you run
   Apache as a Windows Service, or use your regular shortcuts.

     Note: Remember that when adding path values in the Apache
     configuration files on Windows, all backslashes such as
     c:\directory\file.ext must be converted to forward slashes, as
     c:/directory/file.ext. A trailing slash may also be necessary for
     directories.
     __________________________________________________________________

Installing as an Apache module

   You should add the following lines to your Apache httpd.conf file:

   Example 2-3. PHP as an Apache 1.3.x module

   This assumes PHP is installed to c:\php. Adjust the path if this is not
   the case.

   For PHP 5:
# Add to the end of the LoadModule section
LoadModule php5_module "C:/php/php5apache.dll"

# Add to the end of the AddModule section
AddModule mod_php5.c

   For both:
# Add this line inside the <IfModule mod_mime.c> conditional brace
AddType application/x-httpd-php .php

# For syntax highlighted .phps files, also add
AddType application/x-httpd-php-source .phps
     __________________________________________________________________

Installing as a CGI binary

   If you unzipped the PHP package to C:\php\ as described in the Manual
   Installation Steps section, you need to insert these lines to your
   Apache configuration file to set up the CGI binary:

   Example 2-4. PHP and Apache 1.3.x as CGI
ScriptAlias /php/ "c:/php/"
AddType application/x-httpd-php .php

# For PHP 4
Action application/x-httpd-php "/php/php.exe"

# For PHP 5
Action application/x-httpd-php "/php/php-cgi.exe"

# specify the directory where php.ini is
SetEnv PHPRC C:/php

   Note that the second line in the list above can be found in the actual
   versions of httpd.conf, but it is commented out. Remember also to
   substitute the c:/php/ for your actual path to PHP.

   Warning

   By using the CGI setup, your server is open to several possible
   attacks. Please read our CGI security section to learn how to defend
   yourself from those attacks.

   If you would like to present PHP source files syntax highlighted, there
   is no such convenient option as with the module version of PHP. If you
   chose to configure Apache to use PHP as a CGI binary, you will need to
   use the highlight_file() function. To do this simply create a PHP
   script file and add this code: <?php
   highlight_file('some_php_script.php'); ?>.
     __________________________________________________________________

Apache 2.0.x on Microsoft Windows

   This section contains notes and hints specific to Apache 2.0.x installs
   of PHP on Microsoft Windows systems. We also have instructions and
   notes for Apache 1.3.x users on a separate page.

     Note: You should read the manual installation steps first!

     Apache 2.2.x Support: Users of Apache 2.2.x may use the
     documentation below except the appropriate DLL file is named
     php5apache2_2.dll and it only exists as of PHP 5.2.0. See also
     http://snaps.php.net/

   Warning

   We do not recommend using a threaded MPM in production with Apache2.
   Use the prefork MPM instead, or use Apache1. For information on why,
   read the related FAQ entry on using Apache2 with a threaded MPM

   You are highly encouraged to take a look at the Apache Documentation to
   get a basic understanding of the Apache 2.0.x Server. Also consider to
   read the Windows specific notes for Apache 2.0.x before reading on
   here.

     PHP and Apache 2.0.x compatibility notes: The following versions of
     PHP are known to work with the most recent version of Apache 2.0.x:

     * PHP 4.3.0 or later available at http://www.php.net/downloads.php.
     * the latest stable development version. Get the source code
       http://snaps.php.net/php5-latest.tar.gz or download binaries for
       Windows http://snaps.php.net/win32/php5-win32-latest.zip.
     * a prerelease version downloadable from http://qa.php.net/.
     * you have always the option to obtain PHP through SVN.

     These versions of PHP are compatible to Apache 2.0.40 and later.

     Apache 2.0 SAPI-support started with PHP 4.2.0. PHP 4.2.3 works with
     Apache 2.0.39, don't use any other version of Apache with PHP 4.2.3.
     However, the recommended setup is to use PHP 4.3.0 or later with the
     most recent version of Apache2.

     All mentioned versions of PHP will work still with Apache 1.3.x.

   Warning

   Apache 2.0.x is designed to run on Windows NT 4.0, Windows 2000 or
   Windows XP. At this time, support for Windows 9x is incomplete. Apache
   2.0.x is not expected to work on those platforms at this time.

   Download the most recent version of Apache 2.0.x and a fitting PHP
   version. Follow the Manual Installation Steps and come back to go on
   with the integration of PHP and Apache.

   There are two ways to set up PHP to work with Apache 2.0.x on Windows.
   One is to use the CGI binary the other is to use the Apache module DLL.
   In either case you need to edit your httpd.conf to configure Apache to
   work with PHP and then restart the server.

     Note: Remember that when adding path values in the Apache
     configuration files on Windows, all backslashes such as
     c:\directory\file.ext must be converted to forward slashes, as
     c:/directory/file.ext. A trailing slash may also be necessary for
     directories.
     __________________________________________________________________

Installing as a CGI binary

   You need to insert these three lines to your Apache httpd.conf
   configuration file to set up the CGI binary:

   Example 2-5. PHP and Apache 2.0 as CGI
ScriptAlias /php/ "c:/php/"
AddType application/x-httpd-php .php

# For PHP 4
Action application/x-httpd-php "/php/php.exe"

# For PHP 5
Action application/x-httpd-php "/php/php-cgi.exe"

   Warning

   By using the CGI setup, your server is open to several possible
   attacks. Please read our CGI security section to learn how to defend
   yourself from those attacks.
     __________________________________________________________________

Installing as an Apache module

   You need to insert these two lines to your Apache httpd.conf
   configuration file to set up the PHP module for Apache 2.0:

   Example 2-6. PHP and Apache 2.0 as Module

# For PHP 5 do something like this:
LoadModule php5_module "c:/php/php5apache2.dll"
AddType application/x-httpd-php .php

# configure the path to php.ini
PHPIniDir "C:/php"

     Note: Remember to substitute your actual path to PHP for the c:/php/
     in the above examples. Take care to use either 
     php5apache2.dll in your LoadModule directive and not php5apache.dll 
     as the latter ones are designed to run with Apache 1.3.x.

     Note: If you want to use content negotiation, read related FAQ.

   Warning

   Don't mix up your installation with DLL files from different PHP
   versions. You have the only choice to use the DLL's and extensions that
   ship with your downloaded PHP version.
     __________________________________________________________________

Sun, iPlanet and Netscape servers on Microsoft Windows

   This section contains notes and hints specific to Sun Java System Web
   Server, Sun ONE Web Server, iPlanet and Netscape server installs of PHP
   on Windows.

   From PHP 4.3.3 on you can use PHP scripts with the NSAPI module to
   generate custom directory listings and error pages. Additional
   functions for Apache compatibility are also available. For support in
   current web servers read the note about subrequests.
     __________________________________________________________________

CGI setup on Sun, iPlanet and Netscape servers

   To install PHP as a CGI handler, do the following:

     * Copy php5ts.dll to your systemroot (the directory where you
       installed Windows)
     * Make a file association from the command line. Type the following
       two lines:

assoc .php=PHPScript
ftype PHPScript=c:\php\php.exe %1 %*

     * In the Netscape Enterprise Administration Server create a dummy
       shellcgi directory and remove it just after (this step creates 5
       important lines in obj.conf and allow the web server to handle
       shellcgi scripts).
     * In the Netscape Enterprise Administration Server create a new mime
       type (Category: type, Content-Type: magnus-internal/shellcgi, File
       Suffix:php).
     * Do it for each web server instance you want PHP to run

   More details about setting up PHP as a CGI executable can be found
   here: http://benoit.noss.free.fr/php/install-php.html
     __________________________________________________________________

NSAPI setup on Sun, iPlanet and Netscape servers

   To install PHP with NSAPI, do the following:

     * Copy php5ts.dll to your systemroot (the directory where you
       installed Windows)
     * Make a file association from the command line. Type the following
       two lines:

assoc .php=PHPScript
ftype PHPScript=c:\php\php.exe %1 %*

     * In the Netscape Enterprise Administration Server create a new mime
       type (Category: type, Content-Type: magnus-internal/x-httpd-php,
       File Suffix: php).
     * Edit magnus.conf (for servers >= 6) or obj.conf (for servers < 6)
       and add the following: You should place the lines after mime types
       init.

Init fn="load-modules" funcs="php5_init,php5_execute,php5_auth_trans" shlib="c:/
php/sapi/php5nsapi.dll"
Init fn="php5_init" LateInit="yes" errorString="Failed to initialise PHP!" [php_
ini="c:/path/to/php.ini"]

       The php_ini parameter is optional but with it you
       can place your php.ini in your web server configuration directory.
     * Configure the default object in obj.conf (for virtual server
       classes [Sun Web Server 6.0+] in their vserver.obj.conf): In the
       <Object name="default"> section, place this line necessarily after
       all 'ObjectType' and before all 'AddLog' lines:

Service fn="php5_execute" type="magnus-internal/x-httpd-php" [inikey=value inike
y=value ...]

       As additional parameters you can add some special
       php.ini-values, for example you can set a
       docroot="/path/to/docroot" specific to the context php5_execute is
       called. For boolean ini-keys please use 0/1 as value, not
       "On","Off",... (this will not work correctly), e.g.
       zlib.output_compression=1 instead of zlib.output_compression="On"
     * This is only needed if you want to configure a directory that only
       consists of PHP scripts (same like a cgi-bin directory):

<Object name="x-httpd-php">
ObjectType fn="force-type" type="magnus-internal/x-httpd-php"
Service fn=php5_execute [inikey=value inikey=value ...]
</Object>

       After that you can configure a directory in the Administration
       server and assign it the style x-httpd-php. All files in it will
       get executed as PHP. This is nice to hide PHP usage by renaming
       files to .html.
     * Restart your web service and apply changes
     * Do it for each web server instance you want PHP to run

     Note: More details about setting up PHP as an NSAPI filter can be
     found here: http://benoit.noss.free.fr/php/install-php4.html

     Note: The stacksize that PHP uses depends on the configuration of
     the web server. If you get crashes with very large PHP scripts, it
     is recommended to raise it with the Admin Server (in the section
     "MAGNUS EDITOR").
     __________________________________________________________________

CGI environment and recommended modifications in php.ini

   Important when writing PHP scripts is the fact that Sun JSWS/Sun ONE
   WS/iPlanet/Netscape is a multithreaded web server. Because of that all
   requests are running in the same process space (the space of the web
   server itself) and this space has only one environment. If you want to
   get CGI variables like PATH_INFO, HTTP_HOST etc. it is not the correct
   way to try this in the old PHP 3.x way with getenv() or a similar way
   (register globals to environment, $_ENV). You would only get the
   environment of the running web server without any valid CGI variables!

     Note: Why are there (invalid) CGI variables in the environment?

     Answer: This is because you started the web server process from the
     admin server which runs the startup script of the web server, you
     wanted to start, as a CGI script (a CGI script inside of the admin
     server!). This is why the environment of the started web server has
     some CGI environment variables in it. You can test this by starting
     the web server not from the administration server. Use the command
     line as root user and start it manually - you will see there are no
     CGI-like environment variables.
     __________________________________________________________________

Special use for error pages or self-made directory listings (PHP >= 4.3.3)

   You can use PHP to generate the error pages for "404 Not Found" or
   similar. Add the following line to the object in obj.conf for every
   error page you want to overwrite:
Error fn="php5_execute" code=XXX script="/path/to/script.php" [inikey=value inik
ey=value...]

   where XXX is the HTTP error code. Please delete any other Error
   directives which could interfere with yours. If you want to place a
   page for all errors that could exist, leave the code parameter out.
   Your script can get the HTTP status code with $_SERVER['ERROR_TYPE'].

   Another po