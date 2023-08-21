
const { spawn } = require("child_process");
const { join } = require("path");
const { createConnection } = require("net");
const { EventEmitter } = require("events");
const { Logger } = require("yalls");

const { Chess } = require("chess.js");

class Engine extends EventEmitter {
    constructor(config, log) {
        super();

        this.config = config;
        this.log = log?.create_child("engine") ?? Logger.noop();

        // Fire up the engine
        this.controller = new AbortController();
        this.engine = spawn(
            join(__dirname,"..",this.config.binary),
            [this.config.socket, this.config.memory_size_bytes],
            { signal: this.controller.signal }
        );
        this.engine.stdout.on("data", (string) => string.toString().split("\n").forEach(d => { if (d) this.log.debug(d); }));
        this.engine.stderr.on("data", (string) => string.toString().split("\n").forEach(d => { if (d) this.log.error(d); }));
        this.engine.on("error", (err) => this.log.error(err.message + "\n" + err.stack));

        this.board = new Chess();
    }

    async start() {
        // Give the engine a sec to intialize
        await new Promise(res => setTimeout(res, 100));

        while ( true ) {
            try {
                if ( (await this.send("isready")) != "readyok" ) throw new Error("Engine didn't start");
                break;
            } catch(e) {
                this.log.warn("Engine is unresponsive, trying again soon\n"+e.message);
                await new Promise(res => setTimeout(res, 1000));
            }
        }
    }

    async stop(sig=null) {
        // Not actually sure if I need this??
        if ( !this.engine.kill(sig ?? "SIGINT") ) this.controller.abort();
        return Promise.all([
            new Promise(res => this.engine.once("close",res)),
        ])
    }

    async send(msg, timeout_ms=1000) {
        console.log(msg, timeout_ms);
        const con = createConnection(this.config.socket);

        await new Promise((res,rej) => {
            con.once("data", (data) => {
                if ( data.toString().match("READY") ) res();
            });
            con.on("error", rej);
            setTimeout(rej, 1000, new Error("Timeout"));
        });

        return new Promise((res, rej) => {
            con.once("data", (data) => res(data.toString().slice(0,-1)));
            con.write(msg+"\n");
            setTimeout(rej, timeout_ms, new Error("Timeout"));
        });
    }

    async setFEN(fen, moves=[]) {
        this.board.load(fen);
        await this.send(`position fen ${this.board.fen()}`);
        for ( const m of moves ) {
            this.move(m);
        }
    }

    async loadPGN(pgn_string) {
        this.board.loadPgn(pgn_string);
        await this.send(`position fen ${this.board.fen()}`);
    }

    async move(move) {
        if ( !this.board.moves({ verbose:true }).map(v => v.lan).includes(move) ) throw new Error("Invalid move "+move);
        await this.send(`make ${move}`);
        this.board.move(move);
    }

    async go(time_ms=null) {
        const move = await this.send(`go movetime ${time_ms || this.config.dwell_time_ms}`, (time_ms ?? this.config.dwell_time_ms)*3);
        return move;
    }

    get side_to_move() {
        return parseInt(c.turn() == "b")
    }
}

module.exports = { Engine };
