# led-network-iot
Architecture artifacts for the Ambient LED Network project - a supporing architecture aimed at implementing large scale ambient LED installations.  The name for this project is a placeholder until I come up with something more interesting. 

# What This Repository Contains.

- Artifacts related to network and hardware design where applicable 
- Deployment resources and scripts where applicable
- Any other non-software based artifacts that are require version control

# Vision.

The idea behind the LED network is to take physical, real world input and translate it to interaction with a network collection of LED systems. Rather than being a spectator to an LED installation, participants in an immersive project are explicitly a part of it just by being in its presence. Important design features include ease of connectivity, horizontal scaling, and programmability of operations.  The resulting network allows for the creation of immersive and ambient systems at scale, and allowing artists flexibility over how interactive LED systems will behave in an installation.  To do this seamless introduces a number of technical challenges that parallel the typical challenges associated with LED installations.

With the LED network, designers are able to build out large scale interactive LED systems, and with defining LED visualizations made substantially easier, the locus of the work will shift to the "front-end" i.e. working with artists to set up sensors or other feedback-generating mechanisms in their art/workpieces.  With the ability to scale the system while limiting some of the hardware issues intrinsic to LEDs, larger interactive systems can be constructed with greater stability. 

# Design Goals.

There are four design goals that shape this endeavor, roughly in this order:

- Physical world capture : The LED network will capture data from the physical world, and cause it to perform some meaningful operation on an LED display.  The input source
	can be a camera, a sensor, a gesture, or some other device that takes input from the physical world and translates it into a signal.
		
- Horizontal scaling: Rather than being concerned over how many LEDs an individual system can drive, focus is shifted to horizontal scaling of LEDs.  This reduces issues that typically arise with larger volume LED systems (voltage drop, syncing, data loss)

- Near real time response: The LED network will capture data, operate on it, and send it to the LED display with minimal latency.   		
		
- Ease of implementation: Adding individual commodity components to the network should require little effort.  Data capture should be simple to retrieve.  Programming the rules for responding to interactive input should be straightforward with minimal programming know-how.


# Component Definitions

Everything on the LED network does one of there things: capture input, process input, or render a representation of that input.  Components on the network are decoupled as much as possible along the lines of these responsibilities. 

 - Installation:
	
		- An installation is the entire system taken as a whole. 
		- Users interact directly with the installation. 		
		- The installation can be represented as a directed acyclic graph (DAG) of defined components

 - Node:
	
		- Nodes are IP addressable units of hardware that can accept commands and LED frame data and respond with telemetry or metadata
		- Nodes must be able to consume MQTT messages
		- Nodes must be able connect via wi-fi (e.g. via ESP8266 or ESP32)
		- Nodes must be arduino compatible 
		- Nodes can both publish and subscribe to the controller
		- The network can support as many nodes as the IP space will allow 
		
 - Controller:
	
		- A controller is a unit of hardware that is responsible for orchestrating commands and directing them to different nodes		
		- Controllers must support and run an MQTT broker	
	        - Controllers must be able to accept commands and requests from nodes and direct them to other nodes
		- The LED network allows for one logical controller, but that controller can utilize multiple devices to support broker clustering
		- Controllers must be able to capture telemetry input from nodes		
		- Controllers must be able to receive data from translators that are subscribed to it		
		
