
const { EventEmitter } = require("events");
const { request:http_request } = require("http");
const { request:https_request } = require("https");

class NDJSONStream extends EventEmitter {
    constructor({}={}) {
        super();
        this._input_string = "";
        this._success = false;
    }

    write(string) {
        for ( const partial of string.split("\n") ) {
            // Drop the '\r' from the previous message
            if ( this._success && partial[0] == "\r" ) partial = partial.slice(1);
            this._input_string = this._input_string + partial;

            let json;
            try {
                json = JSON.parse(this._input_string);
            } catch (e) {
                this._success = false;
                continue;
            }

            this._success = true;
            this._input_string = "";
            this.emit("json", json);
        }
    }

    close() {
        this.emit("closed");
    }
}

function request(method, url, { token=undefined, content_type="application/json", ...extras}={}, body='', stream_cb=null) {

    // Check the url protocol, also just make sure the url is valid.
    const parsed_url = new URL(url);
    const $ = { "http:":http_request, "https:":https_request }[parsed_url.protocol];
    if ( !$ ) throw new Error("Unsupported request protocol '"+parsed_url.protocol+"'");

    // Setup the options object
    let options = {
        method : method,
        headers: {}
    };

    // Handle Body
    let include_body = ( body && ['post','put','patch'].includes(method.toLowerCase()) );
    if ( include_body ) {
        if ( typeof(body) !== "string" ) body = JSON.stringify(body);
        options.headers['Content-Length'] = Buffer.byteLength(body);
        options.headers['Content-Type'] = content_type;
    }

    // Handle api keys
    if ( token )  options.headers['Authorization'] = `Bearer ${token}`;

    // Override if applicable
    options = Object.assign( options, extras );

    return new Promise((resolve, reject) => {

        let req = $(url, options, (res) => {

            res.type = res.headers['content-type'];
            try {
                res.type = res.type.match(/(application|text)\/([^;]+)/)[2];
            } catch (e) {
                // Ignore
            }

            res.setEncoding('utf-8');
            if ( res.type != "x-ndjson" ) {
                res.data = '';
                res.on('data', (chunk) => res.data = res.data + chunk);
                res.on('end', () => {
                    if ( res.type.includes("json") ) {
                        try {
                            res.data = JSON.parse( res.data || '' );
                        } catch (e) {
                            reject(e);
                            return;
                        }
                        resolve(res);
                    } else {
                        resolve(res);
                    }
                });
            } else {
                res.stream = new NDJSONStream();
                if ( stream_cb ) res.stream.on("json", stream_cb);
                res.on('data', (chunk) => res.stream.write(chunk));
                res.on('end', () => res.stream.close());
                resolve(res);
            }
        });

        req.on('error', reject);

        if ( include_body ) {
            req.write(body);
        }

        req.end();
    });
}

module.exports = { request, NDJSONStream };
