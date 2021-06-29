using System;
using System.Collections.Generic;
using System.Text;

namespace LedLanClient
{
    class LedLanMQTTController
    {

        public LedLanMQTTController()
        {
        }


        // Create client and open connection/subscribe
        public async void Connect()
        {
            await LedLanMQTTClient.ConnectAsyc();
        }


        // Create call to publish message against the client 
        public async void Publish(string topic, string message)
        {
            await LedLanMQTTClient.PublishAsync(topic, message);
        }

        public async void Subscribe(string topic)
        {
            await LedLanMQTTClient.SubscribeAsync(topic);
        }
    }
}
