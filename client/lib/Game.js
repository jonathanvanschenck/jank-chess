
const { EventEmitter } = require("events");
const { Logger } = require("yalls");
const { Chess } = require("chess.js");

const { NDJSONStream } = require("./request.js");
const { API } = require("./API.js");

class Game extends EventEmitter {
    constructor(json, api, log) {
        super();
        this.api = api;
        this._stop_called = false;

        this.id = json.gameId;

        this.board = new Chess(json.fen);
        this.my_color = json.color[0].toLowerCase();
        this.game_speed = json.perf;
        this.opponent = json.opponent.username;


        this.log = log?.create_child("G."+this.id) ?? Logger.noop();
    }

    get ply() {
        return 2*(this.board.moveNumber() - 1) + (this.board.turn() == "b" ? 1 : 0);
    }
    get fen() {
        return this.board.fen();
    }
    get moves() {
        return this.board.history({ verbose:true }).map(v => v.lan);
    }
    get is_my_turn() {
        return this.board.turn() == this.my_color;
    }

    static from_game_start_json(json, api, log) {
        // {
        //   "type": "gameStart",
        //   "game": {
        //     "gameId": "rCRw1AuO",
        //     "fullId": "rCRw1AuOvonq",
        //     "color": "black",
        //     "fen": "r1bqkbnr/pppp2pp/2n1pp2/8/8/3PP3/PPPB1PPP/RN1QKBNR w KQkq - 2 4",
        //     "hasMoved": true,
        //     "isMyTurn": false,
        //     "lastMove": "b8c6",
        //     "opponent": {
        //       "id": "philippe",
        //       "rating": 1790,
        //       "username": "Philippe"
        //     },
        //     "perf": "correspondence",
        //     "rated": true,
        //     "secondsLeft": 1209600,
        //     "source": "friend",
        //     "status": {
        //       "id": 20,
        //       "name": "started"
        //     },
        //     "speed": "correspondence",
        //     "variant": {
        //       "key": "standard",
        //       "name": "Standard"
        //     },
        //     "compat": {
        //       "bot": false,
        //       "board": true
        //     },
        //     "id": "rCRw1AuO"
        //   }
        // }
        return new this(json, api, log);
    }

    async start() {

        this.stream = await this.api.SGET(`/api/bot/game/stream/${this.id}`, (json) => this.parse_message(json)).then(resp => resp.stream);

        return this;
    }

    async stop() {
        if ( this._stop_called ) return;
        this._stop_called = true;


        // TODO : actually close this sucker

        this.emit("stopped");
    }

    async move(move, offer_draw=false) {
        // TODO : error check?
        return await this.api.POST(`/api/bot/game/${this.id}/move/${move}?${offer_draw?"offeringDraw=true":""}`).then(resp => resp.data.ok);
    }

    async abort() {
        return await this.api.POST(`/api/bot/game/${this.id}/abort`).then(resp => resp.data.ok);
    }

    async resign() {
        return await this.api.POST(`/api/bot/game/${this.id}/resign`).then(resp => resp.data.ok);
    }

    async handle_draw(accept) {
        return await this.api.POST(`/api/board/game/${this.id}/draw/${accept?"yes":"no"}`).then(resp => resp.data.ok);
    }

    async claim_victory() {
        return await this.api.POST(`/api/board/game/${this.id}/claim-victory`).then(resp => resp.data.ok);
    }

    parse_gameFull(json) {
        // if ( this.is_my_turn ) this.emit("awaiting_move");
    }
    parse_gameState(json) {
        if ( json.status != "started" ) {
            // TODO
            // {
            //   type: 'gameState',
            //   moves: 'd2d4 g7g6',
            //   wtime: 172742000,
            //   btime: 172800000,
            //   winc: 0,
            //   binc: 0,
            //   status: 'resign',
            //   winner: 'black'
            // }
            this.log.info("Ending game with status "+json.status);
            return this.stop();
        }
        // {
        //   type: 'gameState',
        //   moves: 'd2d4 g7g6',
        //   wtime: 172800000,
        //   btime: 172800000,
        //   winc: 0,
        //   binc: 0,
        //   status: 'started'
        // }
        const moves = this.moves.join(" ");
        if ( moves == json.moves ) return; // No change

        let i = 0;
        for ( const move of json.moves.split(" ").slice(this.ply) ) {
            i += 1;
            this.board.move(move);
            this.log.info("Move played: "+this.board.moveNumber()+". "+(this.board.turn()=="b"?"... ":"")+move);
            this.emit("move", move, this.ply);
        }
        this.log.debug("Now in position: "+this.board.fen());
        // if ( this.is_my_turn ) this.emit("awaiting_move");
    }

