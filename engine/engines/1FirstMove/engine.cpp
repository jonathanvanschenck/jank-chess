#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <regex>
#include <sstream>

#include "Game.h"
#include "Board.h"
#include "Zobrist.h"
#include "Attack.h"

const char* DEFAULT_SOCKET_PATH = "/tmp/chess";
const int DEFAULT_MEMSIZE = 1e6;
const int DEFAULT_SEARCH_TIME = 1e3; // ms

void init() {
    Attack::init();
    Zobrist::init();
}

std::string parse_go(std::string in, Game* gptr) {
    // TODO : make more fully featured ....
    // * go
    //    start calculating on the current position set up with the "position" command.
    //    There are a number of commands that can follow this command, all will be sent in the same string.
    //    If one command is not sent its value should be interpreted as it would not influence the search.
    //    * searchmoves <move1> .... <movei>
    //    	restrict search to this moves only
    //    	Example: After "position startpos" and "go infinite searchmoves e2e4 d2d4"
    //    	the engine should only search the two moves e2e4 and d2d4 in the initial position.
    //    * ponder
    //    	start searching in pondering mode.
    //    	Do not exit the search in ponder mode, even if it's mate!
    //    	This means that the last move sent in in the position string is the ponder move.
    //    	The engine can do what it wants to do, but after a "ponderhit" command
    //    	it should execute the suggested move to ponder on. This means that the ponder move sent by
    //    	the GUI can be interpreted as a recommendation about which move to ponder. However, if the
    //    	engine decides to ponder on a different move, it should not display any mainlines as they are
    //    	likely to be misinterpreted by the GUI because the GUI expects the engine to ponder
    //       on the suggested move.
    //    * wtime <x>
    //    	white has x msec left on the clock
    //    * btime <x>
    //    	black has x msec left on the clock
    //    * winc <x>
    //    	white increment per move in mseconds if x > 0
    //    * binc <x>
    //    	black increment per move in mseconds if x > 0
    //    * movestogo <x>
    //        there are x moves to the next time control,
    //    	this will only be sent if x > 0,
    //    	if you don't get this and get the wtime and btime it's sudden death
    //    * depth <x>
    //    	search x plies only.
    //    * nodes <x>
    //       search x nodes only,
    //    * mate <x>
    //    	search for a mate in x moves
    //    * movetime <x>
    //    	search exactly x mseconds
    //    * infinite
    //    	search until the "stop" command. Do not exit the search without being told so in this mode!

    std::istringstream instream(in);
    std::string arg;

    int search_time = DEFAULT_SEARCH_TIME;

    while ( instream >> arg && arg != "movetime" ) {
        instream >> arg; // eat 'movetime'
        search_time = std::stoi(arg);
    }

    // Move m = gptr->searchFor(search_time);
    // return m.toUCI();
    Move m = gptr->getFirstMove();
    return m.toUCI();
}

std::string parse_position(std::string in, Game* gptr) {
    std::istringstream instream(in);
    std::string arg;

    // Load the fen
    instream >> arg;
    if ( arg == "startpos" ) {
        gptr->loadFen(INITIAL_FEN);
    } else {
        instream >> arg; // eat "fen"
        std::string fen = "";
        // fen
        fen.append(arg);
        fen.append(" ");
        // stm
        instream >> arg;
        fen.append(arg);
        fen.append(" ");
        // castling
        instream >> arg;
        fen.append(arg);
        fen.append(" ");
        // ep
        instream >> arg;
        fen.append(arg);
        fen.append(" ");
        // falfmoves
        instream >> arg;
        fen.append(arg);
        fen.append(" ");
        // fullmoves
        instream >> arg;
        fen.append(arg);
        gptr->loadFen(fen);
    }

    instream >> arg; // eat "moves"
    while ( instream >> arg ) gptr->makeUCIMove(arg);

    // TODO : remove me, for debugging
    std::cout << "In position:\n" << gptr->getBoardPtr()->stringify();
    return "";
}

