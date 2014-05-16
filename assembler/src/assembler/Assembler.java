package assembler;

import java.util.function.*;
import java.util.*;
import java.io.*;

class Tag {
	// tag types
	public static final int  
		T_OPCODE = 0,
		T_REG = 1,
		T_ADDR = 2,
		T_IMMED = 3;

	public int type, val;
	public Function<Integer, BiFunction<Integer, Integer, FineInstr>> asmFunction = null; // if present

	public Tag(int type, int value, Function<Integer, BiFunction<Integer, Integer, FineInstr>> asmFunction) {
		init(type, value);
		this.asmFunction = asmFunction;
	}

	public Tag(int type, int value) {
		init(type, value);
	}

	private void init(int t, int v) {
		type = t;
		val = v;
	}
}

class Arg {
	public static final int T_RG = 1, T_IM = 2, T_LAB = 4, T_NO = 8;
	public static final int T_AD = T_IM | T_LAB;
	public int t;
	public String v;
	public Arg(int type, String v) {
		this.t = type;
		this.v = v;
	}
}

class Pair<A, B> {
	public A a;
	public B b;
	public Pair(A a, B b) {
		this.a = a;
		this.b = b;
	}
}

class FirstParsed {
	public String label;
	public Arg[] args;
	public int instr;
	
	public FirstParsed() {	
	}
	
	public FirstParsed(String label, Arg[] args, int instr) {
		this.label = label;
		this.args = args;
		this.instr = instr;
	}
}

class FineInstr {
	int[] inst;
	public FineInstr(int b0, int b1) {
		this.inst = new int[2];
		this.inst[0] = b0;
		this.inst[1] = b1;
	}
}

public class Assembler {
	private HashMap<String, Tag> lex_trie;

	private final int INSTR_COUNT = 12;
	private final int 
		OP_NOP = 0,
		OP_ADD = 1,
		OP_SUB = 2,
		OP_INC = 3,
		OP_DEC = 4,
		OP_JMP = 5,
		OP_JZ = 6,
		OP_MOV = 7,
		OP_TEST = 8,
		OP_MOVR = 9,
		OP_STOP = 10,
		OP_LD = 11;
	
	private final int
		RG_A = 0,
		RG_B = 1,
		RG_C = 2,
		RG_D = 3;

	public Assembler() {
		init_lex_trie();
		init_asm_functions();
	}

	private void init_lex_trie() {
		lex_trie = new HashMap<String, Tag>();
		lex_trie.put("NOP", new Tag(Tag.T_OPCODE, OP_NOP));
		lex_trie.put("SUB", new Tag(Tag.T_OPCODE, OP_SUB));
		lex_trie.put("ADD", new Tag(Tag.T_OPCODE, OP_ADD));
		lex_trie.put("INC",new Tag(Tag.T_OPCODE, OP_INC));
		lex_trie.put("DEC",new Tag(Tag.T_OPCODE, OP_DEC));
		lex_trie.put("JMP",new Tag(Tag.T_OPCODE, OP_JMP));
		lex_trie.put("MOV",new Tag(Tag.T_OPCODE, OP_MOV));
		lex_trie.put("JZ", new Tag(Tag.T_OPCODE, OP_JZ));
		lex_trie.put("TEST", new Tag(Tag.T_OPCODE, OP_TEST));
		lex_trie.put("MOVR", new Tag(Tag.T_OPCODE, OP_MOVR));
		lex_trie.put("STOP", new Tag(Tag.T_OPCODE, OP_STOP));
		lex_trie.put("LD", new Tag(Tag.T_OPCODE, OP_LD));
		lex_trie.put("RA", new Tag(Tag.T_REG, RG_A));
		lex_trie.put("RB", new Tag(Tag.T_REG, RG_B));
		lex_trie.put("RC", new Tag(Tag.T_REG, RG_C));
		lex_trie.put("RD", new Tag(Tag.T_REG, RG_D));
	}

	private BiFunction<Integer, Integer, FineInstr>asmFunctions[];
	private int[][] argTypes;

