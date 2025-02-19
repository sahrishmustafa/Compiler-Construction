package example_1;
import java.nio.file.*;
import java.io.IOException;
import java.util.List;

public class Lexical_Main {
    public static void main(String[] args) {
        String filePath = "E:\\semester 6\\cc\\Coding stuff booo\\ass1\\src\\example_1\\input.c"; // 🔹 File containing source code
        try {
            // Read file content into a string
            String input = Files.readString(Path.of(filePath));

            // Tokenize file contents
            List<Token> tokens = LexicalAnalyzer.tokenize(input);
            
            // Display errors
            LexicalAnalyzer.displayErrors();
            // Display Tokens
            System.out.println("\n-- Tokenized Output --");
            tokens.forEach(System.out::println);

            // Display Symbol Table
            LexicalAnalyzer.symbolTable.display();
            LexicalAnalyzer.displayDFAForTokens();
            
        } catch (IOException e) {
            System.err.println("Error reading file: " + e.getMessage());
        }
    }
}
