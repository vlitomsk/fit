inv = (`mod` 2) . (+1)
invList = map inv
cmpLists a b = (foldl (&&) True . map (\(x,y) -> x <= y)) $ zip a b
fstHalf xs = take (length xs `div` 2) xs
sndHalf xs = drop (length xs `div` 2) xs
chkS xs = (fstHalf xs) == ((reverse . invList . sndHalf) xs)
chkM [_] = True
chkM xs = cmpLists (fstHalf xs) (sndHalf xs) && chkM (fstHalf xs) && chkM (sndHalf xs)
chkL [_] = True
chkL xs = ((fstHalf xs == sndHalf xs) || (fstHalf xs == (invList . sndHalf) xs)) && (chkL (fstHalf xs))
chkT0 xs = 0 == head xs
chkT1 xs = 1 == last xs
postParams = [ (not . chkT0), (not . chkT1), (not . chkS), (not . chkL), (not . chkM) ]
chkSheffer xs = foldl (&&) True $ map (\pred -> pred xs) postParams
chkComplete xs = foldl (&&) True $ map (\pred -> any pred xs) postParams
lists = [ [0, a, 0, b, c, 1, d, 1, 0, inv d, 0, inv c, inv b,  1, inv a, 1] | a <- [0,1], b <- [0,1], c <- [0,1], d <- [0,1] ] 
answer = length $ (filter (chkM) . filter (chkS)) $ lists
lst = [ [a, b, c, d, e, f, g, h] | a <- [0,1], b <- [0,1], c <- [0,1], d <- [0,1], e <- [0,1], f <- [0,1], g <- [0,1],h <- [0,1] ]

