#!/usr/bin/env node

var mqtt = require('mqtt');


const HOST_URL = 'mqtt://otis.leandog.com:52122';
const USERNAME = 'meetup';
//const PASSWORD = '<replace_with_password>';
const PASSWORD = 'YqVhtVE9ogss';

//const UID = '<replace_with_your_name_or_handle>';
const UID = 'garyjohnson';
const ONLINE_TOPIC = '/boat/meetup/' + UID + '/online';
const STATUS_TOPIC = '/boat/meetup/' + UID + '/status';


var mqttClient = null;

function connectToMqtt() {
    var will = { topic: ONLINE_TOPIC, payload:'0', retain: true };
    mqttClient = mqtt.connect(HOST_URL, { clientId: UID, username: USERNAME, password: PASSWORD, will: will });
}

function epochTime() {
    return Math.round(new Date().getTime()/1000.0);
}

connectToMqtt();

mqttClient.on('connect', function () {
    console.log('mqtt connected!');

    mqttClient.publish(ONLINE_TOPIC, '1', {retain: true});

    var message = UID + ' is sending messages from node.js!';
    var payload = {'statusMessage': message, 'lastUpdated': epochTime()}
    payload_json = JSON.stringify(payload);
    mqttClient.publish(STATUS_TOPIC, payload_json, {retain: true});

    mqttClient.subscribe('/boat/meetup/#');
});

mqttClient.on('disconnect', function() {
    connectToMqtt();
});

mqttClient.on('message', function (topic, message) {
    console.log('Got topic \"' + topic + '\" with payload \"' + message + '\"');
});

