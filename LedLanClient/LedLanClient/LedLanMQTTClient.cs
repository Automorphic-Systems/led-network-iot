using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Options;
using MQTTnet.Extensions.ManagedClient;

namespace LedLanClient
{
    public class LedLanMQTTClient
    {

        public static IManagedMqttClient client;

        private static string MQTT_URI = "192.168.1.10";
        private static string MQTT_USER;
        private static string MQTT_PASSWORD;
        private static int MQTT_PORT = 1883;
        private static bool MQTT_SECURE = false;


        public static async Task ConnectAsyc()
        {
            string clientId = Guid.NewGuid().ToString();

            var messageBuilder = new MqttClientOptionsBuilder()
                                .WithClientId(clientId)
                                .WithTcpServer(MQTT_URI, MQTT_PORT)
                                .WithCleanSession();

            var options = MQTT_SECURE ?
                        messageBuilder
                            .WithTls()
                            .Build() :
                       messageBuilder
                            .Build();

            var managedOptions = new ManagedMqttClientOptionsBuilder()
                                        .WithAutoReconnectDelay(TimeSpan.FromSeconds(5))
                                        .WithClientOptions(options)
                                        .Build();

            client = new MQTTnet.MqttFactory().CreateManagedMqttClient();

            client.UseConnectedHandler(e =>
            {
                Console.WriteLine("Connected successfully with MQTT Broker.");
            });

            client.UseDisconnectedHandler(e =>
            {
                Console.WriteLine("Disconnected from MQTT Brokers.");
                Console.WriteLine(e.Exception?.Message);
                Console.WriteLine(e.Reason);
            });

            client.UseApplicationMessageReceivedHandler(e =>
            {
                try
                {
                    string topic = e.ApplicationMessage.Topic;

                    if (string.IsNullOrWhiteSpace(topic) == false)
                    {
                        string payload = Encoding.UTF8.GetString(e.ApplicationMessage.Payload);
                        Console.WriteLine($"Topic: {topic}. Message Received: {payload}");
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message, ex);
                }
            });

            await client.StartAsync(managedOptions);

            Console.WriteLine("MQTT client started.");

        }

        public static async Task PublishAsync(string topic, string payload, bool retainFlag = true, int qos = 1) =>
            await client.PublishAsync(new MqttApplicationMessageBuilder()
                        .WithTopic(topic)
                        .WithPayload(payload)
                        .WithQualityOfServiceLevel((MQTTnet.Protocol.MqttQualityOfServiceLevel) qos)
                        .WithRetainFlag(retainFlag)
                        .Build());

        public static async Task PublishAsync(string topic, byte[] payload, bool retainFlag = true, int qos = 1) =>
            await client.PublishAsync(new MqttApplicationMessageBuilder()
                .WithTopic(topic)
                .WithPayload(payload)
                .WithQualityOfServiceLevel((MQTTnet.Protocol.MqttQualityOfServiceLevel) qos)
                .WithRetainFlag(retainFlag)
                .Build());

        public static async Task SubscribeAsync(string topic, int qos = 1) =>
            await client.SubscribeAsync(new MqttTopicFilterBuilder()
                .WithTopic(topic)
                .WithQualityOfServiceLevel((MQTTnet.Protocol.MqttQualityOfServiceLevel)qos)
                .Build());
    }
}
