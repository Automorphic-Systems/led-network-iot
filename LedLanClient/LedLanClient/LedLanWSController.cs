using System;
using System.Collections.Generic;
using System.Text;
using System.Net.WebSockets;
using System.Threading;
using System.Threading.Tasks;

namespace LedLanClient
{
    public class LedLanWSController
    {

        public LedLanWSController()
        {

        }

        // Create client and open connection/subscribe
        public async void Connect()
        {
            await LedLanWSClient.ConnectAsync();
        }


        // Create call to publish message against the client 
        public async void Publish(string message)
        {
            await LedLanWSClient.PublishAsync(message);
        }

        public async void Publish(byte[] message)
        {
            await LedLanWSClient.PublishAsync(message);
        }

    }
}
