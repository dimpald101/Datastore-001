Row Structure:
The Row structure represents a single row in the CSV file, and it contains a vector of strings (data) where each string represents a cell value in the row.

TreeNode Base Class:
The TreeNode is an abstract base class for the nodes in the binary tree. It has a virtual function evaluate that needs to be implemented by its derived classes. This function is responsible for evaluating whether a given row satisfies the condition represented by the node.

LeafNode Class:
The LeafNode class represents a leaf node in the binary tree. It corresponds to a simple condition (e.g., column_name = value) and implements the evaluate function accordingly.

LogicalNode Class:
The LogicalNode class represents a non-leaf node in the binary tree. It corresponds to a logical operation (AND or OR) and contains pointers to its left and right child nodes.
cpp

CSVFilter Class:
The CSVFilter class is responsible for parsing the filter expression and creating a binary tree of TreeNode objects representing the filter conditions.

readCSV Function:
The readCSV function reads the CSV file and returns a vector of Row objects, where each Row corresponds to a line in the CSV.

Main Function:
In the main function, the CSV file is read using readCSV, and a CSVFilter object is created with the provided filter expression.
The filtered rows are then obtained using the filterRows function of the CSVFilter object.
Finally, the filtered rows are displayed in pages of 50, and we are prompted to continue to the next page.
