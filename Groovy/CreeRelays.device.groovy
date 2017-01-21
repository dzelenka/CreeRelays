/**
 *  Garage Door Opener
 *
 *  Copyright 2017 Chris Hanson
 *
 *
metadata {
	definition (name: "Garage Door Opener", namespace: "dzelenka", author: "Chris Hanson") {
		capability "Button"
	}
}
*/
metadata {
	definition (name: "Garage Door Opener", namespace: "dzelenka", author: "Chris Hanson") {

		capability "Actuator"
		capability "Refresh"
		capability "Switch"
		capability "Switch Level"

		command "switch1on"
		command "switch2on"

		fingerprint profileId: "C05E", inClusters: "0000,0003,0004,0005,0006,0008,1000", outClusters: "0000,0019"
	}

	// simulator metadata
	simulator {
		// status messages
		status "on": "on/off: 1"
		status "off": "on/off: 0"

		// reply messages
		reply "zcl on-off on": "on/off: 1"
		reply "zcl on-off off": "on/off: 0"
	}

	// UI tile definitions
	tiles {
		standardTile("switch", "device.switch", width: 2, height: 2, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch.on", icon: "st.switches.light.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch.off", icon: "st.switches.light.on", backgroundColor: "#79b821"
		}
		standardTile("refresh", "device.switch", inactiveLabel: false, decoration: "flat") {
			state "default", label:"", action:"refresh.refresh", icon:"st.secondary.refresh"
		}
		controlTile("levelSliderControl", "device.level", "slider", height: 1, width: 3, inactiveLabel: false) {
			state "level", action:"switch level.setLevel"
		}
		valueTile("level", "device.level", inactiveLabel: false, decoration: "flat") {
			state "level", label: 'Active Relay ${currentValue}'
		}
        
		standardTile("switch1", "device.switch1", width: 1, height: 1, canChangeIcon: true, decoration: "flat") {
			state "default", label: "Relay 1", action: "switch1on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
		}

		standardTile("switch2", "device.switch2", width: 1, height: 1, canChangeIcon: true, decoration: "flat") {
			state "default", label: "Relay 2", action: "switch2on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
		}
        
		main(["switch"])
		details(["switch", "level", "levelSliderControl", "refresh","switch1","switch2"])
	}
}

// Parse incoming device messages to generate events

def parse(String description) {
	log.trace description
	if (description?.startsWith("catchall:")) {
		def msg = zigbee.parse(description)
		log.trace msg
		log.trace "data: $msg.data"
        
        if(description?.endsWith("0100") ||description?.endsWith("1001"))
        {
        	def result = createEvent(name: "switch", value: "on")
            log.debug "Parse returned ${result?.descriptionText}"
            return result
        }
        
        if(description?.endsWith("0000") || description?.endsWith("1000"))
        {
        	def result = createEvent(name: "switch", value: "off")
            log.debug "Parse returned ${result?.descriptionText}"
            return result
        }
	}
    
    
   if (description?.startsWith("read attr")) {
   		
        log.debug description[-2..-1]
        def i = Math.round(convertHexToInt(description[-2..-1]) / 256 * 100 )
        
		sendEvent( name: "level", value: i )
    }

}

def on() {
	log.debug "on()"
	sendEvent(name: "switch", value: "on")
    
	"st cmd 0x${device.deviceNetworkId} ${endpointId} 6 1 {}"
    }

def off() {
	log.debug "off()"
	sendEvent(name: "switch", value: "off")
    
	"st cmd 0x${device.deviceNetworkId} ${endpointId} 6 0 {}"
 
}

def refresh() {
    // Schedule poll every 1 min
    //schedule("0 */1 * * * ?", poll)
    //poll()
    
    [
	"st rattr 0x${device.deviceNetworkId} ${endpointId} 6 0", "delay 500",
    "st rattr 0x${device.deviceNetworkId} ${endpointId} 8 0"
    ]
}

def setLevel(value) {
	log.trace "setLevel($value)"
	def cmds = []

	if (value == 0) {
		sendEvent(name: "switch", value: "off")
		cmds << "st cmd 0x${device.deviceNetworkId} ${endpointId} 6 0 {0000 0000}"
	}
	else if (device.latestValue("switch") == "off") {
		sendEvent(name: "switch", value: "on")
	}

	sendEvent(name: "level", value: value)
    
    //Fix the "Less than 7%" dimming bug.  Code taken directly from the GE Link LED Bulb code
	//def level = new BigInteger(Math.round(value * 255 / 100).toString()).toString(16)
    value = (value * 255 / 100)
    def level = hex(value);
    cmds << "st cmd 0x${device.deviceNetworkId} ${endpointId} 8 4 {${level} 0000}"

	//log.debug cmds
	cmds
}

def configure() {

	String zigbeeId = swapEndianHex(device.hub.zigbeeId)
	log.debug "Confuguring Reporting and Bindings."
	def configCmds = [	
  
        //Switch Reporting
        "zcl global send-me-a-report 6 0 0x10 0 3600 {01}", "delay 500",
        "send 0x${device.deviceNetworkId} ${endpointId} 1", "delay 1000",
        
        //Level Control Reporting
        "zcl global send-me-a-report 8 0 0x20 5 3600 {0010}", "delay 200",
        "send 0x${device.deviceNetworkId} ${endpointId} 1", "delay 1500",
        
        "zdo bind 0x${device.deviceNetworkId} ${endpointId} 1 6 {${device.zigbeeId}} {}", "delay 1000",
		"zdo bind 0x${device.deviceNetworkId} ${endpointId} 1 8 {${device.zigbeeId}} {}", "delay 500",
	]
    return configCmds + refresh() // send refresh cmds as part of config
}

def uninstalled() {

	log.debug "uninstalled()"
		
	response("zcl rftd")
 
}

private getEndpointId() {
	new BigInteger(device.endpointId, 16).toString()
}


private hex(value, width=2) {
	def s = new BigInteger(Math.round(value).toString()).toString(16)
	while (s.size() < width) {
		s = "0" + s
	}
	s
}

private Integer convertHexToInt(hex) {
	Integer.parseInt(hex,16)
}

private String swapEndianHex(String hex) {
    reverseArray(hex.decodeHex()).encodeHex()
}

private byte[] reverseArray(byte[] array) {
    int i = 0;
    int j = array.length - 1;
    byte tmp;
    while (j > i) {
        tmp = array[j];
        array[j] = array[i];
        array[i] = tmp;
        j--;
        i++;
    }
    return array
}
def switch1on() {
	log.debug "Executing 'switch1on'"
	setLevel(1)
}
def switch2on() {
	log.debug "Executing 'switch2on'"
	setLevel(2)
}
