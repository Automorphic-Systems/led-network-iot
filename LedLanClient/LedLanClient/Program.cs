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

            var llHttpClient = new LedLanHttpClient();

            while (Console.ReadKey().Key != ConsoleKey.Q)
            {

                llHttpClient.SetFrame();

                Console.WriteLine("Frame message is sent!");
            }
        }
    }
}
