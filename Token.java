package example_1;

class Token {
    String type, value;
    int line;

    Token(String type, String value, int line) {
        this.type = type;
        this.value = value;
        this.line = line;
    }

    @Override
    public String toString() {
        return "Token{type='" + type + "', value='" + value + "', line=" + line + "}";
    }
}