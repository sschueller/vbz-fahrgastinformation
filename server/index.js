let fetch = require('node-fetch');
let moment = require('moment-timezone');

const config = require(__dirname + '/config.json');

const { URLSearchParams } = require('url');

moment.locale('de-ch');
let date = moment().tz('Europe/Zurich').format('L');
let time = moment().tz('Europe/Zurich').format('LT');

// Pulled from https://online.fahrplan.zvv.ch/bin/stboard.exe/dn
//let reqstationS0ID = 'A=1@O=Zürich, Sonneggstrasse@X=8543690@Y=47382834@U=85@L=008591373@B=1@p=1579676729@'
const params = new URLSearchParams();
params.append('dirInput', config.direction);
params.append('maxJourneys', 8);
params.append('input', config.station);
//params.append('REQStationS0ID', reqstationS0ID);
params.append('time', time);
params.append('date', date);
params.append('boardType', 'dep');
params.append('start', 1);
params.append('tpl', 'stbResult2json');

fetch("https://online.fahrplan.zvv.ch/bin/stboard.exe/dny?_ts=" + (new Date().getTime()), {
    "credentials": "include",
    "headers": {
        "accept": "application/json, text/javascript, */*; q=0.01",
        "accept-language": "en-US,en;q=0.9,de;q=0.8",
        "cache-control": "no-cache",
        "content-type": "application/x-www-form-urlencoded; charset=UTF-8",
        "pragma": "no-cache",
        "sec-fetch-mode": "cors",
        "sec-fetch-site": "same-origin",
        "x-requested-with": "XMLHttpRequest"
    }, "referrer": "https://online.fahrplan.zvv.ch/bin/stboard.exe/dn",
    "referrerPolicy": "no-referrer-when-downgrade",
    "body": params,
    "method": "POST",
    "mode": "cors"
}).then(response => {
    return response.json();
}).then(function (res) {
    let output = [];
    res.connections.forEach(connection => { 
        if (connection.mainLocation.countdown >= 0) {            
            output.push({
                line: connection.product.line.replace(/\s/g, ''),
                dest: shortenAndCleanString(connection.product.direction, 13),
                tta: connection.mainLocation.countdown,
                h: (connection.attributes_bfr && connection.attributes_bfr[0] && connection.attributes_bfr[0].code === "NF") ? true : false
            })
        }
    })    
    console.log(JSON.stringify(output));
}).catch(err => { console.log(err); });

function shortenAndCleanString(string, n) {
    string = string.replace(/^Z&#252;rich,/g, '').replace(/Bahnhof /g,'').trim();
    return (string.length > n) ? string.substr(0, n-1) + '.' : string;
}
