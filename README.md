zwave-remote
============
Author: Praveen M Nair
Mail: praveen.nair@icloud.com

Open source z-wave library for home automation 

About my setup
--------------
I have the following
1. aeon z-stick
2. raspberry pi: hosts the backend to control my z-wave devices and a simple web interface over lighttpd to interact with the backend.

About the source code
---------------------
There are 3 parts here
1. zwave_lib: A simple zwave protocol library with its own test code.
2. hzremote: The remote daemon that uses the zwave protocol library and provides an XML-RPC interface to control your zwave module
   I have an aeon z-stick that acts as my gateway to all my z-wave enabled devices
3. www: A simple web interface written in PHP and JS. This web interface talks to my backend using the XML-RPC interface
   I host it on a lighttpd server

Lighttpd installation notes
---------------------------
1. Create web location that the server will use in /var
sudo mkdir /var/www
sudo groupadd www-data
sudo adduser www-data www-data
sudo usermod -a -G www-data www-data
sudo chown -R www-data:www-data /var/www

2. Install lighttpd
sudo apt-get update
sudo apt-get install lighttpd

3. Install php5
sudo apt-get install php5-cgi php5-mysql php5-curl php5-gd php5-idn php-pear php5-imagick php5-imap php5-mcrypt php5-memcache php5-mhash php5-ming php5-pspell php5-recode php5-snmp php5-sqlite php5-tidy php5-xmlrpc php5-xsl php5-fpm php5-cgi php5-cli php5-common

4. Setup lighttpd
Create following symlinks from /etc/lighttpd/conf-available to /etc/lighttpd/conf-enabled
10-accesslog.conf	- OPTIONAL  
10-cgi-php.conf  
10-fastcgi.conf  
15-fastcgi-php.conf  
15-fatcgi-php.conf
10-proxy.conf  		- For routing requests to XMLRPC server

5. Add the following in 10-proxy.conf
#### proxy module for XMLRPC
proxy.debug                   = 1
$HTTP["url"] =~ "^/RPC2" {
       proxy.server = ( "" =>
                       ( ( 
                           "host" => "127.0.0.1",
                           "port" => 8080
                         ) )
                     )
}

Setup notes
-----------
1. Install raspbian wheezy on the raspberry pi
   You can follow the detailed notes from http://elinux.org/RPi_Easy_SD_Card_Setup
   
2. Install necessary tools and libraries
   sudo apt-get install build-essential make git subversion libsqlite3-dev libxmlrpc-c3-dev

3. Fetch the latest source code
   cd /opt
   git clone http://github.com/pmnair/zwave-remote

4. Compile the library
   cd /opt/zwave-remote/
   mkdir bin
   mkdir lib
   cd zwave_lib
   make clean all
   cd ../hzremote
   make clean all

5. Deploy the web interface
   sudo cp -a /opt/zwave-remote/www/* /var/www/
   sudo service lighttpd restart

6. Start the control app
   /opt/zwave-remote/bin/hzremote --daemon
   NOTE: to automatically launch the service add the above line to /etc/rc.local

7. From another machine on the network launch a browser and enter the following address
   http://<ip of r-pi>/hzr.php
 
    
NOTE: Drop me a mail if you face issues with any of the instructions above; comments, improvements 
      code enhancements are all welcome.
