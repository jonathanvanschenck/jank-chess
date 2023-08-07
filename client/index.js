const { resolve, join } = require("path");
const { Logger } = require("yalls");

const { Engine } = require("./lib/Engine.js");
const { API } = require("./lib/API.js");
const { GameManager } = require("./lib/Game.js");

const env = require("./env.js");

const log = Logger.console("", env.log);
const engine = new Engine(env.engine, log);
const api = new API(env.api);
const gm = new GameManager(api, engine, log);

/**
 * Gracefully tear the client down
 *
 * @param {?string} [sig=undefined] The name of the signal that triggered this kill (if any)
 * @return {Promise}
 */
async function shutdown(sig) {
    return Promise.all([
        engine.stop(sig),
        gm.stop(),
    ]).then(() => {
        log.info("All processes stopped, exiting now.")
        process.exit(0); // TODO : we shouldn't need this, figure out what is hanging
    });
}


!async function(){

    // Start the engine first (since the init call inside the
    //  process takes a little while to execute)
    await engine.start();

    // Listen for signals to call shutdown (or just kill on the second signal)
    let once = false;
    for ( const sig of ["SIGINT", "SIGTERM"] ) {
        process.on(sig, () => {
            log.warn("Recieved "+sig+", shutting down...");
            if ( once ) {
                log.warn("Killing ungracefully");
                process.exit(1);
            }
            once = true;
            shutdown(sig);
        });
    }

    // Use a graceful shutdown on uncaught exceptions,
    //  otherwise the engine child process gets orphaned.
    process.on("uncaughtException", (err) => {
        console.error(err);
        shutdown();
    });

    // Start the Game manager last, since we want everything else up and running (ish)
    //  before we fire up any games
    await gm.start();
}()
