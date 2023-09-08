require('dotenv').config({ path: './.env' });

const env = process.env;

let log_format;
switch ( env.LOG_VERBOSITY ) {
    case "simple":
        log_format = ":TYPE | :STRING";
        break;
    case "short":
        log_format = ":TYPE - :NAMESPACE | :STRING";
        break;
    case "full":
        log_format = "[:ISO] :TYPE - :NAMESPACE | :STRING";
        break;
    default:
        log_format = null;
        break;
}

module.exports = {
    log : {
        log_level : env.LOG_LEVEL || 'debug',
        colorized : env.LOG_COLORIZED ? env.LOG_COLORIZED === 'true' : true,
        format : log_format
    },
    api: {
        url: env.URL,
        token: env.TOKEN
    },
    engine: {
        socket: env.ENGINE_SOCKET || "/tmp/chess",
        binary: env.ENGINE_BINARY || "../engine/build/engine1FirstMove",
        memory_size_bytes: parseInt((env.ENGINE_MEMEORY_SIZE_MB || 1)*1e6),
        dwell_time_ms: parseInt(env.ENGINE_DWELL_TIME || 2000),
    },
    game_manager: {
        stream: {
            auto_accept_challenge: ( env.GAME_STREAM_AUTO_ACCEPT_CHALLENGE || "true" ) == "true"
        }
    },
};