	private void init_asm_functions() {
		asmFunctions = (BiFunction<Integer, Integer, FineInstr>[]) new BiFunction[INSTR_COUNT];
		argTypes = new int[INSTR_COUNT][];
		
		asmFunctions[OP_NOP] = assembleNoArg.apply(OP_NOP);
		argTypes[OP_NOP] = new int[]{Arg.T_NO, Arg.T_NO};
		
		asmFunctions[OP_ADD] = assembleRegReg.apply(OP_ADD);
		argTypes[OP_ADD] = new int[]{Arg.T_RG, Arg.T_RG};
		
		asmFunctions[OP_SUB] = assembleRegReg.apply(OP_SUB);
		argTypes[OP_SUB] = new int[]{Arg.T_RG, Arg.T_RG};
		
		asmFunctions[OP_INC] = assembleReg.apply(OP_INC);
		argTypes[OP_INC] = new int[]{Arg.T_RG, Arg.T_NO};
		
		asmFunctions[OP_DEC] = assembleReg.apply(OP_DEC);
		argTypes[OP_DEC] = new int[]{Arg.T_RG, Arg.T_NO};
		
		asmFunctions[OP_JMP] = assembleAddr.apply(OP_JMP);
		argTypes[OP_JMP] = new int[]{Arg.T_AD, Arg.T_NO};
		
		asmFunctions[OP_JZ] = assembleAddr.apply(OP_JZ);
		argTypes[OP_JZ] = new int[]{Arg.T_AD, Arg.T_NO};
		
		asmFunctions[OP_MOV] = assembleRegImmed.apply(OP_MOV);
		argTypes[OP_MOV] = new int[]{Arg.T_RG, Arg.T_IM};

		asmFunctions[OP_TEST] = assembleReg.apply(OP_TEST);
		argTypes[OP_TEST] = new int[]{Arg.T_RG, Arg.T_NO};
		
		asmFunctions[OP_MOVR] = assembleRegReg.apply(OP_MOVR);
		argTypes[OP_MOVR] = new int[]{Arg.T_RG, Arg.T_RG};
		
		asmFunctions[OP_STOP] = assembleNoArg.apply(OP_STOP);
		argTypes[OP_STOP] = new int[]{Arg.T_NO, Arg.T_NO};
		
		asmFunctions[OP_LD] = assembleRegAddr.apply(OP_LD);
		argTypes[OP_LD] = new int[]{Arg.T_RG, Arg.T_AD};
	}
	
	private ArrayList<String> readLines(String in_f) throws IOException {
		BufferedReader br = new BufferedReader(new FileReader(in_f));
		ArrayList<String> lines = new ArrayList<String>();
		String buf = br.readLine();
		while (buf != null) {
			lines.add(buf.trim().toUpperCase());
			buf = br.readLine();
		}
		br.close();
		return lines;
	}

	// does s contain instruction with its arguments?
	private boolean isValuable(String s) {
		if (s.length() == 0)
			return false;
		return (s.charAt(0) != ';');
	}

	private boolean checkLabel(String s) {
		if (s == null)
			return true;
		if (lex_trie.containsKey(s)) // label can't be on of reserved words
			return false;

		return s.matches("[A-Z]+[0-9A-Z_]*");
	}

	// s = "mov   abc    def    ghi", delim = ' '
	// then it return {"mov", "abc    def    ghi"}, or if
	// s = "mov", delim = ' ',
	// then it return {null, "mov"}
	private String[] cut(String s, char delim) {
		String res[] = new String[2];
		int ind = s.indexOf(delim);
		if (ind < 0) {
			res[0] = null;
			res[1] = s;
		} else {
			res[0] = s.substring(0, ind).trim();
			res[1] = s.substring(ind + 1).trim();
		}
		return res;
	}

	private String trimComment(String s) {
		int ind = s.indexOf(';');
		if (ind < 0)
			return s;
		else
			return s.substring(0, ind).trim();
	}

	// splits s to array [label, s without label (trimmed)]
	// if s has no label, returns [null, s]
	private Pair<String, String> parseLabel(String s) throws SyntaxException {
		String[] res = cut(s, ':');
		
		if (!checkLabel(res[0]))
			throw new SyntaxException("Bad label format: `" + res[0] + "`");

		return new Pair<String, String>(res[0], res[1]);
	}

	private Pair<Integer, String> parseInstruction(String s) throws SyntaxException {
		String[] res = s.split("\\s+", 2);
		Tag tag = lex_trie.get(res[0]);
		if (tag == null)
			throw new SyntaxException("Bad instruction: `" + res[0] + "`");
		if (tag.type != Tag.T_OPCODE)
			throw new SyntaxException("Bad instruction: `" + res[0] + "`");
		return new Pair<Integer, String>(tag.val, res.length > 1 ? res[1] : "");
	}
	
