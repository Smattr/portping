import System.Environment
import System.IO
import Network.Socket
import Network.BSD
import Control.Exception

tcpPing :: String -> String -> IO ()
tcpPing hostname port = Control.Exception.catch (do
    addrinfos <- getAddrInfo Nothing (Just hostname) (Just port)
    let serveraddr = head addrinfos
    sock <- socket (addrFamily serveraddr) Stream defaultProtocol
    connect sock (addrAddress serveraddr)
    sClose sock
    putStrLn $ "Response from " ++ hostname ++ " on port " ++ port ++ ".")
    (\e -> do
        let err = show (e :: SomeException)
        hPutStrLn stderr "Failed to connect.")

main = do
    progName <- getProgName
    args <- getArgs
    case (length args) of
        2 -> tcpPing (head args) (head $ tail args)
        _ -> (hPutStrLn stderr ("Usage: " ++ progName ++ " hostname port"))
