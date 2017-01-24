metadata {
	definition (name: "Virtual Button", namespace: "dzelenka", author: "Chris Hanson") {
		capability "Actuator"
		capability "Button"
        
        command "pushed"
	}

	simulator {

	}
	tiles {
		standardTile("button", "device.button", width: 1, height: 1, canChangeIcon: true) {
			state "default", label: "Button", icon: "st.Transportation.transportation12", backgroundColor: "#ffffff",
            action: "pushed"
		}

		main "button"
		details(["button"])
	}
}

def parse(String description) {
	
}

def pushed() {
	sendEvent(name: "button", value: "pushed", data: [buttonNumber: "1"], descriptionText: "Button was pushed", isStateChange: true)
}
