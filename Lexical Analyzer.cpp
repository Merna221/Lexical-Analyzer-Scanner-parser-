#include <iostream> 
#include <string> 
#include <unordered_map> 
#include <vector> 
#include <fstream> 
using namespace std; 
enum TokenType { 
RESERVED_WORD, 
IDENTIFIER, 
NUMBER, 
PLUS, 
MINUS, 
MULTIPLY, 
DIVISION, 
OPERATOR, 
DELIMITER, 
DATA_TYPE, 
LOGICAL_OPERATOR, 
SEMICOLON, 
INCREMENTAL, 
DECREMENTAL, 
ASSIGNMENT_OP, 
COMPARISON_OP, 
QUOTATIONS, 
UNKNOWN 
}; 
struct Token { 
string lexeme; 
TokenType type; 
}; 
unordered_map<string, TokenType> reservedWords = { 
{"if", RESERVED_WORD}, 
{"else", RESERVED_WORD}, 
{"then", RESERVED_WORD}, 
{"for", RESERVED_WORD}, 
{"do", RESERVED_WORD}, 
{"while", RESERVED_WORD}, 
{"read", RESERVED_WORD}, 
{"print", RESERVED_WORD} 
}; 
unordered_map<string, TokenType> dataTypes = { 
{"int", DATA_TYPE}, 
{"char", DATA_TYPE}, 
{"string", DATA_TYPE}, 
{"float", DATA_TYPE} 
}; 
bool isLogicalOperator(const string& op) { 
    return op == "&&" || op == "||" || op == "!"; 
} 
 
TokenType getTokenType(const string& lexeme) { 
    if (reservedWords.count(lexeme)) 
        return reservedWords[lexeme]; 
    else if (dataTypes.count(lexeme)) 
        return dataTypes[lexeme]; 
    else if (isdigit(lexeme[0])) 
        return NUMBER; 
    else if (isalpha(lexeme[0])) 
        return IDENTIFIER; 
    else { 
        switch (lexeme[0]) { 
        case '=': 
            return (lexeme.size() == 1) ? ASSIGNMENT_OP : COMPARISON_OP; // 
Change to return COMPARISON_OP if size > 1 
        case '(': 
        case ')': 
        case '{': 
        case '}': 
            return DELIMITER; 
        case ';': 
            return SEMICOLON; 
        case '+': 
            return (lexeme.size() == 1) ? PLUS : (lexeme == "++" ? INCREMENTAL 
: UNKNOWN); 
        case '-': 
            return (lexeme.size() == 1) ? MINUS : (lexeme == "--" ? DECREMENTAL 
: UNKNOWN); 
        case '*': 
            return (lexeme.size() == 1) ? MULTIPLY : UNKNOWN; 
        case '/': 
            return (lexeme.size() == 1) ? DIVISION : UNKNOWN; 
        case '\"': 
            return QUOTATIONS; 
        default: 
            // Check for comparison operators 
            if (lexeme == "==" || lexeme == ">=" || lexeme == "<=" || lexeme == 
">" || lexeme == "<" || lexeme == "!=") 
                return COMPARISON_OP; 
            // Check for logical operators 
            else if (isLogicalOperator(lexeme)) 
                return LOGICAL_OPERATOR; 
            else 
                return UNKNOWN; 
        } 
    } 
} 
 
vector<Token> tokenize(const string& input) { 
    vector<Token> tokens; 
    string currentToken; 
    TokenType currentType = UNKNOWN; 
 
    auto addToken = [&]() { 
        if (!currentToken.empty()) { 
            tokens.push_back({ currentToken, currentType }); 
            currentToken.clear(); 
        } 
        }; 
 
    for (size_t i = 0; i < input.size(); ++i) { 
        char c = input[i]; 
        if (isspace(c) || c == '(' || c == ')' || c == '{' || c == '}' || c == 
';') { 
            addToken(); 
            if (c != ' ') { 
                tokens.push_back({ string(1, c), getTokenType(string(1, c)) }); 
                currentType = UNKNOWN; 
            } 
        } 
        else { 
            currentToken += c; 
            TokenType type = getTokenType(currentToken); 
            // Check if the current token is a logical operator 
            if (type == UNKNOWN && isLogicalOperator(currentToken)) { 
                currentType = LOGICAL_OPERATOR; 
            } 
            else if (type != UNKNOWN) { 
                currentType = type; 
            } 
            else { 
                // If the current token is unknown, and we have characters 
remaining, 
                // keep building the token until we find a recognized token or 
reach the end 
                if (i == input.size() - 1) { 
                    tokens.push_back({ currentToken, UNKNOWN }); 
                } 
                continue; 
            } 
        } 
    } 
 
    addToken(); 
    return tokens; 
} 
 
