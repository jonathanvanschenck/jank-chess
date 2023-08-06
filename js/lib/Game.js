
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


        // TODO : add board representation?

        this.log = log?.create_child(this.id) ?? Logger.noop();
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
        // TODO
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
        // TODO
        if ( this.is_my_turn ) this.emit("awaiting_move");
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
        const jmoves = json.moves.split(" ");
        this.board.move(jmoves[jmoves.length - 1]);
        this.emit("move", jmoves[jmoves.length - 1], this.board.history.length);
        if ( this.is_my_turn ) this.emit("awaiting_move");
    }

    parse_message(json) {
        switch(json.type) {
            case "gameFull":
                // TODO
                this.parse_gameFull(json.game);
                break;
            case "gameState":
                // TODO
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
    constructor(api, log) {
        super();
        this.api = api;
        this.log = log?.create_child("GameStream") ?? Logger.noop();

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
                // TODO
                this.api.POST(`/api/challenge/${json.challenge.id}/accept`);
                break;
            case "challengeCanceled":
                // TODO
                break;
            case "challengeDeclined":
                // TODO
                break;
        }
    }

    ingest_game(game) {
        this.log.debug(`Starting new game: ${game.id}`);
        game.start().then(() => {
            this.games.set(game.id, game); // hold a reference
            game.once("stopped", () => this.games.delete(game.id));
            this.emit("game", game);
        });
    }
}

module.exports = {
    GameStream,
};
