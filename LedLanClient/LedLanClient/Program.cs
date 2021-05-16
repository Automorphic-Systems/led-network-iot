using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Web;

namespace LedLanClient
{
    class Program
    {

        /// <summary>
        /// Receives input, awaits input commands, or runs a batch sequence
        /// of LED operations send to target nodes via MQTT (publish) or 
        /// HTTP (endpoint)
        /// 
        /// Eventually this will be configured to pass all of the invocations to MQTT 
        /// </summary>
        /// <param name="args"></param>
        static void Main(string[] args)
        {

            var llMqttController = new LedLanMQTTController();
            llMqttController.Connect();
            llMqttController.Subscribe("led/nodemcu");

            var keyRead = Console.ReadKey().Key;

            while (keyRead != ConsoleKey.Q)
            {                
                llMqttController.Publish("led/nodemcu", $"{keyRead.ToString()}: This is a test message");
                keyRead = Console.ReadKey().Key;
            }
        }
    }
}
