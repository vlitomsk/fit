package assembler;

public class AsmException extends MyException {
	public AsmException(String s) {
		super("![ASM] " + s);
	}
}
