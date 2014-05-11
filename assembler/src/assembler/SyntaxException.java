package assembler;

public class SyntaxException extends MyException {
	public SyntaxException(String s) {
		super("![SYNT] " + s);
	}
}
