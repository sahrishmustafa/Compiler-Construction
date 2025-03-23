package example_1;
import dk.brics.automaton.*;

class Conversion {
    public static String convertToAutomatonRegex(String javaRegex) {
        // Remove word boundaries since automaton regex does not support \b
        String automatonRegex = javaRegex.replace("\\b", "");

        // Convert Java regex patterns to equivalent DFA-compatible patterns
        //automatonRegex = automatonRegex.replace("[\\s\\S]", "[\\x00-\\xFF]"); // Any character including newlines
        automatonRegex = automatonRegex.replace("\\d", "[0-9]"); // Digits
        automatonRegex = automatonRegex.replace("\\w", "[a-zA-Z0-9_]"); // Alphanumeric + underscore
        automatonRegex = automatonRegex.replace("\\s", "[ \\t\\n\\r\\f]"); // Whitespace characters
        automatonRegex = automatonRegex.replace("\\S", "[^ \\t\\n\\r\\f]"); // Non-whitespace

        // Handle multi-line comments @many@...@many@
        automatonRegex = automatonRegex.replace("\\[\\s\\S\\]", "[\\x00-\\xFF]");

        // Ensure optional quantifiers like `*?` are converted to `*`
        automatonRegex = automatonRegex.replaceAll("\\*\\?", "*");
        automatonRegex = automatonRegex.replaceAll("\\+\\?", "+");
        automatonRegex = automatonRegex.replaceAll("\\?\\?", "?");

        // Remove non-capturing group syntax (not supported in automaton regex)
        automatonRegex = automatonRegex.replaceAll("\\(\\?:.*?\\)", "");

        // Ensure correct handling of grouped choices like `(a|b)`, converting to `[ab]`
        automatonRegex = automatonRegex.replaceAll("\\((\\w)\\|(\\w)\\)", "[$1$2]");
        automatonRegex = automatonRegex.replaceAll("\\((\\w)\\|([^|()])\\)", "[$1$2]");

        return automatonRegex;
    }

    public static void printDFAStates(Automaton dfa) {
        System.out.println("DFA States and Transitions:");
        for (State state : dfa.getStates()) {
            System.out.println("State " + state + (state.isAccept() ? " [Accepting]" : " [Rejecting]"));
            for (Transition t : state.getTransitions()) {
                System.out.println("  Transition: '" + t.getMin() + "' → State " + t.getDest());
            }
            System.out.println();
        }
    }
}
