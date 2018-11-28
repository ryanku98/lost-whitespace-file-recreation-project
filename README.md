# lost-whitespace-file-recreation-project #

This is a C++ project created under the premise that we want to be able to identify all possible full sequences of words that exist in a whitespace-less sequence of characters: [Project Instructions.txt](https://github.com/ryanku98/lost-whitespace-file-recreation-project/blob/master/Project%20Instructions.txt)

## USE ##
### Requirements ###
Run in a UNIX shell like Bash or Terminal.


### Compile ###
##### dictionary.txt #####
~~~
g++ -o file_recreation file_recreation.cpp
~~~
##### dictionary2.txt #####
~~~
g++ -o file_recreation2 file_recreation2.cpp
~~~

### Run ###
Second and third arguments should be the filepath to the dictionary .txt file and the filepath to a "compressed" input .txt file.
##### dictionary.txt #####
~~~
./file_recreation dictionary.txt Examples/Ex1.txt
~~~
##### dictionary2.txt #####
~~~
./file_recreation2 dictionary2.txt Examples/Ex2.txt
~~~

### Author Notes ###
Files outputted represent every possible "original file" based on the dictionary .txt file used to define what constitutes as a "word", found in a generated Output directory.

The rankings.txt file can be used (also found in the Output directory) to get the ranks of each file's content. In addition, the console will output the top (up to 3) lowest ranked files (which represent the 3 files with the highest probability of being the original file).