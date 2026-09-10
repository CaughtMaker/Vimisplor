The idea of this code was to replicate the capabilites of file explorer that implemented the 'hands on the keyboard' approach that vim uses
It works as follows.
Opens you up in desktop as part of the default
Each green letter next to a file is the key you can press to choose it.
there are some commands that will help you navigate:
-> [,] and '.' will go back and forth between the pages, I only allow 15 files per directory to be shown at a time so if 
   there's more than that the code breaks the directory up into pages. Like pornhub.
-> [;] allows you to go into the parent directory of the current directory
-> [`] allows you to enter a custom path you want
-> [/] let's you quit
-> ['] let's you search

About the search:
  The search function only searches within your current directory.
  when you're done typing press ['] again and the code will assign a letter that will let you choose the file
