const { resolve, join } = require("path");
const { Logger } = require("yalls");

const { Engine } = require("./lib/Engine.js");
const { API } = require("./lib/API.js");
const { GameManager } = require("./lib/Game.js");

const env = require("./env.js");

let [ socket ] = process.argv.slice(2);
if ( !socket ) socket = "/tmp/chess";

const log = Logger.console("", env.log);
const engine = new Engine(env.engine, log);

!async function(){

    await engine.start();

    const api = new API(env.api);
    const gm = new GameManager(api, engine, log);

    const kill = (sig) => {
        return Promise.all([
            engine.stop(sig),
            gm.stop(),
        ]).then(() => {
            log.info("All processes stopped, exiting now.")
            process.exit(0)
        });
    }

    let once = false;
    for ( const sig of ["SIGINT", "SIGTERM"] ) {
        process.on(sig, () => {
            log.warn("Recieved "+sig+", shutting down...");
            if ( once ) {
                log.warn("Killing ungracefully");
                process.exit(1);
            }
            once = true;
            kill(sig);
        });
    }
    process.on("uncaughtException", (err) => {
        console.error(err);
        kill();
    });


    await gm.start();
}()
