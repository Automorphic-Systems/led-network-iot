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

        private static string MQTT_URI;
        private static string MQTT_USER;
        private static string MQTT_PASSWORD;
        private static int MQTT_PORT;
        private static bool MQTT_SECURE;


        public LedLanMQTTClient()
        {
            // Todo: load from config
            MQTT_URI = "http://192.168.1.10";
            MQTT_USER = "splat";
            MQTT_PASSWORD = "splat";
            MQTT_PORT = 1883;
            MQTT_SECURE = false;
        }

        public static async Task ConnectAsyc()
        {
            string clientId = Guid.NewGuid().ToString();

            var messageBuilder = new MqttClientOptionsBuilder()
                                .WithClientId(clientId)
                                .WithCredentials(MQTT_USER, MQTT_PASSWORD)
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
                Console.WriteLine("Connected successfully with MQTT Brokers.");
            });

            client.UseDisconnectedHandler(e =>
            {
                Console.WriteLine("Disconnected from MQTT Brokers.");
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
    }
}
