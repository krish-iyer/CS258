#!/usr/bin/env python3
import simpy
from random_gen import random_gen
import matplotlib.pyplot as plot


class server():
    def __init__(self, env, que_a, que_b, resp_a, resp_b):
        self.env = env
        self.size = len(que_a) + len(que_b)
        self.server_resp_a = []
        self.server_resp_b = []
        self.server_avg_resp = None
        self.server_std_resp = None
        self.end_time = None
        self.start_time = que_a[0] if que_a[0] < que_b [0] else que_b[0] 
        self.action = env.process(self.run(que_a, que_b, resp_a, resp_b))

    def run(self, que_a, que_b, resp_a, resp_b): 
        
        for i in range(self.size):
            
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
                            self.server_resp_a.append(resp_a[0])
                            que_a.pop(0)
                            resp_a.pop(0)
                        else:
                            yield self.env.timeout(resp_b[0])
                            #print("[Queue B {}] arr_t: {} ; resp {} ; now{}".format(i,que_b[0],resp_b[0], self.env.now))
                            self.server_resp_b.append(resp_b[0])
                            que_b.pop(0)
                            resp_b.pop(0)
                    
                    elif(self.env.now >= que_a[0]):
                        yield self.env.timeout(resp_a[0])
                        #print("[Queue A {}] arr_t: {} ; resp {} ; now{}".format(i,que_a[0],resp_a[0],self.env.now))
                        self.server_resp_a.append(resp_a[0])
                        que_a.pop(0)
                        resp_a.pop(0)

                    elif(self.env.now >= que_b[0]):
                        yield self.env.timeout(resp_b[0])
                        #print("[Queue B {}] arr_t: {} ; resp {} ; now{}".format(i,que_b[0],resp_b[0], self.env.now))
                        self.server_resp_b.append(resp_b[0])
                        que_b.pop(0)
                        resp_b.pop(0)

                        
                elif((len(que_a) != 0 and self.env.now >= que_a[0]) ):
                    yield self.env.timeout(resp_a[0])
                    #print("[Queue A {}] arr_t: {} ; resp {} ; now{}".format(i,que_a[0],resp_a[0],self.env.now))
                    self.server_resp_a.append(resp_a[0])
                    que_a.pop(0)
                    resp_a.pop(0)
                
                elif(len(que_b) != 0 and self.env.now >= que_b[0]):
                    yield self.env.timeout(resp_b[0])
                    #print("[Queue B {}] arr_t: {} ; resp {} ; now{}".format(i,que_b[0],resp_b[0], self.env.now))
                    self.server_resp_b.append(self.env.now - que_b[0])
                    que_b.pop(0)
                    resp_b.pop(0)
        
        self.end_time = self.env.now
        server_resp = sorted((self.server_resp_a) + (self.server_resp_b))
        self.server_avg_resp = random_gen.mean(server_resp)
        self.server_std_resp = random_gen.std(server_resp)       

if __name__ == "__main__":

    num_req = 100
    
    random_obj = random_gen()

    # arrival time : 5ms avg 
    # requests : 100
    queue = random_obj.random_poisson(5,num_req,False,True) 

    # make two queues for different requests 
    # size = 0.1*size_of_queue
    queue_a, queue_b = random_obj.random_select(queue, int(0.1*num_req))

    # response time for queue_a
    # lower bound : 3 ms
    # upper bound : 20 ms
    # size : len(queue_a)

    resp_list_a = random_obj.random_uniform(3,20,len(queue_a))


    # response time for queue_b
    # lower bound : 200 ms
    # upper bound : 1000 ms
    # size : len(queue_b)

    resp_list_b = random_obj.random_uniform(200,1000,len(queue_b))

    
    start_time = queue[0]
    runtime = sum(resp_list_a) + sum(queue_a) + sum(queue_b) + sum(resp_list_b)
    resp_l = list(resp_list_a)+list(resp_list_b)

    env = simpy.Environment(start_time)
    serv = server(env, queue_a, queue_b, list(resp_list_a), list(resp_list_b))    
    env.run(runtime)

    arrv_rate = num_req/(serv.end_time-serv.start_time)
    server_util = arrv_rate*(random_gen.mean(resp_l))
    
    print("####################################################################")    
    print("### Results ###")
    print("Arrival Rate : {} ".format(arrv_rate))
    print("Server Utilisation: {}".format(server_util))
    print("Server Average Response: {}".format(serv.server_avg_resp))
    print("Server Standard Deviation Response: {}".format(serv.server_std_resp))
    print("####################################################################")