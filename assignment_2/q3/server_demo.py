# This is the server that runs the demo website
# The below code is adapted from https://pythonbasics.org/webserver/ 
# and mofdified to the need of solving the HW
from http.server import BaseHTTPRequestHandler, HTTPServer
import time
#import cProfile

hostName = "localhost"
serverPort = 8080

class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write(bytes("<html><head><title>https://pythonbasics.org</title></head>", "utf-8"))
        self.wfile.write(bytes("<p>Request: %s</p>" % self.path, "utf-8"))
        self.wfile.write(bytes("<body>", "utf-8"))
        self.wfile.write(bytes("<p>This is a demo web server.</p>", "utf-8"))
        self.wfile.write(bytes("</body></html>", "utf-8"))

def server():
    webServer = HTTPServer((hostName, serverPort), MyServer)
    print("Server started http://%s:%s" % (hostName, serverPort))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")
if __name__ == "__main__":
    #cProfile.run("server()")   
    server() 
#    webServer = HTTPServer((hostName, serverPort), MyServer)
#    print("Server started http://%s:%s" % (hostName, serverPort))
#
#    try:
#        webServer.serve_forever()
#    except KeyboardInterrupt:
#        pass

#    webServer.server_close()
#    print("Server stopped.")
