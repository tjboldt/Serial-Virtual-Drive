# Serial-Virtual-Drive

Also known as Virtual Serial Drive.

This creates a virtual drive over the serial port on an Apple II computer where the drive data is hosted by another PC. I wrote this in one weekend back in 2001 out of necessity. I was the final Apple II librarian for L.O.G.I.C.'s shareware collection and wanted to preserve the disks from the 1980's before they became unreadable. At the time, I made it work with just an Apple IIgs as it had a built-in serial port. Years later, others discovered the code on my website and it was improved and used in many other projects. This code repo is not maintained and only posted here for historical purposes.

## Some noteable projects using code inspired by this project

https://github.com/ADTPro/adtpro

https://forum.arduino.cc/t/virtual-serial-drive-for-apple-ii-ported-to-arduino/143166

https://www.applefritter.com/node/19543

## L.O.G.I.C Disk Archive

https://www.applefritter.com/content/logic-toronto-user-groupbbs-apple-ii-disk-library-now-internet-archive

https://archive.org/search.php?query=creator%3A%22LOGIC%20%28“Loyal%20Ontario%20Group%20Interested%20In%20Computers”%29%22

## Replacement project

Since a cheap, tiny and energy efficient Raspberry Pi Zero W is more powerful than the computer that originally hosted the Serial Virtual Drive code, I designed a new expansion board with the Pi attached which can host many virtual drives on the microSD card. It also can allow the Apple II to execute Linux commands, connect to wifi and more.

https://github.com/tjboldt/Apple2-IO-RPi
