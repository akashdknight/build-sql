# **Project Phase - 1 Report**
# **LOGIC FOR EACH COMMAND IMPLEMENTED** : 

## **LOAD MATRIX** :
- Most of the things are similar to Table and major differences being:     
-- There are no column names in case of matrix   
-- Rows and column size are same in matrix but there is no such restriction for the tables    
-- The way the table->load() is implemented can have at max of BLOCK_SIZE*1000/(sizeof(int)) number of columns.      
- So keeping in mind all these things we had to do the following things for implementing LOAD MATRIX:     
-- Created matrixcatalogue class for keeping track of the matrices which are presently loaded in the program and their details similar to tableCatalogue.    
-- Created a class for matrices with variables and methods similar to that of table class but with some modifications    
-- Since the maximum size of the matrix was 10<sup>4</sup>, and we also have to perform a tranpose of it, we modified the page-design to accomodate large number of columns unlike table page design which had a cap on its size.    
-- So instead of reading the complete rows in a go, we decided to read data in blocks of size nxn where n is sqrt((BLOCK_SIZE*1024)/(sizeof(int))).          
-- Towards the end we will be left with rectangular blocks since all the numbers are not exactly divisible by n. But we read them as it is without any paddings or such things to make them of same size as others.      
-- Since the row and column size can differ from block to blocks, we had to introduce one more vector<int> in matrix class which keeps track number of columns present in each block similar to already existing rowsCount vector which keeps track of number of rows present in each block.    
-- Due to this variable size blocks we also had to modify the writePage() in page.cpp to accomodate these changes. (some changes in variables inside for loops)     
-- And also we need to check for syntactic errors and semantic errors. Following the alredy built convention of this repo, we need to add two function declarations in semanticParser.h and syntacticParser.h then call those functions from respective .cpp files, then write another cpp file for loadmatrix inside executors directors and write the definition of those functions similar to other commands.         
-- Now, we are only writing to the blocks after reading from the harddisk, so no blocks are being read from. In matrix class we already have a blockCount variable which is incremented everytime a new block is created. So, we can  just output the value of that variable for the number of blocks written to, and number of blocks read from are zero.     
--------------------

## **PRINT MATRIX** :    
- For printing the matrix we need to use Cursor and BufferManger classes.    
- But the original implementation gets the table details from table catalogue, and uses the details obtained from it to deduce the page name and print.      
- So, we just implemented an additional boolean parameter called isMatrix whose default value is false and if set to true will get the details from matrixCatalogue and if not from tableCatalogue. Therefore both cases of table and matrices can be handled by setting the flag to false or true while calling the cursor and bufferManger functions.    
- And also the writeRows() function of table assumes that everytime we print a complete row so it automatically inserts a newline character at the end, but in our case different columns of same row are present at different blocks due to changed page design we need to slightly modify the writeRow() function which does not insert newline everytime, again it is implemented using a boolean flag which when set to true a newline character is printed and if not a space character is printed.     
- Since we are reading from the blocks of the matrix present in temp folder and writing to cout, the number of blocks read in this case will be matrix->blockCount and number of blocks written to is 0.          
---------------------

## **TRANSPOSE MATRIX** :  
- Our page structure is such that each page stores a max of n rows and n columns and it need not be square.        
- But the total number of pages will always be square number since the input matrix has equal number of rows and equal number of columns.           
- So we can arrange the pages themselves in the form of matrix, where each entry in the bigger matrix is a smaller matrix which is data present inside that block.          
- Now, inorder to transpose the matrix we will pick two blocks which are diagonally opposite to each other and take their transpose and then switch them with each other.    
- In this procedure we will be using a temporary arrays to store the data of each page which is 2 blocks of memory.     
- And we have implemented a getter method to get all the rows of page at a single go instead of using Cursor to get one row at a time.     
- And while swapping them we have used the writePage() method which was pre-written for tables.       
- We are reading from the blocks of the matrices and we are writing back to the same blocks ( no extra blocks are created ) So number of matrices read is matrix->blockCount, number of matrices written to is also the same. And since we are accessing the same set of blocks for reading and writing total number of blocks accessed will also be same and not their sum in this case.                   
--------------------

## **EXPORT MATRIX** : 
- There already exists a function called makePermanent() in table.cpp. We just need to copy the same function for matrix.cpp.    
- Inside the function what we are doing is, we will open a file with the name matrix->sourceFileName inside data folder with fstream in ios::trunc mode.    
- Then we will write the rows of matrix into that file using the writeRow() function present in matrix.h where we will use filepointer intstead of cout.
- Overall logic is similar to print matrix but instead of cout we will be using file pointer to the opened file.    
----------------------------

## **RENAME MATRIX** : 
- First, change the matrix->matrixName to the new name, but matrix->sourceFileName is not being changed and it remains as it is.
- Then, there exists a map in matrixCatalogue in which name of matrix is key and Matrix * is the value. Using the insertMatrix() function insert the new name as key with Matrix * being the same value as the original value before renaming. 
- Now delete the old name key, value pair from the map.  
- Once this is done iterate through all the pages present in the temp folder to rename them with the new matrix name.    
- Now in ~unload() function which is called automatically once the program ends is actually deleting the source files if matrixName != sourceFileNames in the implementation present in table.cpp, so that deleting part is removed from the ~unload() of matrix.cpp.      
- Also once the rename is done bufferManger's pool should be flushed, becuase it stores the recently called pages in its memory and once the renaming is done there are chances that the pool still contains the pages related to oldname which needs to be removed.    
----------------------

## **CHECKSYMMETRY** :
- It is implemented exactly similar to transpose by taking two blocks at at time which are diagonally opposite.   
- But instead of tranposing and switching them, we are transposing only one of them checking if its equal to the other block. And also all the blocks present along the diagonal should be symmetric matrices themselves. If this is the case then return TRUE or else return FALSE.   
-----------

## **COMPUTE** : 
- Loaded matrix A is transposed.
- Then its renamed to A_RESULT.
- Now again matrix A is loaded.
- Once both the matrices are present we load one page at a time to main memory then compute A<sup>T</sup> - A and using writePage() function update the A_RESULT matrix'x page.  
- We will do the same for all the pages of the matrix.    
---------------

# **LEARNINGS FROM THIS PHASE OF PROJECT** : 

- We understood the importance of data partioning.
- We learnt how to handle large data by splitting them into smaller blocks.    
- By thinking of the optimal solutions to perform various types of commands we learnt to come up with page design based on the requirements.    
- We also learnt about how a database software should work.   
- We learnt the OOPs way to properly implement by splitting different functinalities into various structures with proper documentation.     
--------------------  

# **TEAM WORK**
- First, all three of us studied the code by ourselves inorder to understand the code structure of the repo. Once we understood it we discussed with each other regarding the same clarifying things.   
- Then we discussed the logic for each and every command and regarding the page design for the same.   
- Then we coded the logic discussed among ourselves and tested it out together.  
- We cant split commands individually to say who did what because we discussed the logic together then once the code is done modifications were done by everyone in order to update things. So, everyone contributed equally to the project.   

----------------