	private Arg parseArg(String s) throws SyntaxException {
		if (checkLabel(s)) 
			return new Arg(Arg.T_LAB, s);
		
		if (checkNumber(s)) 
			return new Arg(Arg.T_IM, s);
		
		if (checkRegister(s)) 
			return new Arg(Arg.T_RG, s);
		
		if (s.equals(""))
			return new Arg(Arg.T_NO, null);
		
		throw new SyntaxException("Bad argument format: `" + s + "`");
	}

	private Arg[] parseArgs(String s) throws SyntaxException {		
		String[] tmp = s.split(",\\s*");
		if (tmp.length > 2) 
			throw new SyntaxException("Too many arguments: `" + s + "`");
		
		Arg[] res = new Arg[2];
		
		for (int i = 0; i < tmp.length; ++i) {
			res[i] = parseArg(tmp[i]);
		}
		
		for (int i = tmp.length; i < 2; ++i) {
			res[i] = new Arg(Arg.T_NO, null);
		}

		return res;
	}
	
	private boolean checkArgs(int opcode, Arg[] args) {
		for (int i = 0; i < args.length; ++i)
			if ((argTypes[opcode][i] & args[i].t) == 0)
				return false;
		
		return true;
	}

	/*
		returns Pair of array of 4 strings.
		[0] - label/null
		[1] - instruction/null (if null, line is not valuable)
		[2] - arg1/null
		[3] - arg2/null

		..and integer: 
	*/
	private FirstParsed parseLine(String s) throws SyntaxException, AsmException {
		s = trimComment(s);
//		System.out.println("Parsing line " + s);
		FirstParsed parsed = new FirstParsed();

		Pair<String, String> label = parseLabel(s);
		parsed.label = label.a;
		s = label.b; // string without label
		Pair<Integer, String> instr = parseInstruction(s);
		parsed.instr = instr.a;
		s = instr.b;

		Arg[] args = parseArgs(s);
		parsed.args = args;
		
//		System.out.println("Args:");
//		for (int i = 0; i < args.length; ++i) {
//			System.out.println("  t: " + args[i].t + "; s: `" + args[i].v + "`");
//		}
		
		if (!checkArgs(parsed.instr, args))
			throw new AsmException("Bad argument order (or types): `" + s + "`");
		
		return parsed;
	}

	private Pair< HashMap<String, Integer>, ArrayList<FirstParsed> >
			collectLabels(ArrayList<String> lines) throws SyntaxException, AsmException 
	{
		Object[] objArr = 
				lines
				.parallelStream()
				.filter(this::isValuable).toArray();
		
		String[] lines2 = Arrays.copyOf(objArr, objArr.length, String[].class);

		ArrayList<FirstParsed> fplist = new ArrayList<FirstParsed>();
		HashMap<String, Integer> labelMap = new HashMap<String, Integer>();
 
		int addr = 0;
		for (int i = 0; i < lines2.length; ++i) {
			String line = lines2[i];
			FirstParsed fp = parseLine(line);
			if (fp.label != null) {
				if (labelMap.containsKey(fp.label))
					throw new SyntaxException("Double label: `" + fp.label + "`");
				else
					labelMap.put(fp.label, addr);
			}
			fplist.add(fp);
			addr += 2;
		}

		return new Pair< HashMap<String, Integer>, ArrayList<FirstParsed> >(labelMap, fplist);
	}

	private int parseNumber(String s) {
		if (s.matches("\\d+")) 
			return Integer.parseInt(s);

		if (s.matches("0B[01]+")) 
			return Integer.parseInt(s.substring(2), 2);
		
		if (s.matches("0[0-7]+"))
			return Integer.parseInt(s, 8);

		if (s.matches("0X[0-9A-F]+"))
			return Integer.parseInt(s.substring(2), 16);

		return Integer.MAX_VALUE;
	}
	
	private boolean checkNumber(String s) {
		return parseNumber(s) != Integer.MAX_VALUE;
	}

	private int parseRegister(String arg) {
		return (int)lex_trie.get(arg).val;
	}
	
	private boolean checkRegister(String s) {
		Tag t = lex_trie.get(s);
		if (t == null) 
			return false;
		
		return t.type == Tag.T_REG;
	}

	private final int NO_ARG = Integer.MAX_VALUE;

