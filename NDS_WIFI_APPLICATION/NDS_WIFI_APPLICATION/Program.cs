using System;
using System.Net;
using System.Threading;
using System.Linq;
using System.Text;
using System.IO;


namespace SimpleWebServer
{
    class Program
    {
        public static string getIP()
        {

            IPHostEntry host;
            string localIP = "";
            host = Dns.GetHostEntry(Dns.GetHostName());
            foreach (IPAddress ip in host.AddressList)
            {
                if (ip.AddressFamily.ToString() == "InterNetwork")
                {
                    localIP = ip.ToString();
                }
            }
            return localIP;
        }

        //parse incoming data
        public string GetRequestPostData(HttpListenerRequest request)
        {
            if (!request.HasEntityBody)
            {
                return null;
            }
            using (System.IO.Stream body = request.InputStream) // here we have data
            {
                using (System.IO.StreamReader reader = new System.IO.StreamReader(body, request.ContentEncoding))
                {
                    return reader.ReadToEnd();
                }
            }
        }

        static void Main(string[] args)
        {
            string server_ip = "http://"+getIP()+"/"; //http://localhost:8080/test/
            WebServer ws = new WebServer(SendResponse, server_ip);
            ws.Run();
            Console.WriteLine("A simple webserver, mounted @ IP " + server_ip + ". Press a key to quit.");
            while (1 == 1)
            {
                ConsoleKeyInfo keys = Console.ReadKey();
                if (keys.Key == ConsoleKey.Enter)
                {
                    Console.Write("enterr \n");
                }
                else if (keys.Key == ConsoleKey.Escape)
                {
                    Console.Clear();
                }
            }
            ws.Stop();
            
        }

        public static string SendResponse(HttpListenerRequest request)
        {
            return string.Format("<HTML><BODY>My web page.<br>{0}</BODY></HTML>", DateTime.Now);
        }

        
    }

    public class WebServer
    {
        Program program_inst = new Program();
        private readonly HttpListener _listener = new HttpListener();
        private readonly Func<HttpListenerRequest, string> _responderMethod;
        
        public WebServer(string[] prefixes, Func<HttpListenerRequest, string> method)
        {
            
            if (!HttpListener.IsSupported)
                throw new NotSupportedException(
                    "Needs Windows XP SP2, Server 2003 or later.");

            // URI prefixes are required, for example 
            // "http://localhost:8080/index/".
            if (prefixes == null || prefixes.Length == 0)
                throw new ArgumentException("prefixes");

            // A responder method is required
            if (method == null)
                throw new ArgumentException("method");

            foreach (string s in prefixes)
                _listener.Prefixes.Add(s);

            _responderMethod = method;
            _listener.Start();
        }

        public WebServer(Func<HttpListenerRequest, string> method, params string[] prefixes)
            : this(prefixes, method) { }

        Program prog_inst = new Program();
        public void Run()
        {
            ThreadPool.QueueUserWorkItem((o) =>
            {
                Console.WriteLine("Webserver running...");
                try
                {
                    while (_listener.IsListening)
                    {
                        ThreadPool.QueueUserWorkItem((c) =>
                        {
                            var ctx = c as HttpListenerContext;
                            
                            //handles request and log them
                            try
                            {
                                //dont work /stream empty
                                //var bytes = new byte[ctx.Request.InputStream.Length];
                                //ctx.Request.InputStream.Read(bytes, 0, bytes.Length);
                                //ctx.Request.InputStream.Position = 0;
                                //string content = Encoding.ASCII.GetString(bytes);

                                //dont work /stream empty
                                //StreamReader reader = new StreamReader(ctx.Request.InputStream);
                                //string s = reader.ReadToEnd();
                                //Console.WriteLine(s);
                                
                                //Console.WriteLine(ctx.Request.RawUrl); //has the values sent from client (form for example)
                                Console.WriteLine(ctx.Request.Headers);
                                //Console.WriteLine(prog_inst.GetRequestPostData(ctx.Request));                 
                            }
                            catch { }
                            
                            //handles responses
                            try
                            {
                                string rstr = _responderMethod(ctx.Request);
                                byte[] buf = Encoding.UTF8.GetBytes(rstr);
                                ctx.Response.ContentLength64 = buf.Length;
                                ctx.Response.OutputStream.Write(buf, 0, buf.Length);
                                //Console.WriteLine(rstr); is response to target client
                            }
                            catch { } // suppress any exceptions
                            
                            finally
                            {
                                // always close the stream
                                ctx.Response.OutputStream.Close();
                            }

                        }, _listener.GetContext());
                    }
                }
                catch { } // suppress any exceptions
            });
        }

        public void Stop()
        {
            _listener.Stop();
            _listener.Close();
        }
    }
}