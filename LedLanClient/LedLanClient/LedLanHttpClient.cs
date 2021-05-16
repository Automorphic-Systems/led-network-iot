using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Http;
using System.Threading;
using System.Linq;

namespace LedLanClient
{
    /// <summary>
    /// Contains all of the service calls to available endpoints hosted by LedLan
    /// </summary>
    public class LedLanHttpClient
    {

        private static HttpClient httpClient;
        private static readonly string URI = "http://192.168.1.19:8000";
        private static readonly short FRAME_INTERVAL = 64;
        private static readonly short NUM_LEDS = 288;
        private static readonly short MAX_ITER = 1000;
        private static byte[] palette;

        public LedLanHttpClient()
        {
            httpClient = new HttpClient()
            {
                BaseAddress = new System.Uri(LedLanHttpClient.URI)
            };

        }

        public async void SetFrame()
        {
            try
            {
                var rand = new Random();                
                byte[] leddata = new byte[NUM_LEDS];


                for (int i = 0; i < MAX_ITER; i++)
                {
                    rand.NextBytes(leddata);

                    var binaryContent = new ByteArrayContent(leddata, 0, NUM_LEDS);

                    var binRequest = new HttpRequestMessage(HttpMethod.Post, $"{URI}/testsetframe")
                    {
                        Content = binaryContent
                    };

                    var result = httpClient.SendAsync(binRequest);

                    Thread.Sleep(FRAME_INTERVAL);                    
                }

            }
            catch (HttpRequestException hEx)
            {
                Console.WriteLine(hEx.Message);
                throw;
            }
        }

        public async void SetFramePalette()
        {
            try
            {
                var rand = new Random();

                palette = new byte[4];
                rand.NextBytes(palette);

                for (int i = 0; i < MAX_ITER; i++)
                {
                    int currentPos = 0;
                    byte[] leddata = new byte[NUM_LEDS];

                    while (currentPos < NUM_LEDS)
                    {
                        for (int k = 0; k < palette.Length; k++)
                        {
                            int block = rand.Next(4, 16);

                            int blockScope = block > (NUM_LEDS - currentPos) ? (NUM_LEDS - currentPos) : block;

                            for (int j = 0; j < blockScope; j++)
                            {
                                leddata[currentPos + j] = palette[k];
                            }

                            currentPos += blockScope;
                        }
                    }

                    var binaryContent = new ByteArrayContent(leddata, 0, NUM_LEDS);

                    var binRequest = new HttpRequestMessage(HttpMethod.Post, $"{URI}/testsetframe")
                    {
                        Content = binaryContent
                    };

                    var result = httpClient.SendAsync(binRequest);

                    Thread.Sleep(FRAME_INTERVAL);
                }

            }
            catch (HttpRequestException hEx)
            {
                Console.WriteLine(hEx.Message);
                throw;
            }
        }

        public async void SetFrameShiftPalette()
        {
            try
            {
                var rand = new Random();

                palette = new byte[4];
                rand.NextBytes(palette);
                int iter = 0;

                int currentPos = 0;
                byte[] leddata = new byte[NUM_LEDS];
                while (currentPos < NUM_LEDS)
                {
                    for (int k = 0; k < palette.Length; k++)
                    {
                        int block = rand.Next(4, 16);

                        int blockScope = block > (NUM_LEDS - currentPos) ? (NUM_LEDS - currentPos) : block;

                        for (int j = 0; j < blockScope; j++)
                        {
                            leddata[currentPos + j] = palette[k];
                        }

                        currentPos += blockScope;
                    }
                }

                while (iter < MAX_ITER) {

                    int range = rand.Next(0, 20);
                    int delta = rand.Next(0, 2);
                    Console.WriteLine(delta);
                    
                    for (int i=0; i < range; i++)
                    {
                        if (delta == 0)
                        {
                            //shift leds left   
                            leddata = leddata.Skip(1).Concat(leddata.Take(1)).ToArray();
                        } else
                        {
                            //shift leds right
                            leddata = leddata.Skip(leddata.Length-1).Concat(leddata.Take(leddata.Length-1)).ToArray();
                        }
                    }
                    
                    // Send frame
                    var binaryContent = new ByteArrayContent(leddata, 0, NUM_LEDS);

                    var binRequest = new HttpRequestMessage(HttpMethod.Post, $"{URI}/testsetframe")
                    {
                        Content = binaryContent
                    };

                    var result = httpClient.SendAsync(binRequest);

                    Thread.Sleep(FRAME_INTERVAL);
                }

            }
            catch (HttpRequestException hEx)
            {
                Console.WriteLine(hEx.Message);
                throw;
            }
        }

        public async void TestGet()
        {
            try
            {
                var result = await httpClient.GetAsync("/testget", HttpCompletionOption.ResponseContentRead);

                if (result.IsSuccessStatusCode)
                {
                    Console.WriteLine(result.Content.ReadAsStringAsync().Result);
                }

            }
            catch (HttpRequestException hEx)
            {
                Console.WriteLine(hEx.Message);
                throw;
            }

        }

        //public async string SetMode()
        //{

        //}


        //public async string SetConfig()
        //{

        //}

        //public async void Clear()
        //{

        //}

        //public async string GetHost()
        //{

        //}
    }
}
