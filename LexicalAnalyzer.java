package example_1;
import java.util.*;
import java.util.regex.*;

public class LexicalAnalyzer {
    private static final Map<String, String> TOKEN_PATTERNS = new LinkedHashMap<>();
    public static SymbolTable symbolTable = new SymbolTable(); // Symbol Table

    static {
        TOKEN_PATTERNS.put("COMMENT", "//.*|/\\*[\\s\\S]*?\\*/");  // Ensure this is checked first
        TOKEN_PATTERNS.put("DATATYPE", "\\b(int|float|bool|char)\\b");
        TOKEN_PATTERNS.put("BOOLEAN", "\\b(true|false)\\b");
        TOKEN_PATTERNS.put("DECIMAL", "\\b\\d+\\.\\d{1,5}\\b");  // Max 5 decimal places
        TOKEN_PATTERNS.put("INTEGER", "\\b\\d+\\b");
        TOKEN_PATTERNS.put("CHARACTER", "'(.)'");  // Allow any character inside single quotes
        TOKEN_PATTERNS.put("IDENTIFIER", "\\b[a-z]+\\b");
        TOKEN_PATTERNS.put("OPERATOR", "[+\\-=*/%^]");
        TOKEN_PATTERNS.put("BRACKET", "[(){};,]");
        TOKEN_PATTERNS.put("WHITESPACE", "\\s+");  // Ignored
    }

    public static List<Token> tokenize(String code) {
        List<Token> tokens = new ArrayList<>();
        int lineNumber = 1;
        String lastDatatype = null;  // Track the last found datatype

        // Remove and handle multi-line comments FIRST
        Matcher commentMatcher = Pattern.compile("/\\*[\\s\\S]*?\\*/").matcher(code);
        while (commentMatcher.find()) {
            tokens.add(new Token("COMMENT", commentMatcher.group(), lineNumber));
            code = code.replace(commentMatcher.group(), " "); // Remove comment from code
        }

        for (String line : code.split("\n")) {
            int pos = 0;
            while (pos < line.length()) {
                boolean matched = false;

                for (Map.Entry<String, String> entry : TOKEN_PATTERNS.entrySet()) {
                    String type = entry.getKey();
                    Pattern pattern = Pattern.compile(entry.getValue());
                    Matcher matcher = pattern.matcher(line.substring(pos));

                    if (matcher.find() && matcher.start() == 0) {
                        String value = matcher.group();

                        if (!type.equals("WHITESPACE") && !type.equals("COMMENT")) {
                            tokens.add(new Token(type, value, lineNumber));

                            // If a datatype is found, store it for the next identifier
                            if (type.equals("DATATYPE")) {
                                lastDatatype = value;
                            } 
                            // If an identifier follows a datatype, add it to the symbol table
                            else if (type.equals("IDENTIFIER") && lastDatatype != null) {
                                symbolTable.insert(value, lastDatatype);
                                lastDatatype = null;  // Reset after assignment
                            }
                        }

                        pos += value.length();
                        matched = true;
                        break;
                    }
                }

                if (!matched) {
                    System.out.println("Error: Unrecognized token at line " + lineNumber + " near: " + line.charAt(pos));
                    pos++;
                }
            }
            lineNumber++;
        }

        return tokens;
    }
}
