package assembler;

import java.io.*;

public class Main {
	public static void main(String[] args) throws IOException {
		if (args.length < 1) {
			System.err.println("Usage: java assembler input_file [output_file]");
			System.exit(1);
		}

		String in_fname = args[0];
		String out_fname = null;
		if (args.length == 1) 
			out_fname = in_fname.replaceAll(".asm", ".hex");
		else
			out_fname = args[1];

		Assembler asm = new Assembler();
		int status =	asm.assemble(in_fname, out_fname); // Горшочек, вари!
		if (status == 0)
			System.out.println("The program has compiled ok. See `" + out_fname + "`");
		else {
			System.out.println("The program has errors. Fix it, man!");
			System.exit(1);
		}			
	}
}
