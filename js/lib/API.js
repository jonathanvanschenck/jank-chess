
const { EventEmitter } = require("events");
const { request } = require("./request.js");

class HTTPError extends Error {
    constructor(result) {
        super(`HTTP Error with code ${result.statusCode}`);
        this.code = result.statusCode;
        this.data = result.data;
        this.type = result.type;
    }

    static is_HTTPError(err) {
        return err.constructor.name === this.name;
    }
}

class API {
    #url;
    #token;
    constructor({ token, url }) {
        this.#url = url;
        this.#token = token;
    }

    async $(method, endpoint, extras={}, body, stream) {
        return request(
            method,
            this.#url + endpoint,
            Object.assign({ token:this.#token }, extras),
            body,
            stream
        ).then(res => {
            if ( res.statusCode > 299 ) {
                throw new HTTPError(res);
            }
            return res;
        });
    }
    async GET(endpoint, extras) { return this.$("get", endpoint, extras); }
    async POST(endpoint, body, extras) { return this.$("post", endpoint, extras, body); }
    async PATCH(endpoint, body, extras) { return this.$("patch", endpoint, extras, body); }
    async PUT(endpoint, body, extras) { return this.$("put", endpoint, extras, body); }
    async DELETE(endpoint, extras) { return this.$("delete", endpoint, extras); }

    async SGET(endpoint, stream_cb, extras) { return this.$("get", endpoint, extras, "", stream_cb); }
    async SPOST(endpoint, body, stream_cb, extras) { return this.$("post", endpoint, extras, body, stream_cb); }
    async SPATCH(endpoint, body, stream_cb, extras) { return this.$("patch", endpoint, extras, body, stream_cb); }
    async SPUT(endpoint, body, stream_cb, extras) { return this.$("put", endpoint, extras, body, stream_cb); }
    async SDELETE(endpoint, stream_cb, extras) { return this.$("delete", endpoint, extras, "", stream_cb); }
}

module.exports = {
    HTTPError,
    API,
};