	private int assembleArg(Arg arg, HashMap<String, Integer> labelMap) throws AsmException {
		if (arg.v == null)
			return NO_ARG;

		if (arg.t == Arg.T_IM) 
			return parseNumber(arg.v);

		if (arg.t == Arg.T_LAB) {
			Integer labAddr = labelMap.get(arg.v);
			if (labAddr == null)
				throw new AsmException("Can't find address for label `" + arg.v + "`");
			return (int)labAddr;
		} 

		if (arg.t == Arg.T_RG) 
			return parseRegister(arg.v);
		
		if (arg.t == Arg.T_NO)
			return NO_ARG;
		
		return NO_ARG;
	}

	Function<Integer, BiFunction<Integer, Integer, FineInstr>> assembleRegImmed = opcode -> {
		return (reg, immed) -> new FineInstr(
			(opcode << 4) | ((reg << 1) & 7),
			immed & 0xff
		);
	};

	Function<Integer, BiFunction<Integer, Integer, FineInstr>> assembleNoArg = opcode -> {
		return (arg1, arg2) -> {
			// ПЕРЕДЕЛАТЬ ТАК:
			/*
				Добавить в Tag поле, обозначающее требуемое число параметров
				На этапе до преобразования в инструкции считать параметры, и там выкидывать исключение
				Тогда на вход assembleXY будут поступать проверенные данные 
			*/
			return new FineInstr(opcode << 4, 0);
		};
	};

	// TODO
	/*
		Поправить в readme формат такого типа инструкций
		(второй регистр записывается не по частям в разные байты, а только в старшие биты второго)
	*/
	Function<Integer, BiFunction<Integer, Integer, FineInstr>> assembleRegReg = opcode -> {
		return (reg1, reg2) -> new FineInstr(
			(opcode << 4) | (reg1 << 1),
			reg2 << 5
		);
	};
	
	Function<Integer, BiFunction<Integer, Integer, FineInstr>> assembleAddr = opcode -> {
		return (addr, nope) -> new FineInstr(
			(opcode << 4) | ((addr >> 8) & 1),
			addr & 0xff
		);
	};
	
	Function<Integer, BiFunction<Integer, Integer, FineInstr>> assembleReg = opcode -> {
		return (reg, nope) -> new FineInstr(
			(opcode << 4) | (reg << 1),
			0
		);
	};

	Function<Integer, BiFunction<Integer, Integer, FineInstr>> assembleRegAddr = opcode -> {
		return (reg, addr) -> new FineInstr(
			(opcode << 4) | (reg << 3) | ((addr >> 8) & 1),
			addr & 0xff
		);
	};

	private int[] assembleLine(FirstParsed fpline, HashMap<String, Integer> labelMap) 
			throws SyntaxException, AsmException 
	{
		int arg1 = assembleArg(fpline.args[0], labelMap);
		int arg2 = assembleArg(fpline.args[1], labelMap);
		return asmFunctions[fpline.instr].apply(arg1, arg2).inst;
	}

	private int[] genBytecode(HashMap<String, Integer> labelMap, ArrayList<FirstParsed> fplist) 
			throws SyntaxException, AsmException
	{
		int[] res = new int[fplist.size() * 2];
		for (int i = 0; i < fplist.size(); ++i) {
			FirstParsed fpline = fplist.get(i);
			int[] asmline = assembleLine(fpline, labelMap);
			res[2 * i] = asmline[0] & 0xff;
			res[2 * i + 1] = asmline[1] & 0xff;
		}
		
		return res;
	}
	
	private void writeBytecode(int[] bytecode, String out_f) throws IOException {
		BufferedWriter bw = new BufferedWriter(new FileWriter(out_f));
		String s = HexOut.mkHex(bytecode);
		bw.write(s);
		bw.close();
	}

	public int assemble(String in_f, String out_f) 
			throws IOException
	{
		ArrayList<String> lines = readLines(in_f);
		Pair<HashMap<String, Integer>, ArrayList<FirstParsed>> labels = null;
		try {
			labels = collectLabels(lines);
		} catch (MyException e) {
			System.err.println(e.toString());
			return 1;
		}
		
		int[] bytecode = null;
		try {
			bytecode = genBytecode(labels.a, labels.b);
		} catch (MyException e) {
			System.err.println(e.toString());
			return 2;
		}
		
		writeBytecode(bytecode, out_f);
		
		return 0;
	}
}