- Constellation:
	
		- A constellation is a logical aggregation of multiple topologies.  Taken as a whole they represent the entire LED system to be driven and displayed. 
		- Exactly one constellation exists on the network and is the destination for messages coming from exactly one controller.  				
	
 - Topology:
	
		- Topologies are a physical organization of LEDs supported by the node, such as a matrix, strip, circle, or some other geometric arrangement.
		- One node drives exactly one topology.  		
		- Topologies may reflect two or three dimensional organization of LEDs.   		
		- Topologies are hardware agnostic to the extent that LEDs can be supported by the components driving them.  
		
 - Frame:
	
		- Frames are a stateful representation of the LEDs described in a topology	
		- Frames at minimum indicate the color of each LED (i.e. the LED's state)
		- There is no upper bound on the number of LEDs in a frame; limitations here would be imposed by the hardware itself (ws-spec, voltage, memory, utilization, length of wire)		
		- A frame describes the state of LEDs in exactly one topology
							
- Recipe:
	
		- Recipes are set of instructions that can be executed over the frame. 
		- Recipes accept a frame as input and return a frame as output. 
		- Recipes are issued by the controller		
		- Recipes can be chained together/composited, as long as the size of the frame remains constant
		- Recipes can include simple mapping effects, neural networks, iterative systems, complex events, generative functions, etc. 		
		- There are no logical limits to how many recipes can be applied by the topology to the frame 

- Source:
	
		- Sources are any input that produces a data signal in real time.		
		- Sources can be physical sensors, gestures, camera capture, etc. 		
		- Sources are not connected to the network directly. Data from sources is relayed by way of collectors.		
	
- Collectors:
	
		- Collectors are any logical or hardware devices that take input from a source and pass it on to the controller		
		- Collectors may receive data streams from multiple, heterogeneous sources. 		
		- Collectors may also "flatten" data streams from multiple sources and pass on a single stream		
		- The network can support multiple Collectors, all of which publish to the controller 		
		- Collectors may publish data streams to a translator
		
- Translators:
	
		- A translator is a logical component that accepts input from the collector and converts it to output to be published by the controller.
		- A translator requires three things:  the data from the collector, the topology on which it intends to operate, a recipe which determines how the 
		data will be manipulated. 		
		- A translator publishes the result directly to the controller.		
		- Translators can (and must) be scaled out, as they do the computational heavy lifting between data stream and visual output


# Design Constraints

Design constraints are constraints on the structure of the network itself.  These are modestly fluid as the architecture evolves in its nascent stages.

- Installations are fully localized. Installations may potentially be set up in remote, offline locations.  Therefore all compute/processing resources need to remain on-prem.
	Telemetry data may be broadcast remotely. 
	
- The conduit between capture and display should be as seamless as possible to a designer.  At most designers should be able to declare how data capture should influence display

- The delay between capture from a source and rendering to an LED node should be minimized, and may depend on the type of interaction being processed.
	
- Collectors, controllers, translators, and nodes should be completely decoupled and allow for horizontal scaling. 
	
- Error detection/correction is strongly desired but not an immediately design goal. 
	
- Currently the architecture supports single participant mode, but needs to eventually support concurrent participants interacting with different aspects of the installation.
	
- The choice of hardware in the system should minimally impact the ability to carry out operations required by the installation
		
- Specific limits are imposed on the types of hardware that the network can support.

- Make appropriate tradeoffs between efficiency (getting data close to the metal) and robustness (fault tolerance, error correction, etc.)

# Performance Constraints

 - Processing must happen in real or sub-second time, from the point at which a sensor emits a signal to when an LED is activated.  Cause and effect must be clear to the participant; too much latency makes it difficult to distinguish whether a visualization was created as a result of a user’s interaction or due to other environmental noise.  

 - Haptic/Video/Audio/Ambient feedback must be responsive (<150ms, the point after which delay becomes perceptible by humans)
	
 - LED displays must render at a minimum of 30fps, regardless of size or scale. For example, if one reserves 4 bytes for a single LED (RGB + PWM), a 10000 LED installation would require raw throughput of 4x8x30x10000 ~ 9.6 Mbps. 

# Implementation Metrics

A fledgling list of metrics used to analyze the LED network 
	
  - Frame throughput rate (frames/sec)
  - Maximum frame size 
  - Latency between nodes  
  - MQTT publish rate
  - Latency between controller and node
  - Network throughput between controller and node


# Operationalization Guidelines
	
  - Do not make users think, unless they want to.  The user should have to do nothing more than stumble upon an installation. 
  - Make hardware "invisible".  To a participant it should look like magic. 
  - Define project component boundaries early and often.  Separability is key to being able to scale.   	
  - Abstractions must solve a real problem today, not an imaginary problem tomorrow. 
  - Harden existing implementations first before running off and building new things.		
  - Architecture should dictate the choice of technologies, not the other way around. 

# Questions For Further Research
  
  1. Is separation between collector, translator, and recipe fully necessary?
  2. How does a translator know about the details of a topology?	
  3. How will the network be described logically? 
  4. What latency does horizontal scaling introduce as more and more nodes/controllers are attached to the system?
  5. Is MQTT even the right solution? Would raw UDP communication be better?  What about a different layer, such as Websockets?
  
# Iteration Roadmap (Tentative)

The first stages of the roadmap might look as follows

Iteration 1: 	Simple network
    
    - 2 nodes, 1 controller
    - Basic end to end communication with MQTT
  
Iteration 2:	Initial spec of frames/topologies  

    - 2 nodes, 1 controller 
    - Intermediate support for inter-node communication 
    
Iteration 3: 	Complex network
    
    - 2 topologies (each with 2 nodes), 1 controller
    - Intermediate support for inter-node communication 
    - Multiple topics supported by the broker
    
# Related Repositories

(in progress)

# References

(in progress)
