import System.IO
import Control.Monad
import Data.List
import Data.Char
import System.Environment

-- Einfo - edge info
-- wlast - символ, который добавляется в конец слова
-- cnt - количество повторений слова, заканчивающегося на данном "терме"
data Einfo = Einfo { sym::Char, cnt::Int } deriving (Eq, Show)
data Trie = Node [(Trie, Einfo)] deriving (Eq, Show)
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
			if (null xs) then tmap (replaceAt n (child, incEinfo einfo)) tr
			else tmap (replaceAt n (addToTrie xs child, einfo)) tr
			where (child, einfo@(Einfo t_ch t_cnt)) = edges !! n
		Nothing -> 
			tmap ((addToTrie xs (Node []), Einfo ch initcnt):) tr
			where initcnt = if (null xs) then 1 else 0
addToTrie [] x = x

tradd s = addToTrie (s)

traverse' :: String -> Trie -> IO ()
traverse' s (Node xs) = do 
	mapM f xs
	return ()
	where f (child, Einfo sym cnt) = do
		when (cnt /= 0) $ putStrLn $ reverse (sym:s) ++ " " ++ (show cnt)
		traverse' (sym:s) child
		
traverse :: Trie -> IO ()
traverse  = traverse' []
 
chTrimmable :: Char -> Bool
chTrimmable c = any (==c)".,?!- \"\';:()"

trim :: String -> String
trim [] = []
trim s 
	| chTrimmable (head s) = trim $ tail s
	| chTrimmable (last s) = trim $ init s
	| otherwise = s
	
downcase = map toLower

main :: IO ()
main = do
	args <- getArgs
	handle <- openFile (args !! 0) ReadMode
	--handle <- openFile "vm" ReadMode
	trie <- iter handle (Node [])
	traverse trie
	where iter hdl tracc = do
		isEOF <- hIsEOF hdl
		if (isEOF) then return tracc
		else do
			line <- hGetLine hdl
			--putStrLn line
			iter hdl $ foldl' (flip addToTrie) tracc $ map (trim . downcase) $ words line
