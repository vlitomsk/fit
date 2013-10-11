object RKSearch {
	def fastpow(base: Int, exp: Int): Int = {
		def fastpow_(base: Int, exp: Int, acc: Int): Int =
			if (exp == 0) acc
			else if (exp % 2 == 0) fastpow_(base * base, exp / 2, acc)
			else fastpow_(base, exp - 1, acc * base)
			
		fastpow_(base, exp, 1)
	}                                       

	def rkSearchFrom(needle: String, haystack: String, from: Int): Option[Int] = {
		def rkHash(s: String): Int =
			s.zipWithIndex.map(t => (t._1 % 3) * fastpow(3, t._2)).sum
	
		val nlen = needle.length
		val nhash = rkHash(needle)
		
		def updateHash(oldHash: Int, addChar: Char): Int =
			(oldHash / 3 + (addChar % 3) * fastpow(3, nlen - 1)).toInt
		
		def rkSearch_(offset: Int, shash: Int): Option[Int] = {
			if (shash == nhash) Some(offset)
			else if (offset + nlen >= haystack.length) None
			else rkSearch_(offset + 1, updateHash(shash, haystack(offset + nlen)))
		}
		
		if (from + nlen <= haystack.length)
			rkSearch_(from, rkHash(haystack.substring(from, from + nlen)))
		else
			None
	}
	
	def rkSearch: (String, String) => Option[Int] = rkSearchFrom(_, _, 0)

	def rkSearchAll(needle: String, haystack: String): List[Int] = {
		def rkSearchAll_(from: Int, acc: List[Int]): List[Int] = {
			rkSearchFrom(needle, haystack, from) match {
				case Some(ind: Int) => rkSearchAll_(ind + 1, acc.:+(ind))
				case None => acc
			}
		}
		
		rkSearchAll_(0, List())
	}                       

	def main(args: Array[String]) = {
		val needle = readLine()
		val haystack = readLine()
		println(rkSearchAll(needle, haystack))
	}
}