using System;
using System.Collections.Generic;
using System.Text;
using System.Net.WebSockets;
using System.Threading;
using System.Threading.Tasks;

namespace LedLanClient
{
    public class LedLanWSClient
    {
        private static string WS_URI = "192.168.1.10";
        private static string WS_USER;
        private static string WS_PASSWORD;
        private static int WS_PORT = 81;

        // TODO: make into thread-safe singleton
        private static ClientWebSocket socket;
        public static string message;

        public static async Task ConnectAsync()
        {
            using (socket = new ClientWebSocket())
                try
                {
                    await socket.ConnectAsync(new Uri(WS_URI), CancellationToken.None);

                    await Send(socket, "Connection established!");
                    await Receive(socket);

                }
                catch (Exception ex)
                {
                    Console.WriteLine($"ERROR - {ex.Message}");
                }

        }

        static async Task Send(ClientWebSocket socket, string data) =>
            await socket.SendAsync(Encoding.UTF8.GetBytes(data), WebSocketMessageType.Text, true, CancellationToken.None);

        static async Task Send(ClientWebSocket socket, byte[] data) =>
            await socket.SendAsync(data, WebSocketMessageType.Text, true, CancellationToken.None);


        static async Task Receive(ClientWebSocket socket)
        {
            var buffer = new ArraySegment<byte>(new byte[2048]);
            do
            {
                WebSocketReceiveResult result;
                using (var ms = new System.IO.MemoryStream())
                {
                    do
                    {
                        result = await socket.ReceiveAsync(buffer, CancellationToken.None);
                        ms.Write(buffer.Array, buffer.Offset, result.Count);
                    }
                    while (!result.EndOfMessage);

                    if (result.MessageType == WebSocketMessageType.Close)
                        break;

                    ms.Seek(0, System.IO.SeekOrigin.Begin);

                    // TODO: need to inject type of reader (streamreader or binaryreader)
                    using (var reader = new System.IO.StreamReader(ms, Encoding.UTF8))
                        message = await reader.ReadToEndAsync();
                }
            } while (true);
        }

        public static async Task PublishAsync(string data)
        {
            await Send(socket, data);
        }

        public static async Task PublishAsync(byte[] data)
        {
            await Send(socket, data);
        }

    }
}
