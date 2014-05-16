package assembler;

public class HexOut {
	static final String endFile = ":00000001FF";
	public static String mkHex(int[] data) {
		String res = "";
		int count = data.length / 0xff;
		if (data.length % 0xff != 0)
			count++;
		int addr = 0;
		for (int i = 0; i < count; ++i) {
			int len = Math.min(addr + 0xff, data.length) - addr;
			res += mkRecord(len, addr, data) + "\r\n";
			addr += 0xff;
		}
		return res + endFile;
	}
	
	public static String mkRecord(int len, int addr, int[] data) {
		String res = 
				":" 
			  +	String.format("%02X", len)
			  + String.format("%04X", addr)
			  + "00"; // record type -- binary data
		
		int sum = len + addr;
		for (int i = addr; i < len; ++i) {
			res += String.format("%02X", data[i]);
			sum += data[i];
		}
		sum %= 256;
		sum = 256 - sum;
		
		res += String.format("%02X", sum);
		
		return res;
	}
}
