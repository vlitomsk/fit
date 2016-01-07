-- 1. context-free grammar leftmost and rightmost derivations
-- 2. LL(k) property checking
import Data.List
import Data.List.Split
import System.Environment
import Control.Monad

allUnique' _ [] = True
allUnique' uniqList (x:xs) 
  | x `elem` uniqList = False
  | otherwise = allUnique' (x:uniqList) xs
allUnique :: (Eq a) => [a] -> Bool
allUnique = allUnique' []

xfixes s = zip (inits s) (tails s)
flatten = foldl (++) []

type Seq = String
type StackLog = [Seq]
data Grammar = Grammar { rules :: [(Char, Seq)] , startNT :: Char } deriving (Show)
isNonTerm ch = ch >= 'A' && ch <= 'Z'
isTerm ch = ch >= 'a' && ch <= 'z'

deriveL :: Grammar -> Seq -> [Seq]
deriveL gram seq 
  | null ntseq = [seq]
  | otherwise = map (\(rhead,rtail) -> tseq ++ rtail ++ tail ntseq) $ 
                filter ((== head ntseq) . fst) $ rules gram
  where (tseq,ntseq) = span isTerm seq

-- L/R derivations --

checkR' :: StackLog -> Grammar -> Seq -> [StackLog]
checkR' sl gram inpSeq
  | (null inpSeq && head sl == [startNT gram]) = [sl]
  | otherwise = (flatten $ map (\var -> checkR' (var:sl) gram inpSeq) substVariants)
                ++ if (null inpSeq) then []
                                    else checkR' ((head sl ++ [head inpSeq]):sl) gram (tail inpSeq) 
  where substVariants = flatten $
                        map (\(l, r) -> (map (\(f,_) -> l ++ [f]) . filter ((==r).snd)) $ rules gram) $
                        xfixes $ head sl

checkR :: Grammar -> Seq -> [StackLog]
checkR = checkR' [[]]

checkL' :: StackLog -> Grammar -> Seq -> [StackLog]
checkL' s@([]:_) _ [] = [s]
checkL' _ _ [] = []
checkL' sl@((tophead:toptail):_) gram inpSeq@(ic:ics)
  | (isTerm tophead && ic /= tophead) = []
  | (ic == tophead) = checkL' (toptail:sl) gram ics
  | otherwise = flatten $
                map (\drv -> checkL' (drv:sl) gram inpSeq) $
                deriveL gram (tophead:toptail)
checkL' ([]:_) gram _ = []

checkL:: Grammar -> Seq -> [StackLog]
checkL gram = checkL' [[startNT gram]] gram

-- LL(k) checking --

checkLLk' curK maxK gram seqlist
  | curK == maxK + 1 = True
  | not $ allUnique $ map (head) lTerminated = False
  | otherwise = checkLLk' (curK + 1) maxK gram $ map (tail) lTerminated
  where lTerminated = filter (/=[]) $
                      flatten $
                      map (\s -> if (isTerm $ head s) then [s] else deriveL gram s) seqlist

checkLLk :: Int -> Grammar -> Bool
checkLLk k gram = all (checkLLk' 0 k gram . map (snd)) $  
                  filter ((>1) . length) $ 
                  groupBy (\(h1,_) (h2,_) -> h1==h2) $
                  rules gram

readGram (startStr:rulesLst) = Grammar { rules = readRules rulesLst , startNT = head startStr}
  where readRules = map (\s -> (,) (head $ (splitOn ">" s)!!0) ((splitOn ">" s)!!1))

-- Interaction with user --

main = do
  argv <- getArgs
  if null argv then do 
    putStrLn "Pass grammar file as argument!"
  else do
    grammar <- liftM (readGram . lines) (readFile $ head argv)
    print grammar
    putStrLn "For what k do you want to test LL(k)?"
    kstr <- getLine
    putStrLn $ "LL(" ++ kstr ++ ")? That's " ++ (show $ checkLLk (read kstr :: Int) grammar)
    putStrLn "What sequence do you want to check?"
    testStr <- getLine
    mapM (\(name,fn) -> do 
      putStr $ name ++ " derivation stack logs (empty if sequence doesn't satisfy grammar): \n  "
      print $ fn grammar testStr)  [("Rightmost",checkR), ("Leftmost", checkL)]
    return ()
