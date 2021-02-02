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

Everything on the LED network does one of there things: capture input, process input, or receive/display a representation of that input.  Components on the network are decoupled as much as possible along the lines of these responsibilities. 

 - Installation:
	
		- An installation is the entire system taken as a whole. 
		- Users interact directly with the installation. 		
		- The installation can be represented as a directed acyclic graph (DAG) of defined components


# Design Constraints



# Performance Constraints



# Implementation Metrics


# Operationalization Guidelines

# Questions For Further Research

# Iteration Roadmap

# Related Repositories


