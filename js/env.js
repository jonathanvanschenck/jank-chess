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
        binary: env.ENGINE_BINARY || "../src/server.bin",
        memory_size_bytes: parseInt((env.ENGINE_MEMEORY_SIZE_MB || 1)*1e6),
    },
};
