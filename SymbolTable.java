package example_1;
import java.util.*;

class SymbolTable {
    private Stack<Map<String, String>> scopeStack;
    private List<Map<String, String>> poppedScopes; // Stores all exited scopes

    public SymbolTable() {
        scopeStack = new Stack<>();
        poppedScopes = new ArrayList<>();
        scopeStack.push(new HashMap<>()); // Global scope (Level 0)
    }

    public void enterScope() {
        scopeStack.push(new HashMap<>());
    }

    public void exitScope() {
        if (scopeStack.size() > 1) {
            poppedScopes.add(scopeStack.pop()); // Store exited scope
        } else {
            System.out.println("Warning: Attempted to exit global scope.");
        }
    }

    public boolean declareVariable(String name, String type) {
        Map<String, String> currentScope = scopeStack.peek();

        if (currentScope.containsKey(name)) {
            System.out.println("Warning: Redeclaration of variable '" + name + "' in current scope.");
            return false;
        }
        currentScope.put(name, type);
        return true;
    }

    public String lookupVariable(String name) {
        for (int i = scopeStack.size() - 1; i >= 0; i--) {
            if (scopeStack.get(i).containsKey(name)) {
                return scopeStack.get(i).get(name);
            }
        }
        return null;
    }

    public void printSymbolTable() {
        System.out.println("--Symbol Table--");
        System.out.println("--Global Variables--");
        System.out.println("Scope Level | Identifier | Type");
        System.out.println("---------------------------------");

        List<Map<String, String>> scopeList = new ArrayList<>(scopeStack); // Active scopes
        int maxActiveLevel = scopeList.size() - 1;

        // Print Active Scopes
        for (int i = maxActiveLevel; i >= 0; i--) {
            for (Map.Entry<String, String> entry : scopeList.get(i).entrySet()) {
                System.out.printf("Level %d       | %s       | %s%n", i, entry.getKey(), entry.getValue());
            }
        }

        // Print Popped (Exited) Scopes
        if (!poppedScopes.isEmpty()) {
            System.out.println("\n--Exited(Local) Scopes--");
            System.out.println("Scope Level | Identifier | Type");
            System.out.println("---------------------------------");

            int exitedLevel = maxActiveLevel + 1; // Start numbering from the deepest active scope + 1
            for (Map<String, String> exitedScope : poppedScopes) {
                for (Map.Entry<String, String> entry : exitedScope.entrySet()) {
                    System.out.printf("Exited %d  | %s       | %s%n", exitedLevel, entry.getKey(), entry.getValue());
                }
                exitedLevel++;
            }
        }
    }
}
