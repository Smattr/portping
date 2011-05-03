/*
 * Checks the connectivity of a host on a given TCP port.
 *
 * This code has only been tested with GDC (GNU D Compiler) under version 1 of
 * D. It has not been compiled with DMD.
 */

import std.stdio;
import std.socket;
import std.string;
import std.date;

int main(string[] args) {
    int port;
    d_time start, stop;

    /* Check arguments. */
    if (args.length < 3) {
        fwritef(stderr, "Usage: %s hostname port\n", args[0]);
        return 1;
    }

    /* Convert port to a numerical value. */
    port = atoi(args[2]);
    if (!port) {
        fwritef(stderr, "Argument port must be a numerical value.\n");
        return 1;
    }

    /* Attempt to connect to the given port. */
    start = getUTCtime();
    try
        Socket s = new TcpSocket(new InternetAddress(args[1], port));
    catch (Exception e) {
        fwritef(stderr, "Connection failed: %s\n", e);
        return 1;
    }
    stop = getUTCtime();

    writefln("Response from %s:%s (time=%dms)", args[1], args[2],
        (stop - start) / (TicksPerSecond / 1000));

    return 0;
}

