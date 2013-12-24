import System.IO
import Control.Monad
import Data.List
import Data.Char
import System.Environment

-- Einfo - edge info
-- wlast - символ, который добавляется в конец слова
-- cnt - количество повторений слова, заканчивающегося на данном "терме"
data Einfo = Einfo { sym::Char, _cnt::Int } deriving (Eq, Show)
data Trie = Node [(Trie, Einfo)] deriving (Eq, Show)

tmap :: ([(Trie, Einfo)] -> [(Trie, Einfo)]) -> Trie -> Trie
tmap f (Node lst) = Node (f lst) -- "trie map"

incEinfo :: Einfo -> Einfo
incEinfo (Einfo s c) = Einfo s (c + 1)
	
replaceAt :: (Eq a) => Int -> a -> [a] -> [a]
replaceAt n val (x:xs) 
	| n == 0 = val:xs
	| otherwise = x:replaceAt (n - 1) val xs

-- нужно добавлять перевернутые слова, 
-- тогда в traverse можно использовать prepend (вместо долгого append)
addToTrie :: String -> Trie -> Trie
addToTrie (ch:xs) tr@(Node edges) = 
	-- n' - Maybe Int, индекс ребра, на котором буква ch
	let n' = findIndex (\(_, e) -> ch == sym e) edges 
	in case n' of 
		Just n ->
			-- если в ch последний символ строки, тогда обновляем n-ое ребро
			if null xs then tmap (replaceAt n (child, incEinfo einfo)) tr
			else tmap (replaceAt n (addToTrie xs child, einfo)) tr
			where (child, einfo) = edges !! n
		Nothing -> 
			tmap ((addToTrie xs (Node []), Einfo ch initcnt):) tr
			where initcnt = if null xs then 1 else 0
addToTrie [] x = x

traverse' :: String -> Trie -> IO ()
traverse' s (Node xs) = do 
	mapM_ f xs
	return ()
	where f (child, Einfo symbol count) = do
		when (count /= 0) $ putStrLn $ reverse (symbol:s) ++ " " ++ show count
		traverse' (symbol:s) child
		
traverse :: Trie -> IO ()
traverse  = traverse' []
 
chTrimmable :: Char -> Bool
chTrimmable c = c `elem` ".,?!- \"\';:()"

trim :: String -> String
trim [] = []
trim s 
	| chTrimmable (head s) = trim $ tail s
	| chTrimmable (last s) = trim $ init s
	| otherwise = s
	
downcase :: String -> String
downcase = map toLower

main :: IO ()
main = do
	args <- getArgs
	handle <- openFile (head args) ReadMode
	--handle <- openFile "vm" ReadMode
	trie <- iter handle (Node [])
	traverse trie
	where iter hdl tracc = do
		eof <- hIsEOF hdl
		if eof then return tracc
		else do
			line <- hGetLine hdl
			iter hdl $ foldl' (flip addToTrie) tracc $ map (trim . downcase) $ words line