    parse_message(json) {
        switch(json.type) {
            case "gameFull":
                this.parse_gameFull(json.game);
                break;
            case "gameState":
                this.parse_gameState(json);
                break;
            case "chatLine":
                // TODO
                break;
            case "chatLineSpectator":
                // TODO
                break;
            case "opponentGoneTrue":
                // TODO
                break;
            case "opponentGoneFalse":
                // TODO
                break;
            case "gameStateResign":
                // TODO
                this.stop();
                break;
        }
    }
}

class GameStream extends EventEmitter {
    constructor(config, api, log) {
        super();
        this.config = config;
        this.api = api;
        this.log = log?.create_child("S") ?? Logger.noop();

        this.games = new Map();
    }

    async start() {
        this.stream = await this.api.SGET(`/api/stream/event`, (json) => this.parse_message(json)).then(resp => resp.stream);
    }

    async stop() {
        await Promise.all([...this.games.values()].map(g => g.stop()));
    }

    parse_message(json) {
        switch(json.type) {
            case "gameStart":
                this.ingest_game(Game.from_game_start_json(json.game, this.api, this.log));
                break;
            case "gameFinish":
                // TODO
                break;
            case "challenge":
                this.handle_challenge(json.challenge);
                break;
            case "challengeCanceled":
                // TODO
                break;
            case "challengeDeclined":
                // TODO
                break;
        }
    }

    handle_challenge(json) {
        if ( !this.config.auto_accept_challenge ) return;
        // TODO : include non-standard chess?
        if ( json.variant.key != "standard" ) {
            this.log.warn(`Receive challenge from ${json.challenger.name} for unsuported game of type ${json.variant.name}, declining offer`);
            return this.api.POST(`/api/challenge/${json.id}/decline`);
        }
        return this.api.POST(`/api/challenge/${json.id}/accept`);
    }

    ingest_game(game) {
        this.log.info(`Starting new game against ${game.opponent}: ${game.id}`);
        game.start().then(() => {
            this.games.set(game.id, game); // hold a reference
            game.once("stopped", () => this.games.delete(game.id));
            this.emit("game", game);
        });
    }
}

class GameManager extends EventEmitter {
    constructor(config, api, engine, log) {
        super();

        this.config = config;
        this.api = api;
        this.engine = engine;
        this.log = log?.create_child("GM") ?? Logger.noop();

        this.stream = new GameStream(this.config.stream, this.api, this.log);

        this.games = [];
    }

    get active_game() {
        return this.games[0];
    }

    async activate_game() {
        const g = this.active_game;
        if ( !g ) return;

        this.engine.setFEN(g.fen, this.activate_game.moves);
        g.on("move", (move) => {
            // this.engine.move(move);
            if ( g.is_my_turn ) {
                this.engine.setFEN(g.fen).then(() => this.engine.go().then(move => g.move(move)));
            }
        });
        if ( g.is_my_turn ) this.engine.go().then(move => g.move(move));
    }

    async start() {
        this.stream.on("game", (g) => {
            this.log.info(`Queuing Game ${g.id}`);
            this.games.push(g);

            // Activate if this game is first in queue
            if ( this.active_game?.id == g.id ) this.activate_game();

            // Attach clean up
            g.on("stopped", () => {
                this.log.info(`Game ${g.id} ended, removing from queue`);
                const idx = this.games.findIndex(gg => gg.id == g.id);
                if ( idx < 0 ) return; // already gone??
                this.games.splice(idx,1);

                // If the "stopped" game is the active one, activate a new game
                if ( idx == 0 ) this.activate_game();
            });
        });

        await this.stream.start();
    }

    async stop() {
        await this.stream.stop();
    }
}

module.exports = {
    GameStream,
    Game,
    GameManager,
};