std::string parse_make(std::string in, Game* gptr) {
    std::istringstream instream(in);
    std::string arg;

    // Load the move
    instream >> arg;
    gptr->makeUCIMove(arg);

    // TODO : remove me, for debugging
    std::cout << "In position:\n" << gptr->getBoardPtr()->stringify();
    return "";
}

const std::regex ISREADY_REGEX("isready");
const std::regex GO_REGEX("go\\s?(.*)");
const std::regex POSITION_REGEX("position\\s?(.*)");
const std::regex MAKE_REGEX("make\\s?(.*)");

// Parse UCI input
std::string parse(std::string in, Game* gptr) {
    if ( in == "isready" ) return "readyok";
    std::smatch match;
    // TODO :
    // * setoption name <id> [value <x>]
	  //     this is sent to the engine when the user wants to change the internal parameters
	  //     of the engine. For the "button" type no value is needed.
	  //     One string will be sent for each parameter and this will only be sent when the engine is waiting.
	  //     The name and value of the option in <id> should not be case sensitive and can inlude spaces.
	  //     The substrings "value" and "name" should be avoided in <id> and <x> to allow unambiguous parsing,
	  //     for example do not use <name> = "draw value".
	  //     Here are some strings for the example below:
	  //        "setoption name Nullmove value true\n"
    //         "setoption name Selectivity value 3\n"
	  //        "setoption name Style value Risky\n"
	  //        "setoption name Clear Hash\n"
	  //        "setoption name NalimovPath value c:\chess\tb\4;c:\chess\tb\5\n"

    if ( std::regex_search(in, match, GO_REGEX) ) return parse_go(match[1].str(), gptr);
    if ( std::regex_search(in, match, POSITION_REGEX) ) return parse_position(match[1].str(), gptr);
    if ( std::regex_search(in, match, MAKE_REGEX) ) return parse_make(match[1].str(), gptr);

    return "UNKNOWN";
}

int main(int argc, char* argv[]) {
    const char* socket_path = DEFAULT_SOCKET_PATH;
    int size = DEFAULT_MEMSIZE;

    // Check if a custom socket path is provided as a command line argument
    if (argc >= 2) {
        socket_path = argv[1];
    }

    if (argc >= 3) {
        size = std::stoi(argv[2]);
    }

    init();

    std::cout << "Starting game with memory " << static_cast<float>(size)/1e6 << "Mb\n";
    Game game(size);

    // Remove the existing socket file if it exists
    unlink(socket_path);

    // Step 1: Create a Unix domain socket
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Step 2: Bind the socket to the specified path
    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (bind(server_socket, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        std::cerr << "Error binding socket." << std::endl;
        close(server_socket);
        return 1;
    }

    // Step 3: Start listening for incoming connections
    if (listen(server_socket, 5) == -1) {
        std::cerr << "Error listening on socket." << std::endl;
        close(server_socket);
        return 1;
    }

    std::cout << "Server started. Waiting for connections..." << std::endl;

    // Step 4: Accept incoming connections and handle requests
    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == -1) {
            std::cerr << "Error accepting connection." << std::endl;
            continue;
        }
        std::string greeting = "READY\n";
        send(client_socket, greeting.c_str(), greeting.size(), 0);

        // Step 5: Read data from the client
        char buffer[1024];
        int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read == -1) {
            std::cerr << "Error reading from client." << std::endl;
            close(client_socket);
            continue;
        }

        // Step 6: Process the data (replace this with your actual processing logic)
        // For example, echoing back the data as the response.
        std::string received_data(buffer, bytes_read);
        received_data = received_data.substr(0,received_data.find("\n")); // strip off terminal "\n"
        std::string response = parse(received_data, &game);
        std::cout << "Received: " << received_data << std::endl;
        std::cout << "Sending: " << response << std::endl;
        response.append("\n");
        send(client_socket, response.c_str(), response.size(), 0);

        close(client_socket);
    }

    // Step 8: Close the server socket and remove the Unix socket file
    close(server_socket);
    unlink(socket_path);

    return 0;
}

