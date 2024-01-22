#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <memory>
#include <stack>

struct Row {
    std::vector<std::string> data;
};

class TreeNode {
public:
    virtual bool evaluate(const Row& row) const = 0;
    virtual ~TreeNode() {}
};

class LeafNode : public TreeNode {
private:
    std::string column_name;
    std::string op;
    std::string value;

public:
    LeafNode(const std::string& col, const std::string& oper, const std::string& val)
        : column_name(col), op(oper), value(val) {}

    bool evaluate(const Row& row) const override {
        // For simplicity, assuming column names are indices starting from 0
        int columnIndex = std::stoi(column_name);
        if (columnIndex < 0 || columnIndex >= row.data.size()) {
            return false;  // Invalid column index
        }

        const std::string& columnValue = row.data[columnIndex];

        if (op == "=") {
            return columnValue == value;
        } else if (op == "!=") {
            return columnValue != value;
        } else if (op == "<=") {
            return std::stoi(columnValue) <= std::stoi(value);
        } else {
            // Add more operators as needed
            return false;  // Unsupported operator
        }
    }
};

class LogicalNode : public TreeNode {
private:
    char operation;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;

public:
    LogicalNode(char op, std::unique_ptr<TreeNode> l, std::unique_ptr<TreeNode> r)
        : operation(op), left(std::move(l)), right(std::move(r)) {}

    bool evaluate(const Row& row) const override {
        if (operation == 'A') {  // 'A' represents logical AND
            return left->evaluate(row) && right->evaluate(row);
        } else if (operation == 'O') {  // 'O' represents logical OR
            return left->evaluate(row) || right->evaluate(row);
        } else {
            return false;  // Unsupported logical operation
        }
    }
};

class CSVFilter {
private:
    std::vector<std::unique_ptr<TreeNode>> filters;

public:
    CSVFilter(const std::string& filterExpr) {
        std::stack<std::unique_ptr<TreeNode>> nodeStack;
        size_t i = 0;  // Initialize 'i' here

        for (char c : filterExpr) {
            if (c == '(') {
                // Push nullptr to the stack
                nodeStack.push(nullptr);
            } else if (c == ')') {
                // Pop elements until '(' is encountered
                std::vector<std::unique_ptr<TreeNode>> tempNodes;
                while (!nodeStack.empty() && nodeStack.top() != nullptr) {
                    tempNodes.push_back(std::move(nodeStack.top()));
                    nodeStack.pop();
                }

                nodeStack.pop();  // Remove '('

                // Pop AND/OR and create a new LogicalNode
                char logicalOp = nodeStack.top().get()->evaluate(Row());  // Get char value
                nodeStack.pop();
                nodeStack.push(std::make_unique<LogicalNode>(logicalOp, std::move(tempNodes[1]), std::move(tempNodes[0])));
            } else if (c == 'A' || c == 'O') {
                // Push AND ('A') or OR ('O') to the stack
                nodeStack.push(std::make_unique<char>(c));
            } else {
                // Parse leaf node (column_name operator value)
                std::string col, op, val;
                while (c != ' ' && c != ')' && c != '(' && c != '\0') {
                    col += c;
                    c = filterExpr[++i];
                }

                // Skip spaces
                while (c == ' ') {
                    c = filterExpr[++i];
                }

                while (c != ' ' && c != ')' && c != '(' && c != '\0') {
                    op += c;
                    c = filterExpr[++i];
                }

                // Skip spaces
                while (c == ' ') {
                    c = filterExpr[++i];
                }

                while (c != ' ' && c != ')' && c != '(' && c != '\0') {
                    val += c;
                    c = filterExpr[++i];
                }

                // Create LeafNode and push to the stack
                nodeStack.push(std::make_unique<LeafNode>(col, op, val));
            }
        }

        // Pop remaining elements and create a final filter
        while (!nodeStack.empty()) {
            filters.push_back(std::move(nodeStack.top()));
            nodeStack.pop();
        }
    }

    std::vector<Row> filterRows(const std::vector<Row>& rows) const {
        std::vector<Row> result;
        for (const Row& row : rows) {
            bool match = true;
            for (const auto& filter : filters) {
                if (!filter->evaluate(row)) {
                    match = false;
                    break;
                }
            }

            if (match) {
                result.push_back(row);
            }
        }

        return result;
    }
};

std::vector<Row> readCSV(const std::string& filename) {
    std::vector<Row> rows;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cell;
            Row row;
            while (std::getline(ss, cell, ',')) {
                row.data.push_back(cell);
            }
            rows.push_back(row);
        }
        file.close();
    }
    return rows;
}

int main() {
    std::vector<Row> allRows = readCSV("example.csv");

    // Example filter expression: (((0 = "practo") and (1 != "dogreat")) or (2 <= 100))
    std::string filterExpression = "(((0 = \"practo\") and (1 != \"dogreat\")) or (2 <= 100))";
    CSVFilter csvFilter(filterExpression);

    std::vector<Row> filteredRows = csvFilter.filterRows(allRows);

    const int pageSize = 50;
    for (size_t i = 0; i < filteredRows.size(); i += pageSize) {
        for (size_t j = i; j < i + pageSize && j < filteredRows.size(); ++j) {
            // Display the row
            for (const auto& cell : filteredRows[j].data) {
                std::cout << cell << "\t";
            }
            std::cout << std::endl;
        }

        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return 0;
}
