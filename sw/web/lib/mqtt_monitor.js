// JavaScript code for MQTT Monitor

var url = "";
var options = {};
var client;
var connected_flag = 1;
var reconnectTimeout = 300;

function set_status(status_str) {
    console.log(status_str);
    document.getElementById("status").innerHTML = status_str;
}

function set_message(message_str) {
    if (message_str != "") {
        console.log(message_str);
    }
    document.getElementById("messages").innerHTML = message_str;
}

function set_all(error_msg) {
    console.log(error_msg);
    document.getElementById("status").innerHTML = error_msg;
    document.getElementById("messages").innerHTML = error_msg;
}

function append_message(message_str) {
    console.log(message_str);
    document.getElementById("messages").innerHTML = message_str + "\n" + document.getElementById("messages").innerHTML;
}

function onConnectionLost() {
    connected_flag = 0;
    set_all("connection lost");
}

function onFailure(message) {
    connected_flag = 0;
    set_all("Connection Failed, Retrying")
    setTimeout(MQTTRetryConnect, reconnectTimeout); // Try again in reconnectTimeout ms
}

function onMessageArrived(data, topic) {
    out_msg = "Recv msg of topic '" + topic + "': " + data;
    append_message(out_msg);
}

function onConnect() {
    connected_flag = 1
    set_status("Connected");
    set_message("");
}

function MQTTRetryConnect() {
    // Check if disconnected. If we are already connected, skip so that there are not multiple instances
    if (connected_flag == 1) {
        return false;
    } else {
        return MQTTconnect();
    }
}

function MQTTconnect() {
    // Read user config
    url = document.forms["connform"]["server"].value;
    options = {
        // Clean session
        clean: true,
        connectTimeout: 64000,
        keepalive: 600,
        // Authentication
        clientId: document.forms["connform"]["cid"].value,
        username: document.forms["connform"]["name"].value,
        password: document.forms["connform"]["pass"].value,
    }

    console.log("connecting to " + url + " using : ");
    console.log(options);
    client = mqtt.connect(url, options)

    client.on('connect', function(connack) {
        if (connected_flag == 0) {
            onConnect();
        }
    })

    client.on('reconnect', function() {
        console.log('Reconnecting...')
    })

    client.on('close', function() {
        console.log('Closed, Reconnecting...')
    })

    client.on('disconnect', function(packet) {
        onConnectionLost()
    })

    client.on('error', function(error) {
        onFailure(error)
    })

    client.on('message', function(topic, payload, packet) {
        onMessageArrived(payload.toString(), topic)
    })

    return false;
}

function sub_topics() {

    if (connected_flag == 0) {
        set_message("Not Connected -- Subscribe failed");
        return false;
    }

    var stopic = document.forms["subs"]["Stopic"].value;

    client.subscribe(stopic, {
        qos: 0
    }, function(error, granted) {
        if (error) {
            console.log(error)
            append_message("Failed to subscribe to topic : " + stopic)
        } else {
            append_message("Subscribed to topic : " + stopic)
        }
    })

    return false;
}

function send_button_message(char) {
    if (connected_flag == 0) {
        set_message("Not Connected -- Button failed");
        return false;
    }

    var msg = char;
    var topic = "ntg337_pt8352/w2b";

    // Send a test message with QoS of 0 to the testtopic
    client.publish(topic, msg, {
        qos: 0,
        retain: false
    }, function(error) {
        if (error) {
            console.log(error)
        } else {
            append_message("Sent msg to '" + topic + "': " + msg)
        }
    })

    return false;
}

function send_message() {

    if (connected_flag == 0) {
        set_message("Not Connected -- Send failed");
        return false;
    }

    var msg = document.forms["smessage"]["message"].value;
    var topic = document.forms["smessage"]["Ptopic"].value;

    if (topic == "") {
        topic = "test-topic"
    }

    // Send a test message with QoS of 0 to the testtopic
    client.publish(topic, msg, {
        qos: 0,
        retain: false
    }, function(error) {
        if (error) {
            console.log(error)
        } else {
            append_message("Sent msg to '" + topic + "': " + msg)
        }
    })

    return false;
}
