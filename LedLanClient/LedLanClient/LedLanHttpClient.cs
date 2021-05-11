using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Http;
using System.Threading;

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

                    //for(int j = 0; j<NUM_LEDS; j++)
                    //{
                    //    Console.Write(leddata[j].ToString("X2") + ",");
                    //}

                    //Console.WriteLine();

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