string analyzeSyntax(const vector<Token>& tokens) { 
    bool ifFound = false; 
    bool conditionFound = false; 
    bool thenFound = false; 
    bool statementFound = false; 
    int bracketCount = 0; 
 
    for (size_t i = 0; i < tokens.size(); ++i) { 
        const Token& token = tokens[i]; 
 
        if (token.type == RESERVED_WORD) { 
            if (token.lexeme == "if") { 
                ifFound = true; 
            } 
            else if (ifFound && token.lexeme == "then") { 
                if (!conditionFound) 
                    return "Error: Condition missing after 'if'"; 
                thenFound = true; 
                statementFound = false; 
            } 
        } 
        else if (ifFound && !conditionFound && token.type == DELIMITER && 
token.lexeme == "(") { 
            conditionFound = true; 
        } 
        else if (thenFound && token.type != RESERVED_WORD) { 
            statementFound = true; 
        } 
 
        if (conditionFound) { 
            if (token.type == DELIMITER && token.lexeme == "(") 
                bracketCount++; 
            else if (token.type == DELIMITER && token.lexeme == ")") { 
                bracketCount--; 
                if (bracketCount < 0) 
                    return "Error: Incorrect bracket closure"; 
            } 
        } 
    } 
 
    if (ifFound && !conditionFound) 
        return "Error: Condition missing after 'if'"; 
    if (!thenFound && conditionFound) 
        return "Error: 'then' statement missing after condition"; 
    if (!statementFound && thenFound) 
        return "Error: Statement missing after 'then'"; 
    if (bracketCount != 0) 
        return "Error: Incorrect bracket closure"; 
 
    return "Syntax analysis passed"; 
} 
 
int main() { 
    string input; 
    ifstream file("Text.txt"); // Open the input file 
    if (file.is_open()) { 
        // Read the entire contents of the file into the input string 
        input.assign((istreambuf_iterator<char>(file)), 
istreambuf_iterator<char>()); 
        file.close(); // Close the file 
    } 
    else { 
        cerr << "Unable to open file"; 
        return 1; // Exit with error code if file cannot be opened 
    } 
 
    vector<Token> tokens = tokenize(input); 
    string syntaxResult = analyzeSyntax(tokens); 
 
    cout << "Tokenized equation:\n\n"; 
    for (const Token& token : tokens) { 
        cout << "Lexeme: " << token.lexeme << ", Type: "; 
        switch (token.type) { 
        case RESERVED_WORD: 
            cout << "Reserved Word"; 
            break; 
        case IDENTIFIER: 
            cout << "Identifier"; 
            break; 
        case NUMBER: 
            cout << "Number"; 
            break; 
        case PLUS: 
            cout << "Plus"; 
            break; 
        case MINUS: 
            cout << "Minus"; 
            break; 
        case MULTIPLY: 
            cout << "Multiply"; 
            break; 
        case DIVISION: 
            cout << "Division"; 
            break; 
        case DELIMITER: 
            cout << "Delimiter"; 
            break; 
        case DATA_TYPE: 
            cout << "Data Type"; 
            break; 
        case LOGICAL_OPERATOR: 
            cout << "Logical Operator"; 
            break; 
        case SEMICOLON: 
            cout << "Semicolon"; 
            break; 
        case INCREMENTAL: 
            cout << "Incremental"; 
            break; 
        case DECREMENTAL: 
            cout << "Decremental"; 
            break; 
        case ASSIGNMENT_OP: 
            cout << "Assignment Operator"; 
            break; 
        case COMPARISON_OP: 
            cout << "Comparison Operator"; 
            break; 
        case QUOTATIONS: 
            cout << "Quotations"; 
break; 
case UNKNOWN: 
cout << "Unknown"; 
break; 
} 
cout << endl; 
} 
if (syntaxResult != "Syntax analysis passed") { 
cout << "\nSyntax Error: " << syntaxResult << endl; 
} 
return 0; 
} 
