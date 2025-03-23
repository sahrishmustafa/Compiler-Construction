package example_1;
import java.util.*;
import java.util.regex.*;
import dk.brics.automaton.*;

public class LexicalAnalyzer {
    private static final Map<String, String> TOKEN_PATTERNS = new LinkedHashMap<>();
    public static SymbolTable symbol_Table = new SymbolTable(); 
    private static Deque<Integer> scope_Stack = new ArrayDeque<>();
    private static int current_Scope = 0;
    
    static { // RE patterns for matching - these are used by the Regex library
        TOKEN_PATTERNS.put("COMMENT", "@.*");  
        TOKEN_PATTERNS.put("MULTILINE_COMMENT", "@many@([\\s\\S]*?)@many@");
        TOKEN_PATTERNS.put("DATATYPE", "\\b(digit|decimal|zeroone|abc)\\b");
        TOKEN_PATTERNS.put("BOOLEAN", "\\b(true|false)\\b");
        TOKEN_PATTERNS.put("DECIMAL", "\\b\\d+\\.\\d{1,5}\\b");  
        TOKEN_PATTERNS.put("INTEGER", "\\b\\d+\\b");
        TOKEN_PATTERNS.put("CHARACTER", "'(.)'");  
        TOKEN_PATTERNS.put("OPERATOR", "\\b(eq|add|sub|mul|div|powow|mod)\\b");
        TOKEN_PATTERNS.put("I/O", "(come.|go.)");        
        TOKEN_PATTERNS.put("IDENTIFIER", "_[a-z]+_");
        TOKEN_PATTERNS.put("BRACKET", "[(){}]");
        TOKEN_PATTERNS.put("SEMICOLON", ";"); 
        TOKEN_PATTERNS.put("COMMA", ","); 
        TOKEN_PATTERNS.put("WHITESPACE", "\\s+"); 
        TOKEN_PATTERNS.put("FUNCTION", "\\b(main|ret)\\b");  
    }

    public static void enterScope() {
        current_Scope++;
        scope_Stack.push(current_Scope);
    }


    public static List<Token> tokenize(String code) {
        List<Token> tokens = new ArrayList<>(); // list of tokens
        int lineNumber = 1; 
        List<int[]> multiLineCommentRanges = new ArrayList<>();
        String lastDatatype = null; // keep last datatype to track valid identifiers 
        
        // match multi line comments first 
        Pattern multiLineCommentPattern = Pattern.compile("@many@([\\s\\S]*?)@many@", Pattern.MULTILINE);
        Matcher multiLineMatcher = multiLineCommentPattern.matcher(code);

        while (multiLineMatcher.find()) {
            String comment = multiLineMatcher.group();
            int commentStart = code.substring(0, multiLineMatcher.start()).split("\\n").length;
            int commentEnd = code.substring(0, multiLineMatcher.end()).split("\\n").length;
            tokens.add(new Token("MULTILINE_COMMENT", comment, commentStart));
            multiLineCommentRanges.add(new int[]{commentStart, commentEnd});
        }

        String[] lines = code.split("\\n");
        
        for (int i = 0; i < lines.length; i++) { 
            boolean inMultiLineComment = false;

            for (int[] range : multiLineCommentRanges) {
                if (lineNumber >= range[0] && lineNumber <= range[1]) {
                    inMultiLineComment = true;
                    break;
                }
            }

            if (inMultiLineComment) {
                lineNumber++;
                continue;
            }
            
            // match single - lined comments
            Pattern commentPattern = Pattern.compile("@.*");
            Matcher commentMatcher = commentPattern.matcher(lines[i]);

            if (commentMatcher.find()) { 
                String comment = commentMatcher.group();
                tokens.add(new Token("COMMENT", comment, lineNumber));
                lineNumber++;
                continue; 
            }
            
            int pos = 0;
            while (pos < lines[i].length()) {
                boolean matched = false;

                for (Map.Entry<String, String> entry : TOKEN_PATTERNS.entrySet()) { // for every RE, match the token found
                    String type = entry.getKey();
                    Pattern pattern = Pattern.compile(entry.getValue());
                    Matcher matcher = pattern.matcher(lines[i].substring(pos));
                    
                    if (matcher.find() && matcher.start() == 0) {
                        String value = matcher.group();

                        if (type.equals("BRACKET")) { // brackets allow scope tracking
                            if (value.equals("{")) symbol_Table.enterScope();
                            else if (value.equals("}")) symbol_Table.exitScope();
                        }

                        if (!type.equals("WHITESPACE")) { //spaces, tabs, nulls
                            tokens.add(new Token(type, value, lineNumber));
                            
                            if (type.equals("DATATYPE")) { // allows identifier tracking 
                                lastDatatype = value;
                            } else if (type.equals("IDENTIFIER") && lastDatatype != null) {
                                symbol_Table.declareVariable(value, lastDatatype); // add identifier to symbol table
                                lastDatatype = null;
                            }
                        }
                        pos += value.length();
                        matched = true;
                        break;
                    }
                }
                
                if (!matched) {
                    ErrorHandler.addError(lineNumber, "Unrecognized token near: '" + lines[i].charAt(pos) + "'");
                    pos++;
                }
            }
            lineNumber++;
        }
        return tokens;
    }
    
    public static void displayErrors() {
        ErrorHandler.displayErrors();
    }

    public static void displayDFAForTokens() {
        System.out.println("\nDFA Representation for Each Token Type:");
        for (Map.Entry<String, String> entry : TOKEN_PATTERNS.entrySet()) {
            String tokenType = entry.getKey();
            String regex = entry.getValue();

			// using import dk.brics.automaton.*;
            String automatonRegex = Conversion.convertToAutomatonRegex(regex);
            RegExp re = new RegExp(automatonRegex);
            Automaton dfa = re.toAutomaton();
            dfa.determinize();
            dfa.minimize();
            
            System.out.println("\nToken Type: " + tokenType);
            Conversion.printDFAStates(dfa);
        }
    }
}
