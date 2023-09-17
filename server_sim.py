from random_gen import random_gen

class server():
    def __init__(self, env, que_a, que_b, resp_a, resp_b):
        self.env = env
        self.size = len(que_a) + len(que_b)
        self.server_resp_a = resp_a.copy()
        self.server_resp_b = resp_b.copy()
        self.server_avg_resp = None
        self.server_std_resp = None
        self.end_time = None
        self.start_time = que_a[0] if que_a[0] < que_b [0] else que_b[0] 
        self.arrival_rate = None
        self.server_util = None
        self.server_resp = []
        self.misc_avg_resp = None
        self.action = env.process(self.run(que_a, que_b, resp_a, resp_b))

        
    def run(self, que_a, que_b, resp_a, resp_b): 
        
        while (len(que_a) + len(que_b)) != 0:
            
            if(len(que_a) != 0 or len(que_b) != 0):
                
                if((len(que_a) == 0 and self.env.now < que_b[0]) or (len(que_b) == 0 and self.env.now < que_a[0]) or \
                        (len(que_a) != 0 and len(que_b) != 0 and self.env.now < que_a[0] and self.env.now < que_b[0])):
                    
                    if (len(que_a) != 0 and len(que_b) != 0 ):
                        if(que_a[0] < que_b[0]):
                            yield self.env.timeout(que_a[0] - self.env.now)
                        else:
                            yield self.env.timeout(que_b[0] - self.env.now)
                    elif(len(que_a) == 0):
                        yield self.env.timeout(que_b[0] - self.env.now)
                    elif(len(que_b) == 0):
                        yield self.env.timeout(que_a[0] - self.env.now)
                        
                if((len(que_a) != 0) and (len(que_b) != 0)):
                    if(self.env.now >= que_a[0] and self.env.now >= que_b[0]):
                        if(que_a[0] < que_b[0]):
                            yield self.env.timeout(resp_a[0])
                            #print("[Queue A {}] arr_t: {} ; resp {} ; now{}".format(i,que_a[0],resp_a[0],self.env.now))
                            self.server_resp.append(self.env.now - que_a[0])
                            que_a.pop(0)
                            resp_a.pop(0)
                        else:
                            yield self.env.timeout(resp_b[0])
                            #print("[Queue B {}] arr_t: {} ; resp {} ; now{}".format(i,que_b[0],resp_b[0], self.env.now))
                            self.server_resp.append(self.env.now - que_b[0])
                            que_b.pop(0)
                            resp_b.pop(0)
                    
                    elif(self.env.now >= que_a[0]):
                        yield self.env.timeout(resp_a[0])
                        #print("[Queue A {}] arr_t: {} ; resp {} ; now{}".format(i,que_a[0],resp_a[0],self.env.now))
                        self.server_resp.append(self.env.now - que_a[0])
                        que_a.pop(0)
                        resp_a.pop(0)

                    elif(self.env.now >= que_b[0]):
                        yield self.env.timeout(resp_b[0])
                        #print("[Queue B {}] arr_t: {} ; resp {} ; now{}".format(i,que_b[0],resp_b[0], self.env.now))
                        self.server_resp.append(self.env.now - que_b[0])
                        que_b.pop(0)
                        resp_b.pop(0)

                        
                elif((len(que_a) != 0 and self.env.now >= que_a[0]) ):
                    yield self.env.timeout(resp_a[0])
                    #print("[Queue A {}] arr_t: {} ; resp {} ; now{}".format(i,que_a[0],resp_a[0],self.env.now))
                    self.server_resp.append(self.env.now - que_a[0])
                    que_a.pop(0)
                    resp_a.pop(0)
                
                elif(len(que_b) != 0 and self.env.now >= que_b[0]):
                    yield self.env.timeout(resp_b[0])
                    #print("[Queue B {}] arr_t: {} ; resp {} ; now{}".format(i,que_b[0],resp_b[0], self.env.now))
                    self.server_resp.append(self.env.now - que_b[0])
                    que_b.pop(0)
                    resp_b.pop(0)
        
        self.end_time = self.env.now
        server_resp_list = (list(self.server_resp_a) + list(self.server_resp_b))
        self.misc_avg_resp = random_gen.mean(server_resp_list)
        self.arrival_rate = (self.size)/(self.end_time-self.start_time)
        self.server_util = self.arrival_rate*(self.misc_avg_resp)
        self.server_std_resp = random_gen.std(self.server_resp)       
        self.server_avg_resp = random_gen.mean(self.server_resp)