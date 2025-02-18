package example_1;
import java.util.*;

// ✅ Symbol Table class to store identifiers
class SymbolTable {
    private Map<String, String> table = new HashMap<>(); // Stores <Identifier, Type>

    public void insert(String identifier, String type) {
        if (!table.containsKey(identifier)) {
            table.put(identifier, type);
        } else {
            System.out.println("Warning: Redeclaration of variable '" + identifier + "'");
        }
    }

    public void display() {
        System.out.println("\n--Symbol Table--");
        System.out.println("Identifier | Type");
        System.out.println("-----------------");
        for (Map.Entry<String, String> entry : table.entrySet()) {
            System.out.println(entry.getKey() + "       | " + entry.getValue());
        }
    }
}