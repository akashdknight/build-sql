# **```PROJECT PHASE 2 REPORT```**   
## **```ASSUMPTIONS :```** 
- Block size in our case is 1 kB, so it is assumed that none of the tables will contain more than 250 columns.
- Since the prebuilt table class had ```vector<vector<int>>``` to store the contents of each row, it is assumed that all the contents of the table are integers.
- It is also assumed that aggregate functions will not produce results which overflows the integer limit for the same reason as above.   
- It is assumed that none of the commands will produce empty relation as a result since in that case print function will fail. (Or any read attempts will fail for that matter)     
-------------------------
## **```IMPLEMENTATION DETAILS :```**  
### ```1. SORT FUNCTION```
- First two temporary files or copies of the original table was created.   
- Then 9 blocks are taken at a time and are sorted using cpp inbuilt sort function by passing a custom comparator function to the same.   
- These sorted blocks were written to temporary file 1.   
- Once sorting was done, 2-way merging  was initiated.
- 2-way merging is exactly same algorithm that is used in classic DSA merge sort.  
- If we are reading from tempfile1 then the merged data will be written to tempfile2 and viceversa.   
- Once the merging is completed and all the blocks are unified to form a big continous sorted chunk the loop stops.  
- Then this sorted data is copied to the original csv file block by block.   
- Once copying the sorted data is done, temporary files are deleted.   
- After that, makePermanent() function defined in table.cpp is called inorder to export the sorted result back to the original source file.    
-----------
### ```2. JOIN```   
- If table1 and table2 are being used as the parameters of join then temporary copies of both these tables are created.
- The temporary copies of both the tables are sorted in ascending order by the attribute used for joining condition.  
- Sorting function is slightly modified so as to take a boolean argument (true by default), which when set to false the sorted file is not exported and if not it will be made permanent.   
- Then we iterate through the rows of table1 and for each row, we find the required index of block of table2 by using the binary search. 
- Binary search is done by only using the first rows of Blocks and since in our case the block can contain at max of 250 rows, binary search on records is not done but instead linear search inside the block is used.  
- Binary search actually consists of two functions similar to lower_bound and upper_bound functions of cpp.
- So binary search returns the lower most(index1) and upper most index(index2) of blocks in which a particular value can be found.
- Now if we need less than, then blocks from 0 to index1 are processed.
- If we need less than or equal to, then blocks from 0 to index2 are processed.
- If we need greater than , then index2 to blockCount is processed. 
- If we need greater than or equal to , then index1 to blockCount is processed.
- If we need equal to, then index1 to index2 is processed. 
- Desired rows are copied to new pages and result page is written on to the disk.   
---------------------

### ```3. ORDER BY```  
- This is very similar to sorting.  
- We just created another copy of the given table.
- Then called sort function on it by passing the required attributes and sorting strategies as given through the command. 
- Then the obtained result after sorting will be as required by the ORDER BY command issued.   
---------------------------
### ```GROUP BY``` 
- First the query is processed inorder to get the aggregate function 1 and aggregate function 2 and their respective attributes after we obtain the tokenizedQuery.  
- Then the result table is setup by creating the required columns.  
- Copy of the original table is created and is sorted by grouping attribute in ascending order.  
- For each distinct value of the grouping attribute count, sum, min and max are tracked across each distinct value for the desired attribute.    
- As per the required aggregate function, after checking the given condition, corresponding value is added to the rows vector.  
- Once the rowBuffer reaches the size of maximum rows allowed per block its written to the page of the result table.   
---------------------

### **```LEARNINGS :```**
- We learnt about external sorting and how it can be used to sort large databases where we cannot hold the entire data on RAM in one go.
- We also learnt about the strategy of k-way merging and handling optimum number of blocks at a time to meet both memory and time constraints.  
- We learnt how external sorting or indexed relations can be used to optimise queries such as JOIN, ORDER BY, GROUP BY.  
- We gained a deeper understanding of how different SQL commands work and what happens under the hood.
- Overall, this phase of the project provided us with a great learning experience and taught us how to preprocess data inorder to optimise the queries.   

-----------------

### **```TEAM WORK```**
- All the members of team worked together  to come up with optimal strategies inorder to complete the given task by brainstorming different algorithms.  
- All the members of the team contributed equally towards this phase of the project. 
---------

# **```THANK YOU```**