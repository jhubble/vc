# vc
View Compressed Files

View-Compress v1.0  August 29, 1989  -  Copyright 1989 by Jeremy Hubble 


What it is: 

View-Compress (VC.EXE) is a program that allows you to view the files contained 
within a .ZIP, .ARC,  or .LZH file.  It provides a compact listing of all the 
files, along with sums of sizes, lentgths, etc. , and can accept wildcards. 

How to use it: 

To use view, either type VC, and press the enter key, or type 'VC filename'. 
If you don't supply the filename on the command line, you will be prompted for 
it when the program runs.  The filename can be a .ZIP file (FRED.ZIP), and .LZH 
file (HERB.LZH), an .ARC file (GEORGE.ARC), all .ZIP, .ARC, or .LZH files 
(*.ZIP , *.LZH, or even *.ARC), or the filename without an extension (FRED), 
or any other combination of wildcards and filename combinations you can 
dream up (*.* works, too)  If you do select a wild card in the extension, the 
program will quickly go through all files, but only print those that are .ZIP, 
.ARC, or .LZH, so there might be a pause in between the list while it is 
skipping past all the other non-ZIP/LZH/ARC files in the directory. 

NOTE:  specifying a filename with out an extensions is the same as using the 
(.*) extension. (i.e. BILL is equal to BILL.*) 

NOTE TWO:  The program uses the file's extension to check to see if a filename 
is actually a compressed file, therefore, if you happen to change a file's name 
from FRED.ZIP to FRED.ZQP, it will not be recognized by this program.  For ARC 
and ZIP files, the file header is also checked, to verify that it is actually 
the given file type.  With LZH files, I could not find a distinct header that 
could be used to verify it's type, therefore, if a file happens to have the .LZH 
extension, and  it is: 
(disk was corrupt)
