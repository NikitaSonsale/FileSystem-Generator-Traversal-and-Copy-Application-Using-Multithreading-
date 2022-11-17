# FileSystem-Traversal-and-file-generator-and-Copy-Application-Using-Multithreading-

Here I have created a fixed size file of 10 MB with random data and read from that file using multiple threads each thread reading 1MB data from the file.
and then stored that 1MB data in A node of linked list. Then I wrote the code to sort the node according to the sequence of data in the file. Then I have 
written a code to write the data in the node of linked list in the defferent file using again different threads(1MB each) with synchronization so that the 
original and final file should be identical.

I have written a program which accepts a file name as a parameter and traverse through the test directory recursively to find that file and returns the 
absolute path of that file and also added the wild card option to the filename i.e
                        i)   FileName starts with substring 
                        ii)  FileName ends with substring 
                        iii) FileName contains substring    